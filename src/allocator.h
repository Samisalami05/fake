#pragma once

#include <stdint.h>

#include "src/arraylist.h"

typedef struct {
	arraylist data;
} arena;

arena* arena_init();

void* arena_alloc(arena* a, size_t size);

// Frees all the allocated memory.
void arena_reset(arena* a);

// Frees all the allocated memory and deinits the arena.
void arena_deinit(arena* a);
