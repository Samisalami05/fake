#include "arraylist.h"
#include "fake.h"
#include "file.h"
#include "lex.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static uint8_t identifier_map[256] = {0};

static void append_token_from(Lexer* lexer, TokenType type, uint32_t index) {
	arraylist_append(&lexer->tokens, &(Token){
		.tag = type,
		.index = index,
	});
}

static void append_token(Lexer* lexer, TokenType type) {
	append_token_from(lexer, type, lexer->pos);
}


static bool is_at_end(Lexer* lexer) {
	return lexer->pos >= lexer->file_size;
}

static char curr(Lexer* lexer) {
	return lexer->file[lexer->pos];
}

// Takes the current character and makes it into token and increments curr
static void lex_single(Lexer* lexer, TokenType type) {
	append_token(lexer, type);
	lexer->pos++;
}

static void lex_identifier(Lexer* lexer) {
	uint32_t add_index = lexer->pos;
	for (; !is_at_end(lexer); lexer->pos++) {
		if (0 == identifier_map[(uint8_t)lexer->file[lexer->pos]]) break;
	}
	append_token_from(lexer, TOKEN_IDENTIFIER, add_index);
}

static bool lex_comment(Lexer* lexer) {
	// safe, file str always ends with 0!
	if (lexer->file[lexer->pos+1] == '/') {
		lexer->pos += 1;
		for (; lexer->pos < lexer->file_size; lexer->pos++) {
			if (curr(lexer) == '\n') break;
		}
		return true;
	}
	return false;
}

static void lex_string(Lexer* lexer) {
	uint32_t add_index = lexer->pos;
	lexer->pos++;
	for (; !is_at_end(lexer); lexer->pos++) {
		if (curr(lexer) == '"') {
			lexer->pos++;
			append_token_from(lexer, TOKEN_STRING, add_index);
			return;
		}
	}
	printErr(lexer_get_view(lexer), 
		(StrRef){add_index, lexer->pos - add_index}, 
		"found no matching '\"'"
	);
	exit(1);
}


FileView lexer_get_view(Lexer* lexer) {
	return (FileView){
		.ptr = lexer->file,
		.size = lexer->file_size,
	};
}

void lex(Lexer* lexer) {
	if (!lexer->file || lexer->file_size == 0) return;

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
		char c = curr(lexer);

		if (c == '/' && lex_comment(lexer)) continue;

		if ((c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			 c == '-' || c == '/'  ||
			 c == '.') {
			lex_identifier(lexer);
			continue;
		}

		switch (c) {
			case ':': lex_single(lexer, TOKEN_COLON); break;
			case ',': lex_single(lexer, TOKEN_COMMA); break;
			case '{': lex_single(lexer, TOKEN_CURLY_L); break;
			case '}': lex_single(lexer, TOKEN_CURLY_R); break;
			case '(': lex_single(lexer, TOKEN_PAREN_L); break;
			case ')': lex_single(lexer, TOKEN_PAREN_R); break;

			case '"': lex_string(lexer); break;
			
			// Whitespace
			case ' ':
			case '\t':
			case '\n':
				lexer->pos++;
				break;
		
			default: {
				printErr(lexer_get_view(lexer),
				(StrRef){lexer->pos, 1}, 
				"illegal character");
				exit(1);
			};
		}
	}
	append_token(lexer, TOKEN_EOF);
}

Lexer lexer_from_file(FileView file) {
	Lexer lex = {
		.file = file.ptr,
		.file_size = file.size,
		.pos = 0,
	};
	arraylist_init(&lex.tokens, sizeof(Token));
	return lex;
}

Tokens lexer_tokens(Lexer* lexer) {
	return (Tokens) {
		.ptr = (Token*)lexer->tokens.items,
		.count = lexer->tokens.count,
	};
}

StrRef lexer_token_str(Lexer *lexer, Token token) {
	return lexer_token_id_str(lexer, token.index);
}

StrRef lexer_token_id_str(Lexer *lexer, uint32_t token_index) {
	if (token_index >= lexer->tokens.count) {
		fprintf(stderr, "ERROR in 'lex.c': token_index out of bounds\n");
		exit(1);
	}

	uint32_t src = ((Token*)lexer->tokens.items)[token_index].index;
	uint32_t dst = ((Token*)lexer->tokens.items)[token_index+1].index;

	int i; // new dst
	for (i = src; i < dst; i++) {
		// synchronize with tokenless characters!
		if (lexer->file[i] == ' ') break;
		if (lexer->file[i] == '\t') break;
		if (lexer->file[i] == '\n') break;
	}

	return (StrRef){
		.src = src,
		.len = i-src,
	};
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
void printErr(FileView file, StrRef ref, char* message) {
	uint64_t index = ref.src;

	uint64_t start = file_line_start(file, index);
	uint64_t end = file_line_end(file, index);
	uint64_t len = end - start;

	fprintf(stderr, "%u:%lu: \e[1;91merror:\e[0m %s\n", 0, index - start, message);
	
	uint64_t tab_count = 0;
	for (uint64_t i = start; i < end; i++) {
		if (file.ptr[i] == '\t') tab_count++;
	}
	
	// Print line
	printf(" %4d | %.*s\n", 0, (int)len, file.ptr + start);
	
	// Arrow
	printf("      | ");
	rep_print('\t', tab_count);
	rep_print(' ', index - start - tab_count);
	printf("\e[1;91m^"); // Print red arrow
	rep_print('~', ref.len - 1);
	printf("\e[0m\n");
}
