#ifndef LOG_H
#define LOG_H

#include "file.h"
#include "str.h"

typedef enum {
	LOG_SILENT,
	LOG_MINIMAL,
	LOG_VERBOSE,
} LogLevel;

typedef enum {
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
} LogType;

void set_log_level(LogLevel level);
LogLevel get_log_level();

// Is returned as an asni escape code
char* log_type_style(LogType type);

void log_info(const char* fmt, ...);
void log_warning(const char* fmt, ...);
void log_error(const char* fmt, ...);
void log_perror(const char* fmt, ...);

// Prints the line of the ref with a given message
void log_file(LogType type, FileView file, StrRef ref, const char* msg);
void log_file_loc(LogType type, FileView file, StrRef ref);

#endif
