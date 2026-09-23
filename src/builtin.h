#ifndef BUILTIN_H
#define BUILTIN_H

#include "arraylist.h"
#include <stdbool.h>

bool exec_builtin(const char* name, arraylist* args, int count, arraylist* out);

#endif
