#include <stdbool.h>
#include <string.h>

#include "../arraylist.h"
#include "../log.h"
#include "../builtin.h"

int builtin_dirname(arraylist* args, int count, arraylist* out) {
	if (count != 1) {
		log_error("@dirname(): Expected 1 argument, got %d\n", count);
		return BUILTIN_ERROR;
	}

	foreach (char*, arg, args[0]) {
		char* val = strdup(*arg);
		int len = strlen(val);
		char* end = strrchr(val, '/');
		if (end) len -= strlen(end);
		
		val[len] = '\0';
		arraylist_append(out, &val);
	}

	return BUILTIN_OK;
}
