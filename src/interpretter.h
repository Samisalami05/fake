#ifndef INTERPRETTER_H
#define INTERPRETTER_H

#include "arraylist.h"
#include "file.h"
#include "lex.h"
#include "parse.h"

typedef enum {
	BLOCK_LABEL,
	BLOCK_RULE
} BlockType;

typedef struct {
	char* name;
	uint32_t node;
	BlockType type;
	arraylist deps; // of allocated char*
} Block;

typedef struct {
	struct {
		char* key;
		arraylist value;
	}* varmap;
	
	struct {
		char* key;
		uint32_t value;
	}* blockmap;

	arraylist blocks;

	FileView file;
	Lexer* lexer;
	Ast* ast;
} Interpretter;

int find_var(Interpretter* in, char* name);
int find_block(Interpretter* in, char* name);

bool run_prepass(Interpretter* in);
bool execute_block(Interpretter* in, uint32_t id);

#endif
