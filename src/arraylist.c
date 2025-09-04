#include "arraylist.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void arraylist_init(arraylist *list, uint32_t elem_c)
{
	list->ptr = NULL;
	list->count = 0;
	list->allocated = 0;
	list->size_per_elem = elem_c;
}

void resize(arraylist *list, uint32_t target) {
	list->allocated = target;
	list->ptr = realloc(list->ptr, list->allocated*list->size_per_elem);
}

void arraylist_append(arraylist *list, void *elem)
{
	resize(list, list->count+1);
	uint8_t *ptr = list->ptr;
	memcpy(&ptr[list->count*list->size_per_elem], elem, list->size_per_elem);
	list->count++;
}

// Todo: Use pointers to arraylist instead of &arraylist to eliminate valgrind error here.
void arraylist_deinit(arraylist *list) {
	if (list->allocated > 0) {
		free(list->ptr);
	}
}
