#ifndef STR_H
#define STR_H

#include <stddef.h>
#include <stdint.h>

// str that references part of a cstr
typedef struct {
	uint32_t src; // TODO: off
	uint32_t len;
} StrRef;

char* str_cstr(const char* src, StrRef str);
int str_contains(const char* src, StrRef str, char c);

// TODO: use this
typedef struct {
	char* str;
	size_t capacity;
	size_t count;
} StrBuilder;

void sb_append(StrBuilder* sb, const char* str);

#endif
