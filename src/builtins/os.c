#include <stdbool.h>
#include "../arraylist.h"
#include "../log.h"

bool builtin_os(arraylist* args, int count, arraylist* out) {
	if (count != 0) {
		log_error("@os(): Expects no arguments");
		return false;
	}

	char* os;

	// Source - https://stackoverflow.com/a/5920028
	// Posted by Evgeny Gavrin, modified by community. See post 'Timeline' for change history
	// Retrieved 2026-09-28, License - CC BY-SA 4.0

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	os = "windows";
#elif __APPLE__
	os = "macos";
#elif __linux__
    os = "linux";
#elif __unix__
    os = "unix";
#elif defined(_POSIX_VERSION)
    os = "posix";
#else
#   error "Unknown compiler"
#endif
	arraylist_append(out, &os);

	return true;
}
