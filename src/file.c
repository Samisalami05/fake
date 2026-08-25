#include "file.h"
#include "str.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

bool read_file(const char* path, FileView* out) {
	if (!out) {
		fprintf(stderr, "read_file(): Could not read file %s: 'FileView* out' param is NULL\n", path);
		return false;
	}

	int fd = open(path, O_RDONLY);
	if (fd == -1) {
		perror("read_file(): open");
		return false;
	}

	struct stat st;
	if (fstat(fd, &st) == -1) {
		perror("fstat");
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
		perror("read_file(): mmap");
		close(fd);
		return false;
	}

	return true;
}

void close_file(FileView file) {
    if (!file.ptr || file.size <= 0) return;
	munmap(file.ptr, file.size);
}

uint64_t file_line_start(FileView file, uint64_t index) {
	while (index != 0 && file.ptr[index - 1] != '\n') {
		index--;
	}
	return index;
}

// Includes '\n'
uint64_t file_line_end(FileView file, uint64_t index) {
	while (index < file.size && file.ptr[index] != '\n') {
		index++;
	}
	return index;
}

StrRef file_line(FileView file, uint64_t index) {
	uint64_t start = file_line_start(file, index);
	uint64_t end = file_line_end(file, index);

	StrRef line = { start, end - start };
	return line;
}
