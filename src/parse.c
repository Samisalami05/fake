#include "parse.h"
#include "fake.h"
#include "arraylist.h"
#include "lex.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

StrRef get_token_id_str(ParseState* state, uint32_t token_id) {
	Lexer lexer = {
		.file = state->file_ptr,
		.file_size = state->file_size,
		.tokens.items = (uint8_t*)state->tokens.ptr,
		.tokens.count = state->tokens.count,
	};
	return lexer_token_id_str(&lexer, token_id);
}

char* get_token_id_cstr(ParseState* state, uint32_t token_id) {
	StrRef ref = get_token_id_str(state, token_id);
	char* cstr = malloc(ref.len + 1);
	memcpy(cstr, state->file_ptr + ref.src, ref.len);
	cstr[ref.len] = '\0';
	return cstr;
}

Token curr_token(ParseState* state) {
	return state->tokens.ptr[state->curr];
}

FileView file_view(ParseState* state) {
	return (FileView){
		.ptr = state->file_ptr,
		.size = state->file_size,
	};
}

void parse_error(ParseState* state, char* fmt, ...) {
	char str[128];

	va_list args;
	va_start(args, fmt);
	vsnprintf(str, 128, fmt, args);
	va_end(args);

	StrRef ref = get_token_id_str(state, state->curr);
	printErr(file_view(state), ref, str);
}

bool expect_token(ParseState* state, TokenType token) {
	if (curr_token(state).tag != token) { 
		char* expected = token_tag_str(token);
		char* got = token_tag_str(curr_token(state).tag);
		parse_error(state, "Expected '%s' , got '%s'", expected, got);
		return false;
	}
	return true;
}

bool parse_string(ParseState* state, StrRef* out) {
	if (!expect_token(state, TOKEN_STRING)) return false;

	StrRef ref = get_token_id_str(state, state->curr);
	ref.src += 1;
	ref.len -= 2;
	*out = ref;

	state->curr += 1;
	return true;
}

bool parse_identifier(ParseState* state, StrRef* out) {
	*out = get_token_id_str(state, state->curr);
	state->curr++;
	return true;
}

bool parse_command(ParseState* state, Command* cmd) {
	arraylist_init(&cmd->args, sizeof(char*));

	int len = 0;
	while (1) {
		StrRef str = {0};
		TokenType type = curr_token(state).tag;
		if (type == TOKEN_STRING) {
			if (!parse_string(state, &str)) return false;
		}
		else if (type == TOKEN_IDENTIFIER) {
			if (!parse_identifier(state, &str)) return false;
		}
		else break;
		char* cstr = str_cstr(state->file_ptr, str); 
		arraylist_append(&cmd->args, &cstr);
		len++;
	}

	if (len == 0) {
		parse_error(state, "empty command, command cannot be empty");
		return false;
	}

	return true;
}

bool parse_node(ParseState *state) {
	Label node = {0};
	arraylist_init(&node.commands, sizeof(Command));
	arraylist_init(&node.dependencies, sizeof(char*));

	if (!expect_token(state, TOKEN_IDENTIFIER)) return false;
	node.name = get_token_id_cstr(state, state->curr);

	state->curr += 1;
	
	if (curr_token(state).tag == TOKEN_COLON) {
	state->curr += 1;

	// parse deps
	while (1) {
		TokenType first_tag = curr_token(state).tag;
		if (first_tag == TOKEN_PAREN_R) break;

		if (!expect_token(state, TOKEN_IDENTIFIER)) return false;
		StrRef dep = get_token_id_str(state, state->curr);
		state->curr += 1;

		char* cstr = malloc(dep.len + 1);
		memcpy(cstr, state->file_ptr + dep.src, dep.len);
		cstr[dep.len] = '\0';
		
		arraylist_append(&node.dependencies, &cstr);

		if (curr_token(state).tag == TOKEN_CURLY_L) break;
		if (!expect_token(state, TOKEN_COMMA)) return false;
		state->curr += 1;
	}
	}

	// parse body
	if (!expect_token(state, TOKEN_CURLY_L)) return false;
	state->curr += 1;

	// parse commands
	while (1) {
		if (curr_token(state).tag == TOKEN_CURLY_R) break;

		Command c = {0};
		if (!parse_command(state, &c)) return false;
		arraylist_append(&node.commands, &c);
		
		if (curr_token(state).tag == TOKEN_CURLY_R) break;
		if (!expect_token(state, TOKEN_COMMA)) return false;
		state->curr += 1;
	}
	
	if (!expect_token(state, TOKEN_CURLY_R)) return false;
	state->curr += 1;

	arraylist_append(&state->labels, &node);

	return state->curr;
}

bool parse_fakefile(ParseState *state) {
	while (1) {
		if (curr_token(state).tag == TOKEN_EOF) break;
		if (!parse_node(state)) return false;
	}
	return true;
}


