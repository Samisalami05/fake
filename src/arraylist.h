#pragma once

#include <stddef.h>
#include <stdint.h>

#define AL_APPEND(list, value) \
	do { \
		if (list.count + 1 > list.capacity) { \
			list.capacity = list.capacity == 0 ? 4 : list.capacity * 2; \
			void* tmp = realloc(list.data, list.capacity * sizeof(value)); \
			if (tmp == NULL) { \
				perror("Arraylist: AL_APPEND()"); \
			} \
			/* Have to do this in cpp :( */ \
			memcpy(&list.data, &tmp, sizeof(tmp)); \
		} \
		\
		memcpy(list.data + list.count, &value, sizeof(value)); \
		list.count++; \
	\
	} while (0)

#define AL_BACK(list) list.data[list.count - 1]

#define AL_POP_BACK(list) \
		if (list.count != 0) { \
			list.count--; \
		}

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
void arraylist_appendn(arraylist* list, void *items, size_t count);
void arraylist_deinit(arraylist* list);
void arraylist_clear(arraylist* list);
