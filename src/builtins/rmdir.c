#include <stdbool.h>

#include "../arraylist.h"
#include "../log.h"
#include "../builtin.h"

int builtin_rmdir(arraylist* args, int count, arraylist* out) {
	if (count != 1) {
		log_error("@rmdir(): Expects one argument");
		return BUILTIN_ERROR;
	}

    foreach (char*, path, *args) {
        if (!remove_dir(*path)) {
            continue;
        }
    }

    return BUILTIN_OK;
}
