#include <stdbool.h>
#include "../arraylist.h"
#include "../log.h"

bool builtin_rmdir(arraylist* args, int count, arraylist* out) {
	if (count != 1) {
		log_error("@rmdir(): Expects one argument");
		return false;
	}

    foreach (char*, path, *args) {
        if (!remove_dir(*path)) {
            continue;
        }
    }

    return true;
}
