#ifndef LEXER_H
#define LEXER_H

#include "arraylist.h"
#include "str.h"
#include "file.h"
#include <stddef.h>
#include <stdint.h>

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
} TokenType;

char* token_tag_str(TokenType tag);

typedef struct {
	TokenType tag;
	uint32_t index;
} Token;

typedef struct {
	char* file;
	size_t file_size;

	arraylist tokens;
	size_t pos;
} Lexer;

Lexer lexer_from_file(FileView file);
FileView lexer_get_view(Lexer* lexer);

typedef struct {
	Token* ptr;
	size_t count;
} Tokens;

Tokens lexer_tokens(Lexer* lexer);

void lex(Lexer* lexer);
StrRef lexer_token_str(Lexer* lexer, Token token);
StrRef lexer_token_id_str(Lexer* lexer, uint32_t token_index);

void printErr(FileView file, StrRef ref, char* message);

#endif
