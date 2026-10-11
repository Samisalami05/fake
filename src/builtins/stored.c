#include <stdbool.h>

#include "../arraylist.h"
#include "../log.h"
#include "../cache.h"
#include "../builtin.h"

int builtin_stored(arraylist* args, int count, arraylist* out) {
	if (count != 1) {
		log_error("@stored(): Expects one argument, got %d\n", count);
		return BUILTIN_ERROR;
	}

	char* ret = "true";
	
	foreach(char*, arg, args[0]) {
		if (!cache_read(*arg, NULL)) {
			ret = "false";
		}
	}

	arraylist_append(out, &ret);

	return BUILTIN_OK;
}
