#include <stdbool.h>

#include "../arraylist.h"
#include "../log.h"
#include "../cache.h"
#include "../builtin.h"

int builtin_store(arraylist* args, int count, arraylist* out) {
	if (count != 2) {
		log_error("@store(): Expects two argument, got %d\n", count);
		return BUILTIN_ERROR;
	}
	
	arraylist values = args[1];

	foreach(char*, arg, args[0]) {
		cache_store(*arg, values);
	}

	return BUILTIN_OK;
}
