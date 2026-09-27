#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdbool.h>

#include "arraylist.h"
#include "builtin.h"
#include "fake.h"
#include "file.h"
#include "interpretter.h"
#include "log.h"
#include "parse_args.h"
#include "parse.h"

int main(int argc, char **argv) {
	if (!parse_args(argv)) return 1;

	FileView file = {0};
	if (!read_file("Fakefile", &file)) {
		log_warning("no 'Fakefile' found");
		return 1;
	}

	Lexer lexer = lexer_from_file(file);
	lex(&lexer);
	Tokens tokens = lexer_tokens(&lexer);

	Ast ast = {0};
	if (!parse_fakefile(file, tokens, &ast)) {
		log_error("Failed to parse Fakefile");
		return 1;
	}

	Interpretter in = {0};
	in.file = file;
	in.lexer = &lexer;
	in.ast = &ast;

	if (!run_prepass(&in)) {
		log_error("Prepass failed");
		return 1;
	}
	
	if (argc == 1 && in.blocks.count == 0) goto exit;

	char* target;
	if (argc == 2) target = argv[1];
	else target = in.blockmap[0].key;

	uint32_t id = find_block(&in, target);
	if (id == -1) {
		log_error("Failed to execute block: '%s' does not exist", argv[1]);
		goto exit;
	}
	execute_block(&in, id);

exit:
	close_file(file);
}
