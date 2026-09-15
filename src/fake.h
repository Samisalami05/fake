#pragma once
#include "arraylist.h"
#include "str.h"
#include <gc.h>

// TODO: arena allocator!

#include <stdint.h>

typedef enum {
	AST_NODE_ROOT,

	AST_NODE_VAR_DECL,

	AST_NODE_LABEL,
	AST_NODE_RULE,
	AST_NODE_MULTI,

	AST_NODE_CMD,
	AST_NODE_DEP,

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

// VAR_DECL
//   IDENT
//   STR
//   VAR
//   BUILTIN

// LABEL
//   IDENT - dep
//   IDENT - dep
//   CMD
//   CMD

// CMD
//   IDENT
//   STR
//   VAR
//   BUILTIN

// BUILTIN
//   IDENT
//   STR
//   VAR
//   BUILTIN

// STR
//   IDENT
//   VAR

typedef struct {
	char* name;
	arraylist values; // of allocated char*
} Variable;

typedef enum {
	EXPRESSION_IDENT,
	EXPRESSION_STRING,
	EXPRESSION_BUILTIN,
} ExpressionType;

typedef struct {
	ExpressionType type;
	char* value;
} Expression;

typedef struct {
	arraylist args; // of allocated char*
} Command;

typedef struct {
	char* name;
	arraylist commands; // of Command
	arraylist dependencies; // of allocated char*
} Label;

typedef struct {
	arraylist variables; // of Variable
	arraylist labels; // of Label
} Fakefile;
