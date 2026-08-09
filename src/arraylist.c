#include "arraylist.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

arraylist arraylist_new(size_t item_size) {
	arraylist list = {0};
	arraylist_init(&list, item_size);
	return list;
}

void arraylist_init(arraylist* list, size_t item_size) {
	list->items = NULL;
	list->capacity = 0;
	list->count = 0;
	list->item_size = item_size;
}

void resize(arraylist* list, size_t target) {
	size_t old = list->capacity;
	while (target > list->capacity) {
		list->capacity = list->capacity == 0 ? 4 : list->capacity * 2;
	}

	if (old == list->capacity) return;
	list->items = realloc(list->items, list->capacity*list->item_size);
}

void arraylist_append(arraylist* list, void *item) {
	resize(list, list->count+1);
	memcpy(list->items + list->count*list->item_size, item, list->item_size);
	list->count++;
}

void arraylist_deinit(arraylist* list) {
	free(list->items);
	list->capacity = 0;
	list->count = 0;
}

void arraylist_clear(arraylist* list) {
	list->count = 0;
}
