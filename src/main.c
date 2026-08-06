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

str_ref get_token_id_str(parse_state* state, uint32_t token_index) {
	Lexer lexer = {
		.file = state->file_str,
		.file_size = state->file_size,
		.tokens.items = (uint8_t*)state->tokens,
		.tokens.count = state->token_count,
	};
	return lexer_token_id_str(&lexer, token_index);
}

str_ref get_token_str(parse_state* state, token token) {
	return get_token_id_str(state, token.index);
}

token curr_token(parse_state* state) {
	return state->tokens[state->curr];
}

FileView file_view(parse_state* state) {
	return (FileView){
		.ptr = state->file_str,
		.size = state->file_size,
	};
}

void parse_error(parse_state* state, char* fmt, ...) {
	char str[128];

	va_list args;
	va_start(args, fmt);
	vsnprintf(str, 128, fmt, args);
	va_end(args);

	printErr(file_view(state), curr_token(state).index, str);
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
		parse_error(state, "Expected '%s' , got '%s'", expected, got);
		return false;
	}
	return true;
}

bool parse_string(parse_state* state, str_ref* out) {
	if (!expect_token(state, TOKEN_STRING)) return false;

	str_ref ref = get_token_id_str(state, state->curr);
	ref.src += 1;
	ref.len -= 2;
	*out = ref;

	state->curr += 1;
	return true;
}

bool parse_command(parse_state* state, command* cmd) {
	arraylist_init(&cmd->args, sizeof(str_ref));

	int len = 0;
	while (1) {
		str_ref str = {0};
		token_type type = curr_token(state).tag;
		if (type == TOKEN_STRING) {
			if (!parse_string(state, &str)) return false;
		}
		else if (type == TOKEN_IDENTIFIER) {
			str = get_token_id_str(state, state->curr);
			state->curr++;
		}
		else break;
		arraylist_append(&cmd->args, &str);
		len++;
	}

	if (len == 0) {
		parse_error(state, "empty command, command cannot be empty");
		return false;
	}

	return true;
}

bool parse_node(parse_state *state) {
	unlinked_node *node = malloc(sizeof(unlinked_node));
	arraylist_init(&node->commands, sizeof(command*));
	arraylist_init(&node->dependencies, sizeof(str_ref));

	if (!expect_token(state, TOKEN_IDENTIFIER)) return false;
	node->name = get_token_id_str(state, state->curr);

	state->curr += 1;
	if (!expect_token(state, TOKEN_COLON)) return false;
	state->curr += 1;

	// parse deps
	if (!expect_token(state, TOKEN_PAREN_L)) return false;
	state->curr += 1;
	while (1) {
		token_type first_tag = state->tokens[state->curr].tag;
		if (first_tag == TOKEN_PAREN_R) break;

		if (!expect_token(state, TOKEN_IDENTIFIER)) return false;
		str_ref dep = get_token_id_str(state, state->curr);
		state->curr += 1;
		
		arraylist_append(&node->dependencies, &dep);

		if (curr_token(state).tag == TOKEN_PAREN_R) break;
		if (!expect_token(state, TOKEN_COMMA)) return false;
		state->curr += 1;
	}
	if (!expect_token(state, TOKEN_PAREN_R)) return false;
	state->curr += 1;

	// parse body
	if (!expect_token(state, TOKEN_CURLY_L)) return false;
	state->curr += 1;

	// parse commands
	while (1) {
		if (curr_token(state).tag == TOKEN_CURLY_R) break;

		command *c = malloc(sizeof(command));
		if (!parse_command(state, c)) return false;
		arraylist_append(&node->commands, &c);
		
		if (curr_token(state).tag == TOKEN_CURLY_R) break;
		if (!expect_token(state, TOKEN_COMMA)) return false;
		state->curr += 1;
	}
	
	if (!expect_token(state, TOKEN_CURLY_R)) return false;
	state->curr += 1;

	arraylist_append(&state->unlinked_nodes, &node);

	return state->curr;
}

bool parse_fakefile(parse_state *state) {
	while (1) {
		if (curr_token(state).tag == TOKEN_EOF) break;
		if (!parse_node(state)) return false;
	}
	return true;
}

bool exec_command(parse_state *state, command *c) {
	char **argv = malloc(sizeof(char*)*(c->args.count+1));
	argv[c->args.count] = NULL;

	str_ref *refs = (str_ref*)c->args.items;
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

int main(int argc, char **argv) {
	size_t allocation_size = 0;
	FileView file = read_file("Fakefile", &allocation_size);
	if (file.ptr == NULL) {
		printf("no Fakefile found\n");
		return 1;
	}

	parse_args(argv);

	Lexer lexer = {0};
	lexer.file = file.ptr;
	lexer.file_size = file.size;
	lex(&lexer);

	parse_state state = {0};
	state.file_str = file.ptr;
	state.file_size = file.size;
	state.tokens = (token*)lexer.tokens.items;
	state.token_count = lexer.tokens.count;
	arraylist_init(&state.unlinked_nodes, sizeof(unlinked_node*));
	if (!parse_fakefile(&state)) {
		fprintf(stderr, "Failed to parse fakefile\n");
		return 1;
	}

	unlinked_node **nodes = (unlinked_node**)state.unlinked_nodes.items;
	for (uint32_t i = 0; i < state.unlinked_nodes.count; i++) {
		unlinked_node *node = nodes[i];
		command **commands = (command**)node->commands.items;

		printf("[Node] %.*s - %ld commands\n", node->name.len, state.file_str + node->name.src, node->commands.count);
		
		for (uint32_t j = 0; j < node->commands.count; j++) {
			if (!exec_command(&state, commands[j])) {
				fprintf(stderr, "\e[1;91m'%.*s' interrupted\e[0m: command exited with non zero exit code\n", node->name.len, state.file_str + node->name.src);
				break;
			}
		}
	}

	munmap(file.ptr, allocation_size);
}
