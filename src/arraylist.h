#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
	uint8_t* items;
	size_t count;
	size_t capacity;

	size_t item_size;
} arraylist;

void arraylist_init(arraylist* list, size_t elem_size);
void arraylist_append(arraylist* list, void* elem);
void arraylist_deinit(arraylist* list);
void arraylist_clear(arraylist* list);
