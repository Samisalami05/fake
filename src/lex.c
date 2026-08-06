#include "arraylist.h"
#include "fake.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static void append_token_from(Lexer* lexer, token_type type, uint32_t index) {
	arraylist_append(&lexer->tokens, &(token){
		.tag = type,
		.index = index,
	});
}

static void append_token(Lexer* lexer, token_type type) {
	append_token_from(lexer, type, lexer->pos);
}


// Takes the current character and makes it into token and increments curr
static void lex_single(Lexer* lexer, token_type type) {
	append_token(lexer, type);
	lexer->pos++;
}

static bool is_at_end(Lexer* lexer) {
	return lexer->pos >= lexer->file_size;
}

static char curr(Lexer* lexer) {
	return lexer->file[lexer->pos];
}

FileView lexer_get_view(Lexer* lexer) {
	return (FileView){
		.ptr = lexer->file,
		.size = lexer->file_size,
	};
}

void lex(Lexer* lexer) {
	arraylist_init(&lexer->tokens, sizeof(token));
	lexer->pos = 0;

	if (!lexer->file || lexer->file_size == 0) return;

	uint8_t identifier_map[256] = {0};
	for (uint32_t i = 0; i < 256; i++) {
		if (i >= 'a' && i <= 'z') identifier_map[i] = 1;
		if (i >= 'A' && i <= 'Z') identifier_map[i] = 1;
		if (i >= '0' && i <= '9') identifier_map[i] = 1;
		if (i == '_') identifier_map[i] = 1;
		if (i == '/') identifier_map[i] = 1;
		if (i == '.') identifier_map[i] = 1;
		if (i == '-') identifier_map[i] = 1;
	}

    while (!is_at_end(lexer)) {
		switch (curr(lexer)) {
			case ':':
				append_token(lexer, TOKEN_COLON);
				break;
			case ',':
				append_token(lexer, TOKEN_COMMA);
				break;
			case '{':
				append_token(lexer, TOKEN_CURLY_L);
				break;
			case '}':
				append_token(lexer, TOKEN_CURLY_R);
				break;
			case '(':
				append_token(lexer, TOKEN_PAREN_L);
				break;
			case ')':
				append_token(lexer, TOKEN_PAREN_R);
				break;
			case ' ':
			case '\t':
			case '\n':
				break;
			// small letters
			case 97:
			case 98:
			case 99:
			case 100:
			case 101:
			case 102:
			case 103:
			case 104:
			case 105:
			case 106:
			case 107:
			case 108:
			case 109:
			case 110:
			case 111:
			case 112:
			case 113:
			case 114:
			case 115:
			case 116:
			case 117:
			case 118:
			case 119:
			case 120:
			case 121:
			case 122:
			// big letters
			case 65:
			case 66:
			case 67:
			case 68:
			case 69:
			case 70:
			case 71:
			case 72:
			case 73:
			case 74:
			case 75:
			case 76:
			case 77:
			case 78:
			case 79:
			case 80:
			case 81:
			case 82:
			case 83:
			case 84:
			case 85:
			case 86:
			case 87:
			case 88:
			case 89:
			case 90:
			case '-':
				{
					uint32_t add_index = lexer->pos;
					for (; !is_at_end(lexer); lexer->pos++) {
						if (0 == identifier_map[(uint8_t)lexer->file[lexer->pos]]) break;
					}
					append_token_from(lexer, TOKEN_IDENTIFIER, add_index);
					continue;
				}
				break;
			case '"':
				{
					uint32_t add_index = lexer->pos;
					lexer->pos++;
					for (; !is_at_end(lexer); lexer->pos++) {
						if (curr(lexer) == '"') {
							lexer->pos++;
							append_token_from(lexer, TOKEN_STRING, add_index);
							continue;
						}
					}
					printErr(lexer_get_view(lexer), lexer->pos, "found no matching '\"'");
					exit(1);
				}
				break;
			// comments!
			case '/':
				// safe, file str always ends with 0!
				if (lexer->file[lexer->pos+1] == '/') {
					lexer->pos += 1;
					for (; lexer->pos < lexer->file_size; lexer->pos++) {
						if (curr(lexer) == '\n') break;
					}
					continue;
				} else {
					goto failure;
				}
failure: default: {
				printErr(lexer_get_view(lexer), lexer->pos, "illegal character");
				exit(1);
			};
		}
		lexer->pos += 1;
	}
	append_token(lexer, TOKEN_EOF);
}

str_ref lexer_token_str(Lexer *lexer, token token) {
	return lexer_token_id_str(lexer, token.index);
}

str_ref lexer_token_id_str(Lexer *lexer, uint32_t token_index) {
	if (token_index >= lexer->tokens.count) {
		fprintf(stderr, "ERROR in 'lex.c': token_index out of bounds\n");
		exit(1);
	}

	uint32_t src = ((token*)lexer->tokens.items)[token_index].index;
	uint32_t dst = ((token*)lexer->tokens.items)[token_index+1].index;

	int i; // new dst
	for (i = src; i < dst; i++) {
		// synchronize with tokenless characters!
		if (lexer->file[i] == ' ') break;
		if (lexer->file[i] == '\t') break;
		if (lexer->file[i] == '\n') break;
	}

	return (str_ref){
		.src = src,
		.len = i-src,
	};
}

uint32_t get_line_start(FileView file, uint32_t index) {
	while (index != 0 && file.ptr[index - 1] != '\n') {
		index--;
	}
	return index;
}

// Includes '\n'
uint32_t get_line_end(FileView file, uint32_t index) {
	while (index < file.size && file.ptr[index] != '\n') {
		index++;
	}
	return index;
}

// Prints char n times
void rep_print(char c, int n) {
	if (n <= 0) return;
	char buf[n];
	memset(buf, c, n);
	printf("%.*s", n, buf);
}

// Todo: fix problem with line start/end when colon is not in second command
// Todo: buffer optimize the prints
// Todo: maybe move to a different file so it can be included in main.c
// TODO: check terminal width and only print the part that fits
void printErr(FileView file, uint32_t index, char* message) {
	uint32_t start = get_line_start(file, index);
	uint32_t end = get_line_end(file, index);
	uint32_t len = end - start;

	fprintf(stderr, "%u:%u: \e[1;91merror:\e[0m %s\n", 0, index - start, message);
	
	uint32_t tab_count = 0;
	for (int i = start; i < end; i++) {
		if (file.ptr[i] == '\t') tab_count++;
	}
	
	// Print line
	printf(" %4d | %.*s\n", 0, len, file.ptr + start);
	
	// Arrow
	printf("      | ");
	rep_print('\t', tab_count);
	rep_print(' ', index - start - tab_count);
	printf("\e[1;91m^\e[0m\n"); // Print red arrow
}
