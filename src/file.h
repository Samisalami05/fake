#ifndef FILE_H
#define FILE_H

#include "str.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(__linux__) || defined(__APPLE__)
#include <linux/limits.h>
#include <dirent.h>
#else
#include <windows.h>
#endif

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

#if defined(__linux__) || defined(__APPLE__)
typedef DIR* Dir;
typedef struct dirent* DirEntry;
#else
typedef struct {
	HANDLE handle;
	const char* path;
} Dir;
typedef WIN32_FIND_DATAA DirEntry;
#endif

typedef struct {
	uint64_t sec;
	uint64_t nsec;
} TimeStamp;

bool timestamp_is_newer(TimeStamp a, TimeStamp b);

bool file_last_modified(const char* path, TimeStamp* out);
bool remove_file(const char* path);
bool remove_dir(const char* path);

bool open_dir(const char* path, Dir* out);
bool read_dir(Dir* dir, DirEntry* entry);
bool close_dir(Dir* dir);

bool entry_is_dir(DirEntry* entry);
char* entry_name(DirEntry* entry);

#endif
