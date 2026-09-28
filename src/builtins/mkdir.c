#include <stdbool.h>
#include <sys/stat.h>
#include "../arraylist.h"
#include "../log.h"

bool builtin_mkdir(arraylist* args, int count, arraylist* out) {
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

