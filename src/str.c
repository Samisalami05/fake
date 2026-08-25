#include "str.h"
#include <stdlib.h>
#include <string.h>

// TODO: bounds check in 'src'
char* str_cstr(const char* src, StrRef str) {
	char* cstr = malloc(str.len + 1);
	memcpy(cstr, src + str.src, str.len);
	cstr[str.len] = '\0';
	return cstr;
}
