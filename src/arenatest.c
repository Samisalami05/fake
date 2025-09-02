#include <stdio.h>
#include <stdlib.h>

#include "allocator.h"

int main() {
	printf("running\n");
	arena* a = arena_init();

	int* num = arena_alloc(a, sizeof(int));
	*num = 3;
	printf("%d\n", *num);

	arena_deinit(a);
}
