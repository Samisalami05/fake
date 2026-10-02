#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../arraylist.h"
#include "../log.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif

bool make_dir(const char *path) {
#ifdef _WIN32
	if (!CreateDirectory(path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
		return false;
	}
#else
	if (mkdir(path, 0755) == -1 && errno != EEXIST) {
		return false;
	}
#endif

	return true;
}

bool make_path(char *path) {
    size_t len = strlen(path);

    for (char *p = path + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';

            if (!make_dir(path)) 
				return false;

            *p = '/';
        }
    }

    if (!make_dir(path))
		return false;

    return true;
}

bool builtin_mkdir(arraylist* args, int count, arraylist* out) {
	if (count != 1) {
		log_error("@mkdir(): Expected one arguments, got %d", count);
		return false;
	}

	arraylist paths = args[0];

	foreach (char*, path, paths) {
		if (!make_path(*path)) {
			log_error("@mkdir(): Failed to create directory '%s'", *path);
		}
	}

	return true;
}

