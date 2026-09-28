#pragma once
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
} FakeRole;

typedef struct {
	FakeRole role;
	char* filename;
	arraylist targets;
} FakeConfig;

void parse_args(char **argv, FakeConfig* out);
