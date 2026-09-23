#ifndef FILE_H
#define FILE_H

#include "str.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	char* ptr;
	size_t size;
} FileView;

typedef struct {
	uint64_t num;
	StrRef str;
} FileLine;

bool read_file(const char* path, FileView* out);
void close_file(FileView file);


uint64_t file_line_num(FileView file, uint64_t index);
uint64_t file_line_start(FileView file, uint64_t index);

// Includes '\n'
uint64_t file_line_end(FileView file, uint64_t index);

// Includes '\n'
FileLine file_line(FileView file, uint64_t index);

char* file_str_ref(FileView file, StrRef ref);

#endif
