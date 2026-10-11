#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../arraylist.h"
#include "../log.h"
#include "../builtin.h"

int builtin_prompt(arraylist* args, int count, arraylist* out) {
	if (count > 3) {
		log_error("@prompt(): Expected 3 or less argument, got %d", count);
		return BUILTIN_ERROR;
	}

	arraylist msg = args[0];
	foreach (char*, str, msg) {
		printf("%s ", *str);
	}
	printf("[y|yes]:");

	char buf[64];
	fgets(buf, 64, stdin);
	int len = strlen(buf);

	bool yes = (len == 2 && buf[0] == 'y')
		|| strcmp(buf, "yes\n") == 0;

	if ((count >= 2 && yes) || (count == 3 && !yes)) {
		arraylist ret = args[yes ? 1 : 2];
		arraylist_appendn(out, ret.items, ret.count);
	}
	else {
		if (count == 2) return true;
		char* ret = yes ? "true" : "false";
		arraylist_append(out, &ret);
	}

	return BUILTIN_OK;
}
