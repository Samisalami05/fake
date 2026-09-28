#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "../arraylist.h"
#include "../log.h"

int mkdir_p(char *path, mode_t mode)
{
    size_t len = strlen(path);

    for (char *p = path + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';

            if (mkdir(path, mode) != 0)
				return -1;

            *p = '/';
        }
    }

    if (mkdir(path, mode) != 0)
		return -1;

    return 0;
}

bool builtin_mkdir(arraylist* args, int count, arraylist* out) {
	if (count != 1) {
		log_error("@mkdir(): Expected one arguments, got %d", count);
		return false;
	}

	arraylist paths = args[0];

	foreach (char*, path, paths) {
		mkdir_p(*path, 0755);
	}

	return true;
}

