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

int str_contains(const char* src, StrRef str, char c) {
	int count = 0;
	for (int i = 0; i < str.len; i++) {
		if (src[str.src + i] == c) count++;
	}
	return count;
}

bool str_equals(const char* src, StrRef ref, char* other) {
	int len = strlen(other);
	if (len != ref.len) return false;
	return memcmp(src + ref.src, other, len) == 0;
}
