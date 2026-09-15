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

bool exec_command(Command *c) {
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
		log_perror("Failed to execute program %s", ((char**)c->args.items)[0]);
		exit(1);
	} else if (pid > 0) {
		int status = 0;
		wait(&status); // wait for execvp to finish
		if (status != 0) return false;
	}
	return true;
}

uint32_t sum_cmd(Ast* ast, uint32_t node, arraylist* out) {
	
}

uint32_t exec_cmd(Ast* ast, uint32_t node) {
	AstNode n = ast->data[node];
	for (int i = 0; i < n.child_count; i++) {
		AstNode child = ast->data[node + i];
		
	}
}

// /path/to/*.c
// "/path/to/*.c"

int main(int argc, char **argv) {
	parse_args(argv);

	FileView file = {0};
	if (!read_file("Fakefile", &file)) {
		log_warning("no 'Fakefile' found");
		return 1;
	}

	Lexer lexer = lexer_from_file(file);
	lex(&lexer);
	Tokens tokens = lexer_tokens(&lexer);

	Ast ast;
	if (!parse_fakefile(file, tokens, &ast)) {
		log_error("Failed to parse Fakefile\n");
		return 1;
	}

	

	close_file(file);
}
