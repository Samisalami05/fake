#include "../arraylist.h"
#include "../log.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../file.h"


#ifndef _WIN32
#include <sys/stat.h>
#include <fnmatch.h>
#endif

// TODO: Add [A-Z_0-9] matching
static bool match(const char* str, const char* pattern) {
	int str_pos = 0;
	int pos = 0;
	
	while (pattern[pos] && str[str_pos] && pattern[pos] != '*') {
		if (str[str_pos] != pattern[pos]) return false;
		str_pos++;
		pos++;
	}

	// TODO: This is probably incorrect
	if ((!str[str_pos] && !pattern[pos]) || !pattern[pos]) return true;

	pos++; // Skip the '*'

	char expected = pattern[pos];
	while (str[str_pos]) {
		if (str[str_pos] == expected) {
			if (!match(str + str_pos, pattern + pos)) return false;
		}
		str_pos++;
	}

	return true;
}

static bool matchn(char* str, arraylist patterns) {
	if (patterns.count == 0) return true;
	foreach (char*, pattern, patterns) {
		if (match(str, *pattern)) {
			return true;
		}
	}
	return false;
}

static bool find_rec(char* dir_name, arraylist patterns, arraylist* out) {
	Dir dir;
	if (!open_dir(dir_name, &dir))
		return false;

	DirEntry entry;
	while (read_dir(&dir, &entry)) {
		char* name = entry_name(&entry);
		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
			continue;

		int len = strlen(name);
		int dir_len = strlen(dir_name);

		char* cstr = malloc(dir_len + 1 + len + 1);
		memcpy(cstr, dir_name, dir_len);

		cstr[dir_len] = '/';

		memcpy(cstr + dir_len + 1, name, len);
		cstr[dir_len + 1 + len] = '\0';

		if (entry_is_dir(&entry)) {
			find_rec(cstr, patterns, out);
			continue;
		}

		if (matchn(cstr, patterns)) {
			arraylist_append(out, &cstr);
		}
	}

	close_dir(&dir);
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