#ifndef CACHE_H
#define CACHE_H

#include "arraylist.h"
#include <stdbool.h>

#define CACHE_PATH ".fake"

void cache_parse();
void cache_save();

void cache_store(char* name, arraylist values);
bool cache_read(char* name, arraylist* out);

#endif
