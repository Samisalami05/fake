#include "fake.h"
#include "interpretter.h"
#include "lex.h"
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

	if (conf.role == FAKE_ROLE_DEBUG && conf.debug_flags & DEBUG_LEXER)
		lexer_debug(&out->lexer);

	// Parsing
	Tokens tokens = lexer_tokens(&out->lexer);
	if (!parse_fakefile(out->file, tokens, &out->ast)) {
		log_error("Failed to parse Fakefile");
		return false;
	}

	if (conf.role == FAKE_ROLE_DEBUG && conf.debug_flags & DEBUG_AST)
		ast_debug(&out->ast, out->file);

	// Interpretting
	out->in.file = out->file;
	out->in.lexer = &out->lexer;
	out->in.ast = &out->ast;

	if (!run_prepass(&out->in)) {
		log_error("Prepass failed");
		return false;
	}

	if (conf.role == FAKE_ROLE_DEBUG && conf.debug_flags & DEBUG_BLOCKS)
		in_debug(&out->in);

	return true;
}

static bool debug_print(Fakefile* ff) {
	uint64_t flags = ff->conf.debug_flags;

	if (flags & DEBUG_LEXER) lexer_debug(&ff->lexer);
	if (flags & DEBUG_AST) ast_debug(&ff->ast, ff->file);
	if (flags & DEBUG_BLOCKS) in_debug(&ff->in);
	return true;
}

static bool list_blocks(Fakefile* ff) {
	printf("\e[38;5;243mAvailable labels:\e[0m\n");
	foreach (Block, block, ff->in.blocks) {
		if (block->type != BLOCK_LABEL) continue;
		printf("    %s\n", block->name);
	}
	return true;
}

static bool exec_targets(Fakefile* ff) {
	foreach (char*, target, ff->conf.targets) {
		uint32_t id = find_block(&ff->in, *target);
		if (id == -1) {
			log_error("Failed to execute block: '%s' does not exist", *target);
			return false;
		}
		if (!execute_block(&ff->in, id))
			return false;
	}
	return true;
}

bool fake_exec(Fakefile* ff) {
	switch (ff->conf.role) {
		case FAKE_ROLE_DEBUG: return true; //debug_print(ff);
		case FAKE_ROLE_LIST:  return list_blocks(ff);
		case FAKE_ROLE_RUN:   return exec_targets(ff);
	}
	log_error("Unreachable");
	return false;
}

void fake_close(Fakefile* ff) {
	close_file(ff->file);
}
