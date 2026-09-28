#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "../arraylist.h"
#include "../log.h"

static char* sub(char* path, char* from, char* to) {
	int from_len = strlen(from);
	int from_start = -1;
	for (int i = 0; i < from_len; i++) {
		if (from[i] == '*') {
			from_start = i;
			break;
		}
	}
	if (from_start == -1) return NULL;
	int from_end = from_len - from_start - 1;

	int to_len = strlen(to);
	int to_start = -1;
	for (int i = 0; i < to_len; i++) {
		if (to[i] == '*') {
			to_start = i;
			break;
		}
	}
	if (to_start == -1) return NULL;
	int to_end = to_len - to_start - 1;

	int len = strlen(path);
	int middle = len - from_start - from_end;
	char* cstr = malloc(middle + to_start + to_end + 1);
	memcpy(cstr, to, to_start);
	memcpy(cstr + to_start, path + from_start, middle);
	memcpy(cstr + to_start + middle, to + to_len - to_end, to_end);
	cstr[middle + to_start + to_end] = '\0';

	return cstr;

}

bool builtin_pathsub(arraylist* args, int count, arraylist* out) {
	if (count != 3) {
		log_error("@pathsub(): Expected three arguments, got %d", count);
		return false;
	}

	arraylist paths = args[0];
	char* from = ((char**)args[1].items)[0];
	char* to = ((char**)args[2].items)[0];

	foreach (char*, path, paths) {
		char* new = sub(*path, from, to);
		if (!new) continue;
		arraylist_append(out, &new);
	}

	return true;
}
