#include "file.h"
#include "log.h"
#include "str.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef _WIN32
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

bool read_file(const char* path, FileView* out) {
	assert(out != NULL);

#ifdef _WIN32
	FILE* fp = fopen(path, "r");
	if (!fp) return false;

	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	out->ptr = malloc(size);
	if (!out->ptr) {
		log_perror("read_file() - malloc");
		fclose(fp);
		return false;
	}

	out->size = size - 1;

	fread(out->ptr, 1, size - 1, fp);
	fclose(fp);

#else
	int fd = open(path, O_RDONLY);
	if (fd == -1) return false;

	struct stat st;
	if (fstat(fd, &st) == -1) {
		log_perror("read_file() - fstat: Could not stat '%s'", path);
		close(fd);
		return false;
	}

	// mmap doesnt like size set to zero
	if (st.st_size == 0) {
		out->ptr = NULL;
		out->size = 0;
		close(fd);
		return true;
	}

	out->size = (size_t)st.st_size;
	out->ptr = mmap(NULL, out->size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (out->ptr == MAP_FAILED) {
		log_perror("read_file() - mmap");
		close(fd);
		return false;
	}
#endif

	return true;
}

void close_file(FileView file) {
    if (!file.ptr || file.size <= 0) return;

#ifdef _WIN32
	free(file.ptr);
#else
	munmap(file.ptr, file.size);
#endif
}

uint64_t file_line_num(FileView file, uint64_t index) {
	uint64_t num = 0;
	while (index > 0) {
		if (file.ptr[index] == '\n')
			num++;
		index--;
	}
	return num + 1;
}

uint64_t file_line_start(FileView file, uint64_t index) {
	while (index != 0 && file.ptr[index - 1] != '\n') {
		index--;
	}
	return index;
}

uint64_t file_line_end(FileView file, uint64_t index) {
	while (index < file.size && file.ptr[index] != '\n') {
		index++;
	}
	return index;
}

FileLine file_line(FileView file, uint64_t index) {
	uint64_t start = file_line_start(file, index);
	uint64_t end = file_line_end(file, index);

	FileLine line = {
		.num = file_line_num(file, index),
		.str = (StrRef){
			.src = start,
			.len = end - start + 1
		}
	};
	return line;
}

char* file_str_ref(FileView file, StrRef ref) {
	char* str = malloc(ref.len + 1);
	if (str == NULL) {
		log_perror("file_str_ref() - malloc");
		return NULL;
	}

	memcpy(str, file.ptr + ref.src, ref.len);
	str[ref.len] = '\0';
	return str;
}
