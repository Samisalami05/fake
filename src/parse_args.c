#include "parse_args.h"
#include "arraylist.h"
#include "log.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void parse_args(char **argv, FakeConfig* out) {
	out->filename = "Fakefile";
	out->targets = arraylist_new(sizeof(char*));
	
	for (int i = 1; argv[i] != NULL; i++) {
		char* arg = argv[i];
		if (argv[i][0] != '-') {
			arraylist_append(&out->targets, &arg);
			continue;
		}

		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
			printf("Usage: fake [options] [targets] ...\n");
			goto exit;
		}
		else if (strcmp(argv[i], "--file") == 0) {
			if (!argv[i + 1]) {
				log_error("No file path given to --file flag");
				goto err;
			}
			out->filename = argv[i + 1];
			i++;
		}
		else if (strcmp(argv[i], "--lexer") == 0) {
			out->role = FAKE_ROLE_DEBUG;
			out->debug_flags |= DEBUG_LEXER;
		}
		else if (strcmp(argv[i], "--ast") == 0) {
			out->role = FAKE_ROLE_DEBUG;
			out->debug_flags |= DEBUG_AST;
		}
		else if (strcmp(argv[i], "--blocks") == 0) {
			out->role = FAKE_ROLE_DEBUG;
			out->debug_flags |= DEBUG_BLOCKS;
		}
	}

	if (out->role == FAKE_ROLE_DEBUG) return;

	out->role = out->targets.count == 0 
		? FAKE_ROLE_LIST
		: FAKE_ROLE_RUN;

	return;

exit:
	free(out->targets.items);
	exit(0);

err:
	free(out->targets.items);
	exit(1);
}
