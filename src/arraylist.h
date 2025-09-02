#pragma once

#include <stdint.h>

typedef struct {
	void *ptr;
	uint32_t count;
	uint32_t allocated;

	uint32_t size_per_elem;
} arraylist;

void arraylist_init(arraylist *list, uint32_t elem_c);
void arraylist_append(arraylist *list, void *elem);
void arraylist_deinit(arraylist *list);
