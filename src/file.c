#include "file.h"
#include "log.h"
#include "str.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef _WIN32
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h> // TODO: Remove this
#else
#include <windows.h>
#include <shlwapi.h>
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

	out->size = size;

	fread(out->ptr, 1, size, fp);
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

bool timestamp_is_newer(TimeStamp a, TimeStamp b) {
	if (a.sec > b.sec) return true;
	if (a.sec == b.sec && a.nsec > b.nsec) return true;
	return false;
}

bool file_is_dir(const char* path) {
#ifndef _WIN32
	struct stat s;
	if (stat(path, &s) == -1) {
		return false;
	}
	return S_ISDIR(s.st_mode);
#else
	DWORD attrib = GetFileAttributes(path);
    return (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
#endif
}

bool file_last_modified(const char* path, TimeStamp* out) {
#ifdef _WIN32
	HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return false;

	FILETIME ftLastWrite;
	if (!GetFileTime(hFile, NULL, NULL, &ftLastWrite)) {
		CloseHandle(hFile);
		return false;
	}

	CloseHandle(hFile);

	// Convert FILETIME to TimeStamp
	ULARGE_INTEGER ull;
	ull.LowPart = ftLastWrite.dwLowDateTime;
	ull.HighPart = ftLastWrite.dwHighDateTime;

	// FILETIME is in 100-nanosecond intervals since January 1, 1601 (UTC)
	// Convert to seconds and nanoseconds since Unix epoch (January 1, 1970)
	const uint64_t EPOCH_DIFF = 11644473600ULL; // Difference in seconds between 1601 and 1970
	uint64_t total_seconds = ull.QuadPart / 10000000ULL - EPOCH_DIFF;
	uint64_t total_nanoseconds = (ull.QuadPart % 10000000ULL) * 100;

	out->sec = total_seconds;
	out->nsec = total_nanoseconds;
#else
	struct stat st;
	if (stat(path, &st) != 0) {
		return false;
	}

	out->sec = st.st_mtim.tv_sec;
	out->nsec = st.st_mtim.tv_nsec;
#endif

	return true;
}

bool remove_file(const char* path) {
#ifdef _WIN32
	if (!DeleteFileA(path)) {
		log_perror("Failed to remove file '%s'", path);
		return false;
	}
#else
	if (unlink(path) != 0) {
		log_perror("Failed to remove file '%s'", path);
		return false;
	}
#endif
	return true;
}

bool remove_dir(const char* path) {
#ifdef _WIN32
	if (!RemoveDirectoryA(path)) {
		log_perror("Failed to remove directory '%s'", path);
		return false;
	}
#else
	if (rmdir(path) != 0) {
		log_perror("Failed to remove directory '%s'", path);
		return false;
	}
#endif
	return true;
}

// Returns null on error and status is set to indicate the error.
bool open_dir(const char* path, Dir* out) {
#ifndef _WIN32
    Dir dir = opendir(path);
    if (!dir) {
        log_perror("Failed to open directory '%s'", path);
        return false;
    }
    *out = dir;
    return true;
#else
    *out = (Dir){.handle = INVALID_HANDLE_VALUE, .path = path};
    return true;
#endif
}

bool read_dir(Dir* dir, DirEntry* entry) {
#ifndef _WIN32
    *entry = readdir(*dir);
	if (!*entry) return false;
    return true;
#else
	if (dir->handle == INVALID_HANDLE_VALUE) {
		char search_path[MAX_PATH];
		snprintf(search_path, sizeof(search_path), "%s\\*", dir->path);
		dir->handle = FindFirstFileA(search_path, entry);
		if (dir->handle == INVALID_HANDLE_VALUE) {
			log_perror("Failed to read directory");
			return false;
		}
		return true;
	}

    if (!FindNextFileA(dir->handle, entry)) {
        if (GetLastError() != 0 && GetLastError() != ERROR_NO_MORE_FILES) {
            log_perror("Failed to read directory");
        }
        return false;
    }
    return true;
#endif
}

bool close_dir(Dir* dir) {
#ifndef _WIN32
    if (closedir(*dir) != 0) {
        log_perror("Failed to close directory");
        return false;
    }
    return true;
#else
    if (!FindClose(dir->handle)) {
        log_perror("Failed to close directory");
        return false;
    }
    return true;
#endif
}

bool entry_is_dir(DirEntry* entry) {
#ifndef _WIN32
	struct dirent* ent = *entry;
	if (ent->d_type == DT_UNKNOWN) {
		struct stat s;
		if (stat(ent->d_name, &s) == -1) {
			log_perror("Failed to stat file '%s'", ent->d_name);
			return false;
		}
		return S_ISDIR(s.st_mode);
	}
	return ent->d_type == DT_DIR;
#else
	return (entry->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#endif
}

char* entry_name(DirEntry* entry) {
#ifndef _WIN32
	return (*entry)->d_name;
#else
	return entry->cFileName;
#endif
}
