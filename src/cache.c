#include "cache.h"
#include "arraylist.h"
#include "file.h"
#include "log.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb_ds.h"
#include "str.h"

static struct {
	char* key;
	arraylist value;
}* cache = NULL;

void cache_parse() {
	FileView file = {0};
	if (!read_file(CACHE_PATH, &file)) return;

	int pos = 0;
	while (pos < file.size) {
		StrRef ref = { pos, 0 };
		while (pos < file.size && file.ptr[pos] != ':') pos++;
		ref.len = pos - ref.src;

		char* name = str_cstr(file.ptr, ref);
		pos++;

		// Skip whitespace
		while (pos < file.size && file.ptr[pos] == ' ') pos++;

		if (pos >= file.size || file.ptr[pos] == '\n') continue;

		arraylist values = arraylist_new(sizeof(char*));
		StrRef val = {pos, 0};
		while (pos < file.size && file.ptr[pos] != '\n') {
			if (file.ptr[pos] == ' ') {
				val.len = pos - val.src;
				char* value = str_cstr(file.ptr, val);
				arraylist_append(&values, &value);
				val.src = pos + 1;
			}
			pos++;
		}

		val.len = pos - val.src;
		char* value = str_cstr(file.ptr, val);
		arraylist_append(&values, &value);

		cache_store(name, values);
		
		pos++;
	}
	
close:
	close_file(file);
}

void cache_save() {
	FILE* fp = fopen(CACHE_PATH, "w");
	if (!fp) {
		log_error("Failed to save cache: %s\n", strerror(errno));
		return;
	}

	for (int i = 0; i < stbds_shlen(cache); i++) {
		char* name = cache[i].key;
		arraylist values = cache[i].value;

		fprintf(fp, "%s:", name);
		foreach (char*, val, values) {
			fprintf(fp, " %s", *val);
		}
		fprintf(fp, "\n");
	}

	fclose(fp);
}

void cache_store(char* name, arraylist values) {
	stbds_shput(cache, name, values);
}

bool cache_read(char* name, arraylist* out) {
	int id = stbds_shgeti(cache, name);
	if (id == -1) return false;

	if (out != NULL) {
		arraylist values = cache[id].value;
		arraylist_appendn(out, values.items, values.count);
	}

	return true;
}
