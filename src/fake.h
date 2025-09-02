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
	arraylist args; // str_ref
} command;

typedef struct {
	str_ref name;
	arraylist commands; // of str_ref
	arraylist dependencies; // of str_ref
} unlinked_node;

typedef enum {
	token_identifier = 0,
	token_colon = 1,
	token_string = 2,
	token_eof = 3,
	token_curly_l = 4,
	token_curly_r = 5,
	token_comma = 7,
	token_paren_l,
	token_paren_r,
} token_type;

typedef struct {
	token_type tag;
	uint32_t index;
} token;

typedef struct  {
	char *file_str;
	uint32_t file_size;

	token *tokens;
	uint32_t token_count;
	uint32_t token_allocated;

	arraylist unlinked_nodes;
} parse_state;

void lex(parse_state *state);
str_ref get_token_str(parse_state *state, uint32_t token_index);
