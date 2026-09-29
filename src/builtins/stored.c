#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../arraylist.h"
#include "../log.h"
#include "../cache.h"

bool builtin_stored(arraylist* args, int count, arraylist* out) {
	if (count != 1) {
		log_error("@stored(): Expects one argument, got %d\n", count);
		return false;
	}

	char* ret = "true";
	
	foreach(char*, arg, args[0]) {
		if (!cache_read(*arg, NULL)) {
			ret = "false";
		}
	}

	arraylist_append(out, &ret);

	return true;
}
