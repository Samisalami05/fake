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
	Ast ast;

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

uint32_t add_node(ParseState* state, uint32_t parent, AstNodeType type, StrRef ref) {
	if (parent != UINT32_MAX) 
		state->ast.data[parent].child_count++;

	AstNode node = {
		.type = type,
		.ref = ref,
		.child_count = 0,
	};

	AL_APPEND(state->ast, node);
	return state->ast.count - 1;
}

uint32_t add_node_empty(ParseState* state, uint32_t parent, AstNodeType type) {
	return add_node(state, parent, type, (StrRef){UINT32_MAX, UINT32_MAX});
}

uint32_t add_node_single(ParseState* state, uint32_t parent, AstNodeType type, uint32_t token) {
	return add_node(state, parent, type, get_token_id_str(state, token));
}

bool parse_simple_expr(ParseState* state, uint32_t dest);
bool parse_command(ParseState* state, uint32_t dest);
bool parse_commands(ParseState* state, uint32_t dest);
bool parse_deps(ParseState* state, uint32_t dest);
bool parse_decl(ParseState* state);
bool parse_statement(ParseState* state);

uint32_t print_node(Ast* ast, uint32_t node, uint32_t depth);

bool parse_fakefile(FileView file, Tokens tokens, Ast* out) {
	ParseState state = {0};
	state.file = file;
	state.tokens = tokens;

	add_node_empty(&state, UINT32_MAX, AST_NODE_ROOT);
	
	while (1) {
		if (curr_token(&state).tag == TOKEN_EOF) break;
		if (!parse_statement(&state)) return false;
	}

	for (int i = 0; i < state.ast.count; i++) {
		AstNode node = state.ast.data[i];
		char* str = state.file.ptr + node.ref.src;
		printf("%3d: %10s", i, ast_type_cstr(node.type));
		if (node.ref.src == UINT32_MAX) {
			printf("\n");
			continue;
		}
		printf(" - %.*s\n", node.ref.len, str);
	}
	print_node(&state.ast, 0, 0);

	*out = state.ast;

	return true;
}

bool parse_deps(ParseState* state, uint32_t dest) {
	while (curr_token(state).tag == TOKEN_IDENTIFIER) {
		add_node_single(state, dest, AST_NODE_DEP, state->curr);
		state->curr++;

		if (curr_token(state).tag != TOKEN_COMMA) break;
		state->curr++;
	}

	return true;
}

bool parse_simple_expr(ParseState* state, uint32_t dest) {
	TokenType tag = curr_token(state).tag;
	switch (tag) {
		case TOKEN_IDENTIFIER:
			add_node_single(state, dest, AST_NODE_IDENTIFIER, state->curr);
			break;
		case TOKEN_DOLLAR:
			state->curr++;
			if (!expect_token(state, TOKEN_IDENTIFIER)) return false;
			add_node_single(state, dest, AST_NODE_VAR_REF, state->curr);
			break;
		case TOKEN_STRING:;
			StrRef ref = get_token_id_str(state, state->curr);
			ref.src++;
			ref.len -= 2;
			add_node(state, dest, AST_NODE_STRING, ref);
			break;
		case TOKEN_AT_SIGN:
			state->curr++;
			if (!expect_token(state, TOKEN_IDENTIFIER)) return false;
			uint32_t id = add_node_single(state, dest, AST_NODE_BUILTIN, state->curr);
			state->curr++;

			if (!expect_token(state, TOKEN_PAREN_L)) return false;
			state->curr++;

			while (curr_token(state).tag != TOKEN_PAREN_R) {
				if (!parse_command(state, id)) return false;
				if (curr_token(state).tag == TOKEN_COMMA) state->curr++;
			}

			break;
		case TOKEN_HASHTAG:
			state->curr++;
			if (!expect_token(state, TOKEN_IDENTIFIER)) return false;
			add_node_single(state, dest, AST_NODE_AUTOVAR, state->curr);
			break;
		default:
			parse_error(state, "No expression given");
			return false;
	}
	state->curr++;

	return true;
}
bool parse_command(ParseState* state, uint32_t dest) {
	uint32_t id = add_node_empty(state, dest, AST_NODE_CMD);
	while (curr_token(state).tag != TOKEN_COMMA && curr_token(state).tag != TOKEN_CURLY_R && curr_token(state).tag != TOKEN_PAREN_R) {
		if (!parse_simple_expr(state, id)) return false;
	}

	return true;
}

bool parse_commands(ParseState* state, uint32_t dest) {
	while (curr_token(state).tag != TOKEN_CURLY_R) {
		if (!parse_command(state, dest)) return false;
		if (curr_token(state).tag == TOKEN_COMMA) state->curr++;
	}
	return true;
}

bool parse_decl(ParseState* state) {
	if (!expect_token(state, TOKEN_IDENTIFIER)) return false;
	AstNodeType decl_type = AST_NODE_RULE;

	StrRef type_str = get_token_id_str(state, state->curr);
	char* type = state->file.ptr + type_str.src;
	if (type_str.len == 5 && strncmp(type, "label", type_str.len) == 0) {
		decl_type = AST_NODE_LABEL;
	}
	else if (type_str.len == 5 && strncmp(type, "multi", type_str.len) == 0) {
		decl_type = AST_NODE_MULTI;
	}
	else if (type_str.len == 4 && strncmp(type, "rule", type_str.len) == 0) {
		decl_type = AST_NODE_RULE;
	}
	else {
		parse_error(state, "Unknown decl type '%.*s'", type_str.len, type);
		return false;
	}

	state->curr++;

	if (!expect_token(state, TOKEN_IDENTIFIER)) return false;
	int id = add_node_single(state, AST_ROOT, decl_type, state->curr);
	state->curr++;


	if (curr_token(state).tag == TOKEN_COLON) {
		state->curr++;
		if (!parse_deps(state, id)) return false;
	}

	if (!expect_token(state, TOKEN_CURLY_L)) return false;
	state->curr++;

	if (!parse_commands(state, id)) return false;

	if (!expect_token(state, TOKEN_CURLY_R)) return false;
	state->curr++;

	return true;
}

bool parse_var_decl(ParseState* state) {
	if (!expect_token(state, TOKEN_IDENTIFIER)) return false;
	int id = add_node_single(state, AST_ROOT, AST_NODE_VAR_DECL, state->curr);
	state->curr++;

	if (!expect_token(state, TOKEN_EQUALS)) return false;
	state->curr++;

	if (!parse_command(state, id)) return false;

	if (!expect_token(state, TOKEN_COMMA)) return false;
	state->curr++;

	return true;
}

bool parse_statement(ParseState* state) {
	if (!expect_token(state, TOKEN_IDENTIFIER)) return false;
	state->curr++;

	TokenType tag = curr_token(state).tag;
	state->curr--;
	switch (tag) {
		case TOKEN_EQUALS:
			if (!parse_var_decl(state)) return false;
			break;
		default:
			if (!parse_decl(state)) return false;
	}

	return true;
}

uint32_t print_node(Ast* ast, uint32_t node, uint32_t depth) {
	for (int i = 0; i < depth; i++) printf("  ");

	AstNode n = ast->data[node];
	printf("%s\n", ast_type_cstr(n.type));
	
	uint32_t curr = node;
	for (int i = 0; i < n.child_count; i++) {
		curr = print_node(ast, curr + 1, depth + 1);
	}
	return curr;
}
