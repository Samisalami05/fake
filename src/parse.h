#pragma once

#include "arraylist.h"
#include "fake.h"
#include "file.h"
#include "lex.h"
#include <stdbool.h>


// TODO: build parse tree for lsp

bool parse_fakefile(FileView file, Tokens tokens, Fakefile* out);
