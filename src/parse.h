#pragma once

#include "arraylist.h"
#include "fake.h"
#include "file.h"
#include "lex.h"
#include <stdbool.h>

typedef enum {
	AST_NODE_ROOT,

	AST_NODE_VAR_DECL,

	AST_NODE_LABEL,
	AST_NODE_RULE,
	AST_NODE_MULTI,

	AST_NODE_CMD,
	AST_NODE_DEPS,
	AST_NODE_NAMES,

	// Expression
	AST_NODE_EXPRESSION,

	// Operators
	AST_NODE_ADD,
	AST_NODE_SUB,

	AST_NODE_SIMPLE_EXPR,

	AST_NODE_IDENTIFIER,
	AST_NODE_STRING,
	AST_NODE_VAR_REF,
	AST_NODE_BUILTIN,
	AST_NODE_AUTOVAR,
} AstNodeType;

typedef struct {
	AstNodeType type;

	StrRef ref;
	uint32_t child_count;
} AstNode;

#define AST_ROOT 0

typedef struct {
	AstNode* data;
	size_t capacity;
	size_t count;
} Ast;

char* ast_type_cstr(AstNodeType type);

bool parse_fakefile(FileView file, Tokens tokens, Ast* out);
