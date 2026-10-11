#ifndef BUILTIN_H
#define BUILTIN_H

#include "arraylist.h"
#include "fake.h"
#include <stdbool.h>

#define BUILTIN_OK ST_OK
#define BUILTIN_ERROR ST_ERROR
#define BUILTIN_EXIT ST_EXIT

int builtin_os(arraylist* args, int count, arraylist* out);
int builtin_arch(arraylist* args, int count, arraylist* out);
int builtin_env(arraylist* args, int count, arraylist* out);

int builtin_find(arraylist* args, int count, arraylist* out);
int builtin_echo(arraylist* args, int count, arraylist* out);
int builtin_concat(arraylist* args, int count, arraylist* out);
int builtin_repeat(arraylist* args, int count, arraylist* out);
int builtin_pathsub(arraylist* args, int count, arraylist* out);
int builtin_mkdir(arraylist* args, int count, arraylist* out);
int builtin_rm(arraylist* args, int count, arraylist* out);
int builtin_rmdir(arraylist* args, int count, arraylist* out);
int builtin_rmrf(arraylist* args, int count, arraylist* out);
int builtin_prompt(arraylist* args, int count, arraylist* out);
int builtin_dirname(arraylist* args, int count, arraylist* out);

int builtin_store(arraylist* args, int count, arraylist* out);
int builtin_read(arraylist* args, int count, arraylist* out);
int builtin_stored(arraylist* args, int count, arraylist* out);
int builtin_option(arraylist* args, int count, arraylist* out);
int builtin_selection(arraylist* args, int count, arraylist* out);

int exec_builtin(const char* name, arraylist* args, int count, arraylist* out);

#endif
