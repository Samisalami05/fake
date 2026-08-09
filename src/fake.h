#pragma once
#include "arraylist.h"

// TODO: arena allocator!

#include <stdint.h>
// str that references inside the file
typedef struct {
	uint32_t src; // TODO: off
	uint32_t len;
} str_ref;

// TODO: use this
typedef struct {
	char* str;
	size_t capacity;
	size_t count;
} StrBuilder;

void sb_append(StrBuilder* sb, const char* str);

typedef enum {
	TOKEN_IDENTIFIER = 0,
	TOKEN_COLON,
	TOKEN_STRING,
	TOKEN_EOF,
	TOKEN_CURLY_L,
	TOKEN_CURLY_R,
	TOKEN_COMMA,
	TOKEN_PAREN_L,
	TOKEN_PAREN_R,
} token_type;

char* token_tag_str(token_type tag);

typedef struct {
	token_type tag;
	uint32_t index;
} token;

typedef struct {
	char* ptr;
	size_t size;
} FileView;

typedef struct {
	char* file;
	size_t file_size;

	arraylist tokens;
	size_t pos;
} Lexer;

Lexer lexer_from_file(FileView file);
FileView lexer_get_view(Lexer* lexer);

typedef struct {
	token* ptr;
	size_t count;
} Tokens;

Tokens lexer_tokens(Lexer* lexer);

void lex(Lexer* lexer);
str_ref lexer_token_str(Lexer* lexer, token token);
str_ref lexer_token_id_str(Lexer* lexer, uint32_t token_index);

void printErr(FileView file, str_ref ref, char* message);
