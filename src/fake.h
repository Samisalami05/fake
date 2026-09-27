#pragma once
#include "arraylist.h"
#include "str.h"

// TODO: arena allocator!

#include <stdint.h>

typedef enum {
	BLOCK_LABEL,
	BLOCK_RULE
} BlockType;

typedef struct {
	char* name;
	uint32_t node;
	BlockType type;
	arraylist deps; // of allocated char*
} Block;

typedef struct {
	arraylist variables; // of Variable
	arraylist labels; // of Label
} Fakefile;
