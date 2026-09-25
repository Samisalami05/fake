#include "builtin.h"
#include "arraylist.h"
#include "log.h"
#include "str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>

static bool match(char* str, arraylist patterns) {
	if (patterns.count == 0) return true;
	foreach (char*, pattern, patterns) {
		if (fnmatch(*pattern, str, 0) == 0) {
			return true;
		}
	}
	return false;
}

static bool find_rec(char* dir_name, arraylist patterns, arraylist* out) {
	DIR* dir = opendir(dir_name);
	if (!dir) {
		return false;
	}

	struct dirent* entry;
	while ((entry = readdir(dir))) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;

		int len = strlen(entry->d_name);
		int dir_len = strlen(dir_name);

		char* cstr = malloc(dir_len + 1 + len + 1);
		memcpy(cstr, dir_name, dir_len);
		cstr[dir_len] = '/';
		memcpy(cstr + dir_len + 1, entry->d_name, len);
		cstr[dir_len + 1 + len] = '\0';

		if (entry->d_type == DT_UNKNOWN) {
			struct stat s;
			if (stat(cstr, &s) == -1) {
				continue;
			}

			if (!S_ISDIR(s.st_mode) && match(cstr, patterns)) {
				arraylist_append(out, &cstr);
			}
		}
		else if (entry->d_type != DT_DIR) {
			if (match(cstr, patterns))
				arraylist_append(out, &cstr);
		}

		if (!find_rec(cstr, patterns, out))
			continue;
	}

	closedir(dir);
	return true;
}

static bool find(arraylist* args, int count, arraylist* out) {
	if (count == 0 || count > 2) {
		log_error("@find(): Expected one or two arguments, got %d", count);
		return false;
	}

	arraylist dirs = args[0];
	arraylist patterns = args[1];

	foreach (char*, dir_name, dirs) {
		if (!find_rec(*dir_name, patterns, out))
			log_error("@find(): Failed to open dir '%s'", *dir_name);
	}

	return true;
}

static bool echo(arraylist* args, int count, arraylist* out) {
	for (int i = 0; i < count; i++) {
		foreach (char*, param, args[i]) {
			printf("%s ", *param);
			//arraylist_append(out, param);
		}
	}
	printf("\n");
	return true;
}

static bool concat(arraylist* args, int count, arraylist* out) {
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

// src/*.c
// build/*.c
// src/main.c

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

static bool pathsub(arraylist* args, int count, arraylist* out) {
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

static bool b_mkdir(arraylist* args, int count, arraylist* out) {
	if (count != 1) {
		log_error("@mkdir(): Expected one arguments, got %d", count);
		return false;
	}

	arraylist paths = args[0];

	foreach (char*, path, paths) {
		mkdir(*path, 0755);
	}

	return true;
}


bool exec_builtin(const char* name, arraylist* args, int count, arraylist* out) {
	if (strcmp(name, "find") == 0) return find(args, count, out);
	else if (strcmp(name, "echo") == 0) return echo(args, count, out);
	else if (strcmp(name, "concat") == 0) return concat(args, count, out);
	else if (strcmp(name, "pathsub") == 0) return pathsub(args, count, out);
	else if (strcmp(name, "mkdir") == 0) return b_mkdir(args, count, out);
	return false;
}
