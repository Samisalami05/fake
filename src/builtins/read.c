#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arraylist.h"
#include "../log.h"
#include "../cache.h"
#include "../builtin.h"

int builtin_read(arraylist* args, int count, arraylist* out) {
	if (count != 1) {
		log_error("@read(): Expects one argument, got %d\n", count);
		return BUILTIN_ERROR;
	}
	
	foreach(char*, arg, args[0]) {
		cache_read(*arg, out);
	}

	return BUILTIN_OK;
}
