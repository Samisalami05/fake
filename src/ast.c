#ifndef AST_H
#define AST_H

#include "fake.h"

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

#endif
