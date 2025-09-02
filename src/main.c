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

#include "fake.h"

#define NOT_FOUND 0xFFFFFFFF

void parse_error() {
	printf("bro this is not a fakefile\n");
	exit(1);
}

uint32_t find_next_char(char *str, uint32_t str_size, uint32_t from, char c) {
	for (uint32_t i = from; i < str_size; i++) {
		if (str[i] == c) return i;
	}
	return NOT_FOUND;
}

uint32_t find_next_char_nonl(char *str, uint32_t str_size, uint32_t from, char c) {
	for (uint32_t i = from; i < str_size; i++) {
		if (str[i] == '\n') return NOT_FOUND;
		if (str[i] == '\0') return NOT_FOUND;
		if (str[i] == c) return i;
	}
	return NOT_FOUND;
}

uint32_t parse_node(parse_state *state, uint32_t curr) {
	unlinked_node *node = malloc(sizeof(unlinked_node));
	arraylist_init(&node->commands, sizeof(command*));
	arraylist_init(&node->dependencies, sizeof(str_ref));

	if (state->tokens[curr].tag != token_identifier) parse_error();
	node->name = get_token_str(state, curr);

	curr += 1;
	if (state->tokens[curr].tag != token_colon) parse_error();
	curr += 1;

	// parse deps
	if (state->tokens[curr].tag != token_paren_l) parse_error();
	curr += 1;
	while (1) {
		token_type first_tag = state->tokens[curr].tag;
		if (first_tag == token_paren_r) break;

		if (state->tokens[curr].tag != token_identifier) parse_error();
		str_ref dep = get_token_str(state, curr);
		curr += 1;
		if (state->tokens[curr].tag != token_comma) parse_error();
		curr += 1;

		arraylist_append(&node->dependencies, &dep);
	}
	if (state->tokens[curr].tag != token_paren_r) parse_error();
	curr += 1;

	// parse body

	if (state->tokens[curr].tag != token_curly_l) parse_error();
	curr += 1;

	// parse commands
	while (1) {
		token_type first_tag = state->tokens[curr].tag;
		if (first_tag == token_curly_r) break;
		
		command *c = malloc(sizeof(command));
		arraylist_init(&c->args, sizeof(str_ref));

		while (1) {
			if (state->tokens[curr].tag != token_string) break;
			str_ref ref = get_token_str(state, curr);
			ref.src += 1;
			ref.len -= 2;
			
			curr += 1;
			
			arraylist_append(&c->args, &ref);
		}
		if (state->tokens[curr].tag != token_comma) parse_error();
		curr += 1;

		arraylist_append(&node->commands, &c);
	}
	
	if (state->tokens[curr].tag != token_curly_r) parse_error();
	curr += 1;

	arraylist_append(&state->unlinked_nodes, &node);

	return curr;
}

void parse_fakefile(parse_state *state) {
	uint32_t curr = 0;
	while (1) {
		if (state->tokens[curr].tag == token_eof) break;
		curr = parse_node(state, curr);
	}
}

void exec_command(parse_state *state, command *c) {
	char **argv = malloc(sizeof(char*)*(c->args.count+1));
	argv[c->args.count] = NULL;

	str_ref *refs = c->args.ptr;
	for (uint32_t i = 0; i < c->args.count; i++) {
		char *arg = malloc(refs[i].len+1);
		memcpy(arg, &state->file_str[refs[i].src], refs[i].len);
		arg[refs[i].len] = 0;

		argv[i] = arg;
		//printf("arg: %s\n", argv[i]);
	}

	uint32_t pid = fork();
	if (pid == 0) {
		if (execvp(argv[0], argv) == -1) {
			perror("execvp");
			exit(1);
		}
	} else if (pid > 0) {
		wait(NULL); // wait for execvp to finish
	} else {
		printf("goofy moment\n");
		exit(1);
	}
}

int main(int argc, char **argv) {
	int fd = open("fakefile", 0);
	if (fd == -1) {
		printf("no fakefile found\n");
		exit(1);
	}
	struct stat file_stat;
	fstat(fd, &file_stat);
	char *file_str = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	parse_state state = {
		.file_str = file_str,
		.file_size = file_stat.st_size,
	};
	arraylist_init(&state.unlinked_nodes, sizeof(unlinked_node*));
	lex(&state);
	parse_fakefile(&state);

	unlinked_node **nodes = state.unlinked_nodes.ptr;
	for (uint32_t i = 0; i < state.unlinked_nodes.count; i++) {
		unlinked_node *node = nodes[i];
		command **commands = node->commands.ptr;
		
		str_ref name_ref = nodes[i]->name;
		for (uint32_t j = 0; j < node->commands.count; j++) {
			exec_command(&state, commands[j]);
		}
	}

	munmap(file_str, file_stat.st_size);
}
