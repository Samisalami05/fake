#pragma once

#include "arraylist.h"
#include "fake.h"
#include <stdbool.h>

typedef struct {
	arraylist args; // of allocated char*
} command;

typedef struct {
	char* name;
	arraylist commands; // of command
	arraylist dependencies; // of allocated char*
} Label;

typedef struct  {
	char *file_str;
	size_t file_size;

	Tokens tokens;

	arraylist labels;
	size_t curr; // Current token
} parse_state;

// TODO: build parse tree for lsp

bool parse_fakefile(parse_state *state);
