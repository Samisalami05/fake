#include <stdbool.h>
#include <string.h>
#include "../log.h"
#include "../arraylist.h"

bool builtin_concat(arraylist* args, int count, arraylist* out) {
	if (count != 2) {
		log_error("@concat(): Expected two arguments, got %d", count);
		return false;
	}

	arraylist values = args[0];
	arraylist adds = args[1];

	foreach (char*, value, values) {
		arraylist compacted = arraylist_new(sizeof(char));
		arraylist_appendn(&compacted, *value, strlen(*value));
		foreach (char*, add, adds) {
			int len = strlen(*add);
			arraylist_appendn(&compacted, *add, len);
		}
		char end = '\0';
		arraylist_append(&compacted, &end);
		arraylist_append(out, &compacted.items);
	}

	return true;
}
