#include "parse_args.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char *flags[] = {
	"--help",
};

// currently, just search for --help
bool parse_args(char **argv) {
	for (int i = 1; argv[i] != NULL; i++) {
		if (0 == strcmp(argv[i], flags[0])) {
			printf("usage: fake (lol)\n");
			return false;
		} else {
			printf("no such flag: %s\n", argv[i]);
			return false;
		}
	}
	return true;
}
