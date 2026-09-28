#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../arraylist.h"
#include "../log.h"

bool builtin_env(arraylist* args, int count, arraylist* out) {
	if (count > 1) {
		log_error("@env(): Expects one argument, got %d\n", count);
		return false;
	}

	foreach(char*, arg, args[0]) {
		char* env = getenv(*arg);
		if (!env) continue;

		int len = strlen(env);
		char* new = malloc(len + 1);
		strcpy(new, env);

		arraylist_append(out, &new);
	}

	return true;
}
