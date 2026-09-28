#include "builtin.h"
#include "arraylist.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


bool exec_builtin(const char* name, arraylist* args, int count, arraylist* out) {
	if      (strcmp(name, "os") == 0)       return builtin_os(args, count, out);
	else if (strcmp(name, "arch") == 0)     return builtin_arch(args, count, out);
	else if (strcmp(name, "env") == 0)      return builtin_env(args, count, out);
	else if (strcmp(name, "find") == 0)     return builtin_find(args, count, out);
	else if (strcmp(name, "echo") == 0)     return builtin_echo(args, count, out);
	else if (strcmp(name, "concat") == 0)   return builtin_concat(args, count, out);
	else if (strcmp(name, "pathsub") == 0)  return builtin_pathsub(args, count, out);
	else if (strcmp(name, "mkdir") == 0)    return builtin_mkdir(args, count, out);
	else if (strcmp(name, "prompt") == 0)   return builtin_prompt(args, count, out);
	else if (strcmp(name, "dirname") == 0)  return builtin_dirname(args, count, out);
	return false;
}
