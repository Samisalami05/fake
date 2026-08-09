#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
	uint8_t* items;
	size_t count;
	size_t capacity;

	size_t item_size;
} arraylist;

#define foreach(type, name, list) \
    for (type *name = (type *)(list).items; \
         name < (type *)((uint8_t *)(list).items + (list).count * (list).item_size); \
         ++name)

arraylist arraylist_new(size_t item_size);
void arraylist_init(arraylist* list, size_t item_size);
void arraylist_append(arraylist* list, void* item);
void arraylist_deinit(arraylist* list);
void arraylist_clear(arraylist* list);
