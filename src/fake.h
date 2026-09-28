#ifndef FAKE_H
#define FAKE_H

#include "file.h"
#include "interpretter.h"
#include "lex.h"
#include "parse_args.h"

// TODO: arena allocator!

#include <stdint.h>

typedef struct {
	FakeConfig conf;

	FileView file;
	Lexer lexer;
	Ast ast;
	Interpretter in;
} Fakefile;

bool fake_open(FakeConfig conf, Fakefile* out);
bool fake_exec(Fakefile* ff);
void fake_close(Fakefile* ff);

#endif
