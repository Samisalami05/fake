#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../log.h"
#include "../arraylist.h"

bool builtin_repeat(arraylist* args, int count, arraylist* out) {
	if (count != 2) {
		log_error("@repeat(): Expected two arguments, got %d", count);
		return false;
	}

	arraylist values = args[0];
	int num = atoi(*(char**)args[1].items);

	for (int i = 0; i < num; i++) {
		foreach (char*, value, values) {
			arraylist_append(out, value);
		}
	}

	return true;
}
