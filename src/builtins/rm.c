#include <stdbool.h>
#include "../arraylist.h"
#include "../log.h"

// TODO: 
// @rm()
// @rmdir()
// @rmall() or @rm_all() or @rmrf()

bool builtin_rm(arraylist* args, int count, arraylist* out) {
	if (count != 1) {
		log_error("@rm(): Expects one argument");
		return false;
	}

    foreach (char*, path, *args) {
        if (!remove_file(*path)) {
            continue;
        }
    }


    return true;
}
