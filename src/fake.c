#include "fake.h"
#include "interpretter.h"
#include "log.h"
#include "parse_args.h"
#include <stdio.h>

bool fake_open(FakeConfig conf, Fakefile* out) {
	out->conf = conf;

	// Reading
	if (!read_file(conf.filename, &out->file)) {
		log_error("'%s' not found", conf.filename);
		return false;
	}

	// Lexing
	out->lexer = lexer_from_file(out->file);
	lex(&out->lexer);

	// Parsing
	Tokens tokens = lexer_tokens(&out->lexer);
	if (!parse_fakefile(out->file, tokens, &out->ast)) {
		log_error("Failed to parse Fakefile");
		return false;
	}

	// Interpretting
	out->in.file = out->file;
	out->in.lexer = &out->lexer;
	out->in.ast = &out->ast;

	if (!run_prepass(&out->in)) {
		log_error("Prepass failed");
		return false;
	}
	return true;
}

bool fake_exec(Fakefile* ff) {
	if (ff->conf.role == FAKE_ROLE_LIST) {
		printf("\e[38;5;243mAvailable labels:\e[0m\n");
		foreach (Block, block, ff->in.blocks) {
			if (block->type != BLOCK_LABEL) continue;
			printf("    %s\n", block->name);
		}
		return true;
	}

	foreach (char*, target, ff->conf.targets) {
		uint32_t id = find_block(&ff->in, *target);
		if (id == -1) {
			log_error("Failed to execute block: '%s' does not exist", *target);
			return false;
		}
		execute_block(&ff->in, id);
	}
	return true;
}

void fake_close(Fakefile* ff) {
	close_file(ff->file);
}
