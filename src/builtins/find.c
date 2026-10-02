#include "../arraylist.h"
#include "../log.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>
#endif

static bool match(char* str, arraylist patterns) {
#ifdef _WIN32
	return true; // TODO: Implement this function for windows
#else
	if (patterns.count == 0) return true;
	foreach (char*, pattern, patterns) {
		if (fnmatch(*pattern, str, 0) == 0) {
			return true;
		}
	}
#endif
	return false;
}

static bool find_rec(char* dir_name, arraylist patterns, arraylist* out) {
#ifdef _WIN32
	return true; // TODO: Implement this function for windows
#else
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
#endif
	return true;
}

bool builtin_find(arraylist* args, int count, arraylist* out) {
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


