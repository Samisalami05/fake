#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../arraylist.h"
#include "../log.h"

void render(arraylist options, int pos) {
	for (int i = 0; i < options.count; i++) {
		char* opt = ((char**)options.items)[i];
		if (i == pos) {
			printf("\e[31m%s\e[0m                      \n", opt);
			continue;
		}
		printf("%s\n", opt);
	}
}

void reset_cursor(arraylist options) {
	printf("\r\e[%luA", options.count);
	fflush(stdout);
}

bool builtin_option(arraylist* args, int count, arraylist* out) {
	if (count != 1) {
		log_error("@prompt(): Expected 1 argument, got %d", count);
		return false;
	}

	arraylist options = args[0];
	int pos = 0;

	render(options, pos);
	
	char c;
	while ((c = fgetc(stdin)) != 'q') {
		if (c == 'w') {
			if (pos > 0) pos--;
		}
		else if (c == 's') {
			if (pos < options.count - 1) pos++;
		}

		reset_cursor(options);
		render(options, pos);

		if (c == '\n') printf("\r\e[1A");
	}

	arraylist_append(out, options.items + pos * options.item_size);

	return true;
}
