#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../arraylist.h"
#include "../log.h"

bool rmdir_rec(const char* path) {
	Dir dir;
	if (!open_dir(path, &dir)) return false;

	DirEntry entry;
	while (read_dir(&dir, &entry)) {
		char* name = entry_name(&entry);
		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
			continue;

		char entry_path[256];
		snprintf(entry_path, 256, "%s/%s", path, name);

		if (entry_is_dir(&entry)) {
			rmdir_rec(entry_path);
			continue;
		}
		remove_file(entry_path);
	}

	close_dir(&dir);

	remove_dir(path);
	return true;
}

bool builtin_rmrf(arraylist* args, int count, arraylist* out) {
	if (count != 1) {
		log_error("@rmrf(): Expects one argument");
		return false;
	}

    foreach (char*, path, *args) {
		if (file_is_dir(*path)) {
			rmdir_rec(*path);
			continue;
        }
		remove_file(*path);
    }


    return true;
}
