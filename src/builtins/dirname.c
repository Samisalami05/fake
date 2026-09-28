#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../arraylist.h"
#include "../log.h"

bool builtin_dirname(arraylist* args, int count, arraylist* out) {
	if (count != 1) {
		log_error("@dirname(): Expected 1 argument, got %d\n", count);
		return false;
	}

	foreach (char*, arg, args[0]) {
		char* val = strdup(*arg);
		int len = strlen(val);
		char* end = strrchr(val, '/');
		if (end) len -= strlen(end);
		
		val[len] = '\0';
		arraylist_append(out, &val);
	}

	return true;
}
