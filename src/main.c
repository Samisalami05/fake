#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdbool.h>

#include "arraylist.h"
#include "fake.h"
#include "file.h"
#include "log.h"
#include "parse_args.h"
#include "parse.h"

bool exec_command(ParseState *state, Command *c) {
	void* end = NULL;
	arraylist_append(&c->args, &end);

	foreach (char*, arg, c->args) {
		if (*arg == NULL) continue;
		printf("%s ", *arg);
	}
	printf("\n");

	pid_t pid = fork();
	if (pid == 0) {
		execvp(*(char**)c->args.items, (char**)c->args.items);
		perror("execvp");
		exit(1);
	} else if (pid > 0) {
		int status = 0;
		wait(&status); // wait for execvp to finish
		if (status != 0) return false;
	}
	return true;
}

ParseState state_init(FileView file, Tokens tokens) {
	ParseState state = {0};
	state.file_ptr = file.ptr;
	state.file_size = file.size;
	state.tokens = tokens;
	arraylist_init(&state.labels, sizeof(Label));
	return state;
}

// /path/to/*.c
// "/path/to/*.c"

int main(int argc, char **argv) {
	FileView file = {0};
	if (!read_file("Fakefile", &file)) {
		log_warning("no 'Fakefile' found");
		return 1;
	}

	parse_args(argv);

	Lexer lexer = lexer_from_file(file);
	lex(&lexer);
	Tokens tokens = lexer_tokens(&lexer);

	ParseState state = state_init(file, tokens);
	if (!parse_fakefile(&state)) {
		log_error("Failed to parse Fakefile\n");
		return 1;
	}

	foreach (Label, node, state.labels) {
		printf("[Node] %s - %ld commands\n", node->name, node->commands.count);
		
		foreach (char*, dep, node->dependencies) {
			printf("\t%s\n", *dep);
		}

		foreach (Command, cmd, node->commands) {
			if (!exec_command(&state, cmd)) {
				fprintf(stderr, "\e[1;91m'%s' interrupted\e[0m: command exited with non zero exit code\n", node->name);
				break;
			}
		}
	}

	close_file(file);
}
