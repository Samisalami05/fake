#include "interpretter.h"
#include "arraylist.h"
#include "builtin.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

// Returns -1 if not found
int find_var(Interpretter* in, char* name) {
	return stbds_shgeti(in->varmap, name);
}

// Returns -1 if not found
int find_block(Interpretter* in, char* name) {
	return stbds_shgeti(in->blockmap, name);
}

Block get_block(Interpretter* in, uint32_t id) {
	return ((Block*)in->blocks.items)[id];
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

int process_cmd(Interpretter* in, int node, arraylist* out, char* target) {
	int pos = node + 1;
	for (int i = 0; i < in->ast->data[node].child_count; i++) {
		AstNode child = in->ast->data[pos];
		switch (child.type) {
			case AST_NODE_STRING:
			case AST_NODE_IDENTIFIER: {
				char* str = file_str_ref(in->file, child.ref);
				arraylist_append(out, &str);
				pos++;
				break;
			}
			case AST_NODE_BUILTIN: {
				char* name = file_str_ref(in->file, child.ref);
				pos++;

				// Lazy evaluation for @if() builtin
				if (strcmp(name, "if") == 0) {
					if (child.child_count < 2) {
						log_error("@if(): Expects atleast two arguments, got %d", child.child_count);
						return -1;
					}

					int expr_count = child.child_count == 2 
						? child.child_count - 1
						: child.child_count - 2;


					bool equals = true;

					arraylist values = arraylist_new(sizeof(char*));
					for (int i = 0; i < expr_count; i++) {
						arraylist_clear(&values);
						pos = process_cmd(in, pos, &values, target);
						if (pos == -1) return -1;


						char* first = ((char**)values.items)[0];
						if (values.count == 1) {
							if (strcmp(first, "true") != 0)
								equals = false;
							continue;
						}

						for (int j = 1; j < values.count; j++) {
							char* other = ((char**)values.items)[j];
							if (strcmp(first, other) != 0) {
								equals = false;
								break;
							}
						}
					}

					if (equals) {
						pos = process_cmd(in, pos, out, target);
						if (pos == -1) return -1;
						pos = skip_cmd(in->ast, pos);
						if (pos == -1) return -1;
					}
					else if (child.child_count > 2) {
						pos = skip_cmd(in->ast, pos);
						if (pos == -1) return -1;
						pos = process_cmd(in, pos, out, target);
						if (pos == -1) return -1;
					}

					break;
				}

				arraylist args[child.child_count];
				for (int i = 0; i < child.child_count; i++) {
					args[i] = arraylist_new(sizeof(char*));
					pos = process_cmd(in, pos, args + i, target);
					if (pos == -1) return -1;
				}

				if (!exec_builtin(name, args, child.child_count, out)) {
					log_error("Could not execute builtin @%s(): Make sure it exists", name);
					return -1;
				}
				break;
			}
			case AST_NODE_VAR_REF: {
				char* name = file_str_ref(in->file, child.ref);
				if (target && strcmp(name, "name") == 0) {
					arraylist_append(out, &target);
				}
				else if (target && strcmp(name, "deps") == 0) {
					int id = find_block(in, target);
					if (id == -1) {
						log_error("Unreachable");
						return -1;
					}

					Block block = get_block(in, id);
					for (int i = 0; i < block.deps.count; i++) {
						arraylist_append(out, ((char**)block.deps.items) + i);
					}
				}
				else {
					int var = find_var(in, name);
					if (var == -1) {
						log_error("Variable '%s' does not exist", name);
						return -1;
					}
					
					arraylist values = in->varmap[var].value;
					foreach (char*, val, values) {
						arraylist_append(out, val);
					}
				}

				pos++;
				break;
			}
			case AST_NODE_AUTOVAR: {
				log_error("Autovars not implemented");
				return -1;

				if (!target) {
					log_error("Autovars can only exist in blocks");
					return -1;
				}
				char* name = file_str_ref(in->file, child.ref);
				
				pos++;
				break;
			}
			default:;
		}
	}
	return pos;
}

bool process_var(Interpretter* in, int node) {
	if (in->ast->data[node].child_count == 0) return true;
	if (in->ast->data[node].child_count > 1) {
		log_error("Variable contains more than one child node");
		return false;
	}

	char* name = file_str_ref(in->file, in->ast->data[node].ref);
	arraylist values = arraylist_new(sizeof(char*));
	if (process_cmd(in, node + 1, &values, NULL) == -1) {
		log_error("Failed to process variable '%s'", name);
		return false;
	}

	stbds_shput(in->varmap, name, values);

	return true;
}

bool add_block(Interpretter* in, char* name, Block block) {
	if (stbds_shgeti(in->blockmap, name) != -1) {
		log_error("Target '%s' already exists", name);
		return false;
	}
	uint32_t id = in->blocks.count;
	stbds_shput(in->blockmap, name, id);
	arraylist_append(&in->blocks, &block);
	return true;
}

bool process_block(Interpretter* in, int node) {
	if (in->ast->data[node].child_count == 0) {
		log_error("Failed to process block: No children found in node");
		return false;
	}

	if (in->ast->data[node + 1].type != AST_NODE_NAMES) {
		log_error("Failed to process block: No names node found");
		return false;
	}

	arraylist names = arraylist_new(sizeof(char*));
	int next = process_cmd(in, node + 1, &names, NULL);
	if (!next) return false;


	arraylist deps = arraylist_new(sizeof(char*));
	if (in->ast->data[next].type == AST_NODE_DEPS) {
		if (!process_cmd(in, next, &deps, NULL)) return false;
	}

	BlockType type;
	switch (in->ast->data[node].type) {
		case AST_NODE_LABEL: type = BLOCK_LABEL; break;
		case AST_NODE_RULE: type = BLOCK_RULE; break;
		case AST_NODE_MULTI: type = BLOCK_RULE; break;
		default: return false;
	}
	
	if (in->ast->data[node].type != AST_NODE_MULTI) {
		for (int i = 0; i < names.count; i++) {
			char* name = ((char**)names.items)[i];

			Block block = {0};
			block.name = name;
			block.type = type;
			block.deps = deps;
			block.node = node;

			if (!add_block(in, name, block)) return false;
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
			block.name = name;
			block.type = type;
			block.deps = arraylist_new(sizeof(char*));
			arraylist_append(&block.deps, ((char**)deps.items) + i);
			block.node = node;

			if (!add_block(in, name, block)) return false;

		}
	}

	return true;
}
bool run_prepass(Interpretter* in) {
	in->blocks = arraylist_new(sizeof(Block));

	for (int i = 0; i < in->ast->count; i++) {
		AstNode node = in->ast->data[i];
		switch (node.type) {
			case AST_NODE_VAR_DECL: {
				if (!process_var(in, i)) return false;
				break;
			}
			case AST_NODE_LABEL:
			case AST_NODE_RULE: 
			case AST_NODE_MULTI: {
				if (!process_block(in, i)) return false;
				break;
			}
			default:;
		}
	}

	return true;
}

bool should_execute(Interpretter* in, uint32_t id) {
    Block block = get_block(in, id);

    if (block.type == BLOCK_LABEL) return true;

    struct stat target;
    if (stat(block.name, &target) != 0)
        return true;

    foreach (char*, dep, block.deps) {
        struct stat dep_stat;

        if (stat(*dep, &dep_stat) != 0)
            return true;

		struct timespec dep_time = dep_stat.st_mtim;
		struct timespec target_time = target.st_mtim;

        if (dep_time.tv_sec > target_time.tv_sec
				|| (dep_time.tv_sec == target_time.tv_sec
				&& dep_time.tv_nsec > target_time.tv_nsec))
            return true;

    }

    return false;
}

bool execute_block(Interpretter* in, uint32_t id) {
	if (!should_execute(in, id)) return true;

	Block block = get_block(in, id);
	foreach (char*, dep, block.deps) {
		uint32_t id = find_block(in, *dep);
		if (id == -1) continue;

		if (!execute_block(in, id)) return false;
	}

	AstNode node = in->ast->data[block.node];
	int pos = block.node + 1;
	for (int i = 0; i < node.child_count; i++) {
		AstNodeType type = in->ast->data[pos].type;
		switch (type) {
			case AST_NODE_NAMES:
			case AST_NODE_DEPS:
				pos = skip_cmd(in->ast, pos);
				if (pos == -1) {
					log_error("Failed to execute block");
					return false;
				}
				break;
			case AST_NODE_CMD: {
				arraylist arglist = arraylist_new(sizeof(char*));
				pos = process_cmd(in, pos, &arglist, block.name);
				if (pos == -1) {
					log_error("Failed to execute block");
					return false;
				}

				if (arglist.count == 0) continue;

				foreach (char*, arg, arglist) {
					printf("%s ", *arg);
				}
				printf("\n");

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

void in_debug(Interpretter* in) {
	printf("\nVariables:\n");
	for (int i = 0; i < stbds_shlen(in->varmap); i++) {
		printf("    %s = ", in->varmap[i].key);
		foreach (char*, val, in->varmap[i].value) {
			printf("\e[38;5;243m%s\e[0m ", *val);
		}
		printf("\n");
	}

	printf("\nBlocks:\n");

	for (int i = 0; i < stbds_shlen(in->blockmap); i++) {
		printf("    %s: ", in->blockmap[i].key);
		uint32_t id = in->blockmap[i].value;
		Block block = get_block(in, id);
		foreach (char*, dep, block.deps) {
			printf("\e[38;5;243m%s\e[0m ", *dep);
		}
		printf("(%u:%u)", id, block.node);

		if (block.type == BLOCK_RULE)
			printf(" \e[38;5;243m%s\e[0m", should_execute(in, id) ? "old" : "up to date");
		printf("\n");
	}
	printf("\n");
}
