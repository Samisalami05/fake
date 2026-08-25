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

bool read_file(const char* path, FileView* out);
void close_file(FileView file);

uint64_t file_line_start(FileView file, uint64_t index);
uint64_t file_line_end(FileView file, uint64_t index);
StrRef file_line(FileView file, uint64_t index);

#endif
