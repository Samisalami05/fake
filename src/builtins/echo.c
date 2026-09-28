#include <stdbool.h>
#include <stdio.h>
#include "../arraylist.h"

// TODO: fix printing from var decl even when no target is ran
bool builtin_echo(arraylist* args, int count, arraylist* out) {
	for (int i = 0; i < count; i++) {
		foreach (char*, param, args[i]) {
			printf("%s ", *param);
		}
	}
	printf("\n");
	return true;
}
