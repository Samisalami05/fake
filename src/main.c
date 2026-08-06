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

#include "fake.h"
#include "parse_args.h"

#define NOT_FOUND 0xFFFFFFFF

token curr_token(parse_state* state) {
	return state->tokens[state->curr];
}

void parse_error(parse_state* state, char* fmt, ...) {
	char str[128];

	va_list args;
	va_start(args, fmt);
	vsnprintf(str, 128, fmt, args);
	va_end(args);

	printErr(state, curr_token(state).index, str);
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

bool expect_token(parse_state* state, token_type token) {
	if (curr_token(state).tag != token) { 
		char* expected = token_tag_str(token);
		char* got = token_tag_str(curr_token(state).tag);
		parse_error(state, "Expected '%s', got '%s'", expected, got);
		return false;
	}
	return true;
}

bool parse_node(parse_state *state) {
	unlinked_node *node = malloc(sizeof(unlinked_node));
	arraylist_init(&node->commands, sizeof(command*));
	arraylist_init(&node->dependencies, sizeof(str_ref));

	if (!expect_token(state, token_identifier)) return false;
	node->name = get_token_id_str(state, state->curr);

	state->curr += 1;
	if (!expect_token(state, token_colon)) return false;
	state->curr += 1;

	// parse deps
	if (!expect_token(state, token_paren_l)) return false;
	state->curr += 1;
	while (1) {
		token_type first_tag = state->tokens[state->curr].tag;
		if (first_tag == token_paren_r) break;

		if (!expect_token(state, token_identifier)) return false;
		str_ref dep = get_token_id_str(state, state->curr);
		state->curr += 1;
		
		arraylist_append(&node->dependencies, &dep);

		if (curr_token(state).tag == token_paren_r) break;
		if (!expect_token(state, token_comma)) return false;
		state->curr += 1;
	}
	if (!expect_token(state, token_paren_r)) return false;
	state->curr += 1;

	// parse body

	if (!expect_token(state, token_curly_l)) return false;
	state->curr += 1;

	// parse commands
	while (1) {
		token_type first_tag = state->tokens[state->curr].tag;
		if (first_tag == token_curly_r) break;
		
		command *c = malloc(sizeof(command));
		arraylist_init(&c->args, sizeof(str_ref));

		while (1) {
			if (curr_token(state).tag != token_string) break;
			str_ref ref = get_token_id_str(state, state->curr);
			ref.src += 1;
			ref.len -= 2;
			
			state->curr += 1;
			
			arraylist_append(&c->args, &ref);
		}
		if (!expect_token(state, token_comma)) return false;
		state->curr += 1;

		arraylist_append(&node->commands, &c);
	}
	
	if (!expect_token(state, token_curly_r)) return false;
	state->curr += 1;

	arraylist_append(&state->unlinked_nodes, &node);

	return state->curr;
}

bool parse_fakefile(parse_state *state) {
	while (1) {
		if (curr_token(state).tag == token_eof) break;
		if (!parse_node(state)) return false;
	}
	return true;
}

bool exec_command(parse_state *state, command *c) {
	char **argv = malloc(sizeof(char*)*(c->args.count+1));
	argv[c->args.count] = NULL;

	str_ref *refs = c->args.ptr;
	for (uint32_t i = 0; i < c->args.count; i++) {
		char *arg = malloc(refs[i].len+1);
		memcpy(arg, &state->file_str[refs[i].src], refs[i].len);
		arg[refs[i].len] = 0;

		argv[i] = arg;
		printf("%s ", argv[i]);
	}
	printf("\n");

	uint32_t pid = fork();
	if (pid == 0) {
		execvp(argv[0], argv);
		perror("execvp");
		exit(1);
	} else if (pid > 0) {
		int status = 0;
		wait(&status); // wait for execvp to finish
		if (status != 0) return false;
	}
	return true;
}

int main(int argc, char **argv) {
	int fd = open("Fakefile", 0);
	if (fd == -1) {
		printf("no Fakefile found\n");
		exit(1);
	}
	struct stat file_stat;
	fstat(fd, &file_stat);

	// +1 => to not need to care about out-of-bounds checks in the lexer
	size_t allocation_size = file_stat.st_size+1; // not the same as file size!
	char *file_str = mmap(NULL, allocation_size, PROT_READ, MAP_PRIVATE, fd, 0);

	parse_state state = {0};
	state.file_str = file_str;
	state.file_size = file_stat.st_size;

	arraylist_init(&state.unlinked_nodes, sizeof(unlinked_node*));

	parse_args(argv);
	lex(&state);
	if (!parse_fakefile(&state)) {
		fprintf(stderr, "Failed to parse fakefile\n");
		return 1;
	}

	unlinked_node **nodes = state.unlinked_nodes.ptr;
	for (uint32_t i = 0; i < state.unlinked_nodes.count; i++) {
		unlinked_node *node = nodes[i];
		command **commands = node->commands.ptr;

		printf("[Node] %.*s - %d commands\n", node->name.len, state.file_str + node->name.src, node->commands.count);
		
		for (uint32_t j = 0; j < node->commands.count; j++) {
			if (!exec_command(&state, commands[j])) {
				fprintf(stderr, "\e[1;91m'%.*s' interrupted\e[0m: command exited with non zero exit code\n", node->name.len, state.file_str + node->name.src);
				break;
			}
		}
	}

	munmap(file_str, allocation_size);
}
