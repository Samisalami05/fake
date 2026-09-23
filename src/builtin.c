#include "builtin.h"
#include "arraylist.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>

static bool find(arraylist* args, int count, arraylist* out) {
	/*
	if (params.count == 0) return false;

	char* dir_name = ((char**)params.items)[0];
	char* ext = ((char**)params.items)[1];
	int ext_len = strlen(ext);

	DIR* dir = opendir(dir_name);
	
	struct dirent* entry;
	while ((entry = readdir(dir))) {
		int len = strlen(entry->d_name);
		int i = ext_len;
		while (entry->d_name[len - i] != '.') {
			if (i >= ext_len) {
				continue;
			}
			//if (entry->d_name[i] !=)
		}
	}

	closedir(dir); */
	return true;
}

static bool echo(arraylist* args, int count, arraylist* out) {
	for (int i = 0; i < count; i++) {
		foreach (char*, param, args[i]) {
			printf("%s ", *param);
			//arraylist_append(out, param);
		}
	}
	printf("\n");
	return true;
}

bool exec_builtin(const char* name, arraylist* args, int count, arraylist* out) {
	if (strcmp(name, "find") == 0) return find(args, count, out);
	else if (strcmp(name, "echo") == 0) return echo(args, count, out);
	return false;
}
