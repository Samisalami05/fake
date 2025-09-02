#include <stdlib.h>

#include "allocator.h"

arena* arena_init() {
	arena* a = malloc(sizeof(arena));
	arraylist_init(&a->data, 8);
}

void* arena_alloc(arena* a, size_t size) {
	void* p = malloc(size);
	arraylist_append(&a->data, p);
}

void arena_reset(arena* a) {
	arraylist_deinit(&a->data);
}

void arena_deinit(arena* a) {
	arraylist_deinit(&a->data);
	free(a);
}
