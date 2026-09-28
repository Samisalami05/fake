#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include "arraylist.h"
#include <stdint.h>
#include <stdbool.h>

/*
 * Argument parsing for fake
 *
 * Using arguments, users can set flags, decide what target to execute and get help
 */

typedef enum {
	FAKE_ROLE_RUN,   // default, run the targets
	FAKE_ROLE_LIST,  // list all labels
	FAKE_ROLE_DEBUG, // debug print
} FakeRole;

#define DEBUG_LEXER   (1 << 0)
#define DEBUG_AST     (1 << 1)
#define DEBUG_BLOCKS  (1 << 2)

typedef struct {
	FakeRole role;
	char* filename;
	arraylist targets;
	uint64_t debug_flags;
} FakeConfig;

void parse_args(char **argv, FakeConfig* out);

#endif
