#ifndef BUILTIN_H
#define BUILTIN_H

#include "arraylist.h"
#include <stdbool.h>


bool builtin_os(arraylist* args, int count, arraylist* out);
bool builtin_arch(arraylist* args, int count, arraylist* out);
bool builtin_env(arraylist* args, int count, arraylist* out);

bool builtin_find(arraylist* args, int count, arraylist* out);
bool builtin_echo(arraylist* args, int count, arraylist* out);
bool builtin_concat(arraylist* args, int count, arraylist* out);
bool builtin_pathsub(arraylist* args, int count, arraylist* out);
bool builtin_mkdir(arraylist* args, int count, arraylist* out);
bool builtin_prompt(arraylist* args, int count, arraylist* out);
bool builtin_dirname(arraylist* args, int count, arraylist* out);

bool exec_builtin(const char* name, arraylist* args, int count, arraylist* out);

#endif
