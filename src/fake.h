#pragma once
#include "arraylist.h"

// TODO: arena allocator!

#include <stdint.h>
// str that references inside the file
typedef struct {
	uint32_t src; // TODO: off
	uint32_t len;
} str_ref;

typedef struct {
char* str;
size_t allocated;
size_t count;
} StrBuilder;

void sb_append(StrBuilder* sb, const char* str);

typedef struct {
	arraylist args; // str_ref
} command;

typedef struct {
	str_ref name;
	arraylist commands; // of str_ref
	arraylist dependencies; // of str_ref
} unlinked_node;

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

FileView lexer_get_view(Lexer* lexer);

typedef struct  {
	char *file_str;
	uint32_t file_size;

	token *tokens;
	uint32_t token_count;

	arraylist unlinked_nodes;
	uint32_t curr; // Current token
} parse_state;

void lex(Lexer* lexer);
str_ref lexer_token_str(Lexer* lexer, token token);
str_ref lexer_token_id_str(Lexer* lexer, uint32_t token_index);

void printErr(FileView file, uint32_t curr, char* message);
