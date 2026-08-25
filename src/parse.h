#pragma once

#include "arraylist.h"
#include "fake.h"
#include "lex.h"
#include <stdbool.h>

typedef struct {
	arraylist args; // of allocated char*
} Command;

typedef struct {
	char* name;
	arraylist commands; // of Command
	arraylist dependencies; // of allocated char*
} Label;

typedef struct  {
	char *file_ptr;
	size_t file_size;

	Tokens tokens;

	arraylist labels; // of Label
	size_t curr; // Current token
} ParseState;

// TODO: build parse tree for lsp

bool parse_fakefile(ParseState *state);
