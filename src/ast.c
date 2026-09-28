#ifndef AST_H
#define AST_H

#include "file.h"
#include "parse.h"
#include <stdio.h>

char* ast_type_cstr(AstNodeType type) {
	switch (type) {
		case AST_NODE_ROOT: return "root";
		case AST_NODE_VAR_DECL: return "var decl";
		case AST_NODE_LABEL: return "label";
		case AST_NODE_RULE: return "rule";
		case AST_NODE_MULTI: return "multi";
		case AST_NODE_CMD: return "command";
		case AST_NODE_DEPS: return "deps";
		case AST_NODE_NAMES: return "names";
		case AST_NODE_EXPRESSION: return "expression";
		case AST_NODE_ADD: return "add";
		case AST_NODE_SUB: return "sub";
		case AST_NODE_SIMPLE_EXPR: return "simple expr";
		case AST_NODE_IDENTIFIER: return "identifier";
		case AST_NODE_STRING: return "string";
		case AST_NODE_VAR_REF: return "var ref";
		case AST_NODE_BUILTIN: return "builtin";
		case AST_NODE_AUTOVAR: return "autovar";
	}
	return "?";
}

static uint32_t print_node(Ast* ast, FileView file, uint32_t node, uint32_t depth) {
	AstNode n = ast->data[node];
	printf("%3u | ", node);
	if (n.ref.src != UINT32_MAX) {
		 printf("%-7.*s | ", n.ref.len, file.ptr + n.ref.src); 
	}
	else printf("        | ");

	for (int i = 0; i < depth; i++) printf("  ");

	printf("%-10s", ast_type_cstr(n.type));
		printf("\n");

	uint32_t curr = node;
	for (int i = 0; i < n.child_count; i++) {
		curr = print_node(ast, file, curr + 1, depth + 1);
	}
	return curr;
}

void ast_debug(Ast* ast, FileView file) {
	printf("\nAst:\n");
	print_node(ast, file, AST_ROOT, 0);
}

#endif
