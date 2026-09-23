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
#include "log.h"
#include "parse_args.h"
#include "parse.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

static struct { char* key; arraylist value; }* varmap = NULL;
static struct { char* key; uint32_t value; }* blockmap = NULL;

static arraylist blocks;

// Returns -1 if not found
int find_var(char* name) {
	return stbds_shgeti(varmap, name);
}

Block get_block(uint32_t id) {
	return ((Block*)blocks.items)[id];
}

int skip_cmd(Ast* ast, int node) {
	int pos = node + 1;
	for (int i = 0; i < ast->data[node].child_count; i++) {
		AstNode child = ast->data[pos];
		switch (child.type) {
			case AST_NODE_STRING:
			case AST_NODE_VAR_REF:
			case AST_NODE_IDENTIFIER:
			case AST_NODE_AUTOVAR: {
				pos++;
				break;
			}
			case AST_NODE_BUILTIN: {
				pos++;
				for (int i = 0; i < child.child_count; i++) {
					pos = skip_cmd(ast, pos);
					if (pos == -1) return -1;
				}
				break;
			}
			default:;
		}
	}
	return pos;
}

int process_cmd(Ast* ast, int node, FileView file, arraylist* out, char* target) {
	int pos = node + 1;
	for (int i = 0; i < ast->data[node].child_count; i++) {
		AstNode child = ast->data[pos];
		switch (child.type) {
			case AST_NODE_STRING:
			case AST_NODE_IDENTIFIER: {
				char* str = file_str_ref(file, child.ref);
				arraylist_append(out, &str);
				pos++;
				break;
			}
			case AST_NODE_BUILTIN: {
				char* name = file_str_ref(file, child.ref);
				pos++;
				arraylist args[child.child_count];
				for (int i = 0; i < child.child_count; i++) {
					args[i] = arraylist_new(sizeof(char*));
					pos = process_cmd(ast, pos, file, args + i, target);
					if (pos == -1) return -1;
				}

				if (!exec_builtin(name, args, child.child_count, out)) {
					log_error("Could not execute builtin @%s(): Make sure it exists", name);
					return -1;
				}
				break;
			}
			case AST_NODE_VAR_REF: {
				char* name = file_str_ref(file, child.ref);
				int var = find_var(name);
				if (var == -1) {
					log_error("Variable '%s' does not exist", name);
					return -1;
				}

				arraylist values = varmap[var].value;
				foreach (char*, val, values) {
					arraylist_append(out, val);
				}
				pos++;
				break;
			}
			case AST_NODE_AUTOVAR: {
				if (!target) {
					log_error("Autovars can only exist in blocks");
					return -1;
				}
				char* name = file_str_ref(file, child.ref);
				if (strcmp(name, "names") == 0) {
					arraylist_append(out, &target);
				}
				else if (strcmp(name, "deps") == 0) {
					int id = stbds_shgeti(blockmap, target);
					if (id == -1) {
						log_error("Unreachable");
						return -1;
					}

					Block block = get_block(id);
					for (int i = 0; i < block.deps.count; i++) {
						arraylist_append(out, ((char**)block.deps.items) + i);
					}
				}
				else {
					log_error("Autovar '%s' does not exist", name);
					return -1;
				}
				pos++;
				break;
			}
			default:;
		}
	}
	return pos;
}

bool process_var(Ast* ast, int node, FileView file) {
	if (ast->data[node].child_count == 0) return true;
	if (ast->data[node].child_count > 1) {
		log_error("Variable contains more than one child node");
		return false;
	}

	char* name = file_str_ref(file, ast->data[node].ref);
	arraylist values = arraylist_new(sizeof(char*));
	if (process_cmd(ast, node + 1, file, &values, NULL) == -1) {
		log_error("Failed to process variable '%s'", name);
		return false;
	}

	stbds_shput(varmap, name, values);

	return true;
}

bool add_block(char* name, Block block) {
	if (stbds_shgeti(blockmap, name) != -1) {
		log_error("Target '%s' already exists", name);
		return false;
	}
	uint32_t id = blocks.count;
	stbds_shput(blockmap, name, id);
	arraylist_append(&blocks, &block);
	return true;
}

bool process_block(Ast* ast, int node, FileView file) {
	if (ast->data[node].child_count == 0) {
		log_error("Failed to process block: No children found in node");
		return false;
	}

	if (ast->data[node + 1].type != AST_NODE_NAMES) {
		log_error("Failed to process block: No names node found");
		return false;
	}

	arraylist names = arraylist_new(sizeof(char*));
	int next = process_cmd(ast, node + 1, file, &names, NULL);
	if (!next) return false;


	arraylist deps = arraylist_new(sizeof(char*));
	if (ast->data[next].type == AST_NODE_DEPS) {
		if (!process_cmd(ast, next, file, &deps, NULL)) return false;
	}

	BlockType type;
	switch (ast->data[node].type) {
		case AST_NODE_LABEL: type = BLOCK_LABEL; break;
		case AST_NODE_RULE: type = BLOCK_RULE; break;
		case AST_NODE_MULTI: type = BLOCK_RULE; break;
		default: return false;
	}
	
	if (ast->data[node].type != AST_NODE_MULTI) {
		for (int i = 0; i < names.count; i++) {
			char* name = ((char**)names.items)[i];

			Block block = {0};
			block.type = type;
			block.deps = deps;
			block.node = node;

			if (!add_block(name, block)) return false;
		}
	}
	else {
		if (names.count != deps.count) {
			// TODO: this might not be the best implementation
			log_error("Invalid multi block: Targets and deps count have to match");
			return false;
		}

		for (int i = 0; i < names.count; i++) {
			char* name = ((char**)names.items)[i];
			
			Block block = {0};
			block.type = type;
			block.deps = arraylist_new(sizeof(char*));
			arraylist_append(&block.deps, ((char**)deps.items) + i);
			block.node = node;

			if (!add_block(name, block)) return false;

		}
	}

	return true;
}

bool prepass(Ast* ast, FileView file) {
	blocks = arraylist_new(sizeof(Block));

	for (int i = 0; i < ast->count; i++) {
		AstNode node = ast->data[i];
		switch (node.type) {
			case AST_NODE_VAR_DECL: {
				if (!process_var(ast, i, file)) return false;
				break;
			}
			case AST_NODE_LABEL:
			case AST_NODE_RULE: 
			case AST_NODE_MULTI: {
				if (!process_block(ast, i, file)) return false;
				break;
			}
			default:;
		}
	}

	for (int i = 0; i < stbds_shlen(varmap); i++) {
		printf("%s = ", varmap[i].key);
		foreach (char*, val, varmap[i].value) {
			printf("%s ", *val);
		}
		printf("\n");
	}

	for (int i = 0; i < stbds_shlen(blockmap); i++) {
		printf("%s: ", blockmap[i].key);
		uint32_t id = blockmap[i].value;
		Block block = ((Block*)blocks.items)[id];
		foreach (char*, dep, block.deps) {
			printf("%s ", *dep);
		}
		printf("(%d)", block.node);
		printf("\n");
	}
	return true;
}

bool execute_block(Ast* ast, char* name, FileView file) {
	uint32_t id = stbds_shgeti(blockmap, name);
	if (id == -1) {
		log_error("Failed to execute block: '%s' does not exist", name);
		return false;
	}

	Block block = ((Block*)blocks.items)[id];
	AstNode node = ast->data[block.node];
	int pos = block.node + 1;
	for (int i = 0; i < node.child_count; i++) {
		AstNodeType type = ast->data[pos].type;
		switch (type) {
			case AST_NODE_NAMES:
			case AST_NODE_DEPS:
				pos = skip_cmd(ast, pos);
				if (pos == -1) {
					log_error("Failed to execute block");
					return false;
				}
				break;
			case AST_NODE_CMD: {
				arraylist arglist = arraylist_new(sizeof(char*));
				pos = process_cmd(ast, pos, file, &arglist, name);
				if (pos == -1) {
					log_error("Failed to execute block");
					return false;
				}
				void* null = NULL;
				arraylist_append(&arglist, &null);

				char** args = (char**)arglist.items;

				pid_t pid = fork();
				if (pid == 0) {
					execvp(*args, args);
					log_perror("Failed to execute program %s", *args);
					exit(1);
				} else if (pid > 0) {
					int status = 0;
					waitpid(pid, &status, 0); // wait for execvp to finish
					if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
						log_error("Child process exited with status %d", WEXITSTATUS(status));
						return false;
					}
				}
				break;
			}
			default:
				log_error("Failed to execute block: Invalid node type '%s'", ast_type_cstr(type));
		}
	}
	return true;
}

int main(int argc, char **argv) {
	parse_args(argv);

	FileView file = {0};
	if (!read_file("Fakefile", &file)) {
		log_warning("no 'Fakefile' found");
		return 1;
	}

	Lexer lexer = lexer_from_file(file);
	lex(&lexer);
	Tokens tokens = lexer_tokens(&lexer);

	Ast ast;
	if (!parse_fakefile(file, tokens, &ast)) {
		log_error("Failed to parse Fakefile");
		return 1;
	}

	if (!prepass(&ast, file)) {
		log_error("Prepass failed");
		return 1;
	}

	if (blocks.count > 0) {
		char* first = blockmap[0].key;
		execute_block(&ast, first, file);
	}
	close_file(file);
}
