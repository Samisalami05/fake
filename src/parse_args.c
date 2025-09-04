#include "parse_args.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char *flags[] = {
	"--help",
};

// currently, just search for --help
void parse_args(char **argv) {
	for (int i = 1; argv[i] != NULL; i++) {
		if (0 == strncmp(argv[i], flags[0], strlen(flags[0]))) {
			printf("usage: fake (lol)\n");
			exit(0);
		} else {
			printf("no such flag: %s\n", argv[i]);
			exit(1);
		}
	}
}
