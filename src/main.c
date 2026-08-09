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
#include "parse_args.h"
#include "parse.h"

bool exec_command(parse_state *state, command *c) {
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

FileView read_file(const char *filename, size_t *out_size) {
	int fd = open(filename, O_RDONLY);
	if (fd == -1) {
		return (FileView){
			.ptr = NULL,
			.size = 0
		};
	}

	struct stat file_stat;
	fstat(fd, &file_stat);

	// +1 => to not need to care about out-of-bounds checks in the lexer
	size_t allocation_size = file_stat.st_size+1; // not the same as file size!
	char *file_str = mmap(NULL, allocation_size, PROT_READ, MAP_PRIVATE, fd, 0);

	if (out_size) {
		*out_size = allocation_size;
	}

	return (FileView){
		.ptr = file_str,
		.size = file_stat.st_size
	};
}

parse_state state_init(FileView file, Tokens tokens) {
	parse_state state = {0};
	state.file_str = file.ptr;
	state.file_size = file.size;
	state.tokens = tokens;
	arraylist_init(&state.labels, sizeof(Label));
	return state;
}

// /path/to/*.c
// "/path/to/*.c"

int main(int argc, char **argv) {
	size_t allocation_size = 0;
	FileView file = read_file("Fakefile", &allocation_size);
	if (file.ptr == NULL) {
		printf("no Fakefile found\n");
		return 1;
	}

	parse_args(argv);

	Lexer lexer = lexer_from_file(file);
	lex(&lexer);
	Tokens tokens = lexer_tokens(&lexer);

	parse_state state = state_init(file, tokens);
	if (!parse_fakefile(&state)) {
		fprintf(stderr, "Failed to parse fakefile\n");
		return 1;
	}

	foreach (Label, node, state.labels) {
		printf("[Node] %s - %ld commands\n", node->name, node->commands.count);
		
		foreach (char*, dep, node->dependencies) {
			printf("\t%s\n", *dep);
		}

		foreach (command, cmd, node->commands) {
			if (!exec_command(&state, cmd)) {
				fprintf(stderr, "\e[1;91m'%s' interrupted\e[0m: command exited with non zero exit code\n", node->name);
				break;
			}
		}
	}

	munmap(file.ptr, allocation_size);
}
