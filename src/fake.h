#pragma once
#include "arraylist.h"

// TODO: arena allocator!

#include <stdint.h>

typedef enum {
	AST_NODE_VAR_DECL,
	AST_NODE_LABEL,
	AST_NODE_CMD,
	AST_NODE_IDENT,
	AST_NODE_VARIABLE,
	AST_NODE_BUILTIN,
	AST_NODE_STRING,
} AstNodeType;

typedef struct {
	AstNodeType type;
	char* name;

	uint32_t* children;
	size_t child_count;
} AstNode;

typedef struct {
	AstNode* nodes;
	size_t capacity;
	size_t count;

	arraylist labels; // of uint32_t
	arraylist vars;   // of uint32_t
} Ast;

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
