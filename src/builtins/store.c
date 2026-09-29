#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../arraylist.h"
#include "../log.h"
#include "../cache.h"

bool builtin_store(arraylist* args, int count, arraylist* out) {
	if (count != 2) {
		log_error("@store(): Expects two argument, got %d\n", count);
		return false;
	}
	
	arraylist values = args[1];

	foreach(char*, arg, args[0]) {
		cache_store(*arg, values);
	}

	return true;
}
