#include "log.h"
#include "file.h"
#include "str.h"
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static LogLevel log_level = LOG_VERBOSE;

void set_log_level(LogLevel level) {
	log_level = level;
}

LogLevel get_log_level() {
	return log_level;
}

char* log_type_style(LogType type) {
	switch (type) {
		case LOG_INFO: return "\e[33m";
		case LOG_WARN: return "\e[33m";
		case LOG_ERROR: return "\e[31m";
	}
	return "";
}

static char* log_type_str(LogType type) {
	switch (type) {
		case LOG_INFO: return "Info";
		case LOG_WARN: return "Warning";
		case LOG_ERROR: return "Error";
	}
	return "";
}

void log_info(const char* fmt, ...) {
	char* style = log_type_style(LOG_INFO);
	char* str = log_type_str(LOG_INFO);

	printf("%s[%s]\e[0m: ", style, str);

	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	printf("\n");
}

void log_warning(const char* fmt, ...) {
	char* style = log_type_style(LOG_WARN);
	char* str = log_type_str(LOG_WARN);

	fprintf(stderr, "%s[%s]\e[0m: ", style, str);

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fprintf(stderr, "\n");
}

void log_error(const char* fmt, ...) {
	char* style = log_type_style(LOG_ERROR);
	char* str = log_type_str(LOG_ERROR);

	fprintf(stderr, "%s[%s]\e[0m: ", style, str);

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fprintf(stderr, "\n");
}

void log_perror(const char* fmt, ...) {
	char* style = log_type_style(LOG_ERROR);
	char* str = log_type_str(LOG_ERROR);

	fprintf(stderr, "%s[%s]\e[0m: ", style, str);

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fprintf(stderr, ": %s\n", strerror(errno));
}

// Prints char n times
static void rep_print(char c, int n) {
	if (n <= 0) return;
	char buf[n];
	memset(buf, c, n);
	printf("%.*s", n, buf);
}

// TODO: Only search for the line once
void log_file(LogType type, FileView file, StrRef ref, const char* msg) {
	if (log_level == LOG_SILENT) return;

	uint64_t num = file_line_num(file, ref.src);
	uint64_t col = ref.src - file_line_start(file, ref.src) + 1;

	char* style = log_type_style(type);
	char* str = log_type_str(type);

	printf("%zu:%zu: %s[%s]\e[0m: %s\n", num, col, style, str, msg);

	if (log_level == LOG_VERBOSE)
		log_file_loc(type, file, ref);
}

void log_file_loc(LogType type, FileView file, StrRef ref) {
	FileLine line = file_line(file, ref.src);
	StrRef str = line.str;

	printf("%5zu | %.*s", line.num, (int)str.len, file.ptr + str.src);

	int tab_count = str_contains(file.ptr, str, '\t');
	uint64_t col = ref.src - str.src + 1;

	printf("      | ");
	rep_print('\t', tab_count);
	rep_print(' ', col - tab_count - 1);
	printf("%s^\e[0m\n", log_type_style(type));
}
