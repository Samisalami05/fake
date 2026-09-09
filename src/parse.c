#include "parse.h"
#include "builtin.h"
#include "fake.h"
#include "arraylist.h"
#include "file.h"
#include "lex.h"
#include "log.h"
#include "str.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct  {
	FileView file;

	Tokens tokens;

	arraylist variables; // of Variable
	arraylist labels; // of Label
	size_t curr; // Current token
} ParseState;

StrRef get_token_id_str(ParseState* state, uint32_t token_id) {
	Lexer lexer = {
		.file = state->file.ptr,
		.file_size = state->file.size,
		.tokens.items = (uint8_t*)state->tokens.ptr,
		.tokens.count = state->tokens.count,
	};
	return lexer_token_id_str(&lexer, token_id);
}

char* get_token_id_cstr(ParseState* state, uint32_t token_id) {
	StrRef ref = get_token_id_str(state, token_id);
	char* cstr = malloc(ref.len + 1);
	memcpy(cstr, state->file.ptr + ref.src, ref.len);
	cstr[ref.len] = '\0';
	return cstr;
}

Token curr_token(ParseState* state) {
	return state->tokens.ptr[state->curr];
}

void parse_error(ParseState* state, char* fmt, ...) {
	char msg[128];

	va_list args;
	va_start(args, fmt);
	vsnprintf(msg, 128, fmt, args);
	va_end(args);

	StrRef ref = get_token_id_str(state, state->curr);
	log_file(LOG_ERROR, state->file, ref, msg);
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

Variable* find_variable(ParseState* state, StrRef name) {
	char* str = state->file.ptr + name.src;

	foreach (Variable, var, state->variables) {
		// TODO: might be undefined behaviour
		if (strlen(var->name) == name.len && memcmp(str, var->name, name.len) == 0) {
			return var;
		}
	}

	return NULL;
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

bool parse_line(ParseState* state, arraylist* out);

// This also executes the builtin function
bool parse_builtin(ParseState* state, arraylist* out) {
	StrRef str = get_token_id_str(state, state->curr);
	StrRef name = { str.src + 1, str.len - 1 };
	if (name.len <= 0) {
		state->curr--;
		parse_error(state, "There does not exist a builtin function without a name");
		return false;
	}

	state->curr++;
	
	if (!expect_token(state, TOKEN_PAREN_L)) return false;
	state->curr++;

	arraylist params = {0};
	if (!parse_line(state, &params)) return false;

	if (!expect_token(state, TOKEN_PAREN_R)) return false;
	state->curr++;

	char* cstr = str_cstr(state->file.ptr, name);
	if (!exec_builtin(cstr, params, out)) {
		parse_error(state, "Failed to execute builtin function '%s'", cstr);
		return false;
	}

	return true;
}

// 'out' is an arraylist of char*
bool parse_line(ParseState* state, arraylist* out) {
	arraylist_init(out, sizeof(char*));

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

		switch (state->file.ptr[str.src]) {
			case '$':; // variable
				StrRef name = { str.src + 1, str.len - 1 };
				if (name.len <= 0) {
					state->curr--;
					parse_error(state, "No name given to variable");
					return false;
				}

				Variable* var = find_variable(state, name);
				if (!var) {
					state->curr--;
					parse_error(state, "The variable '%.*s' does not exist", name.len, state->file.ptr + name.src);
					return false;
				}

				foreach(char*, value, var->values) {
					arraylist_append(out, value);
				}
				
				break;
			case '@':;  // builtin
				state->curr--;
				if (!parse_builtin(state, out)) return false;

				break;
			default:;   // other
				char* cstr = str_cstr(state->file.ptr, str);
				arraylist_append(out, &cstr);
		}
	}

	return true;
}

bool parse_command(ParseState* state, Command* cmd) {
	if (!parse_line(state, &cmd->args)) return false;

	if (cmd->args.count == 0) {
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
		memcpy(cstr, state->file.ptr + dep.src, dep.len);
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

	return true;
}

bool parse_variable(ParseState* state) {
	Variable var = {0};
	arraylist_init(&var.values, sizeof(char*));

	if (!expect_token(state, TOKEN_IDENTIFIER)) return false;
	var.name = get_token_id_cstr(state, state->curr);
	state->curr++;

	if (!expect_token(state, TOKEN_EQUALS)) return false;
	state->curr++;

	if (!parse_line(state, &var.values)) return false;

	if (!expect_token(state, TOKEN_COMMA)) return false;
	state->curr++;

	arraylist_append(&state->variables, &var);

	return true;
}

bool parse_statement(ParseState* state) {
	if (!expect_token(state, TOKEN_IDENTIFIER)) return false;
	state->curr++;

	TokenType type = curr_token(state).tag;
	state->curr--;

	switch (type) {
		case TOKEN_CURLY_L:
		case TOKEN_COLON: return parse_node(state);
		case TOKEN_EQUALS: return parse_variable(state);
		default:
			state->curr++;
			parse_error(state, "Expected ':', '{' or '=', got '%s'", token_tag_str(type));
			return false;
	}
	return true;
}

bool parse_fakefile(FileView file, Tokens tokens, Fakefile* out) {
	ParseState state = {0};
	state.file = file;
	state.tokens = tokens;
	arraylist_init(&state.variables, sizeof(Variable));
	arraylist_init(&state.labels, sizeof(Label));

	while (1) {
		if (curr_token(&state).tag == TOKEN_EOF) break;
		if (!parse_statement(&state)) return false;
	}

	out->labels = state.labels;
	out->variables = state.variables;

	return true;
}
