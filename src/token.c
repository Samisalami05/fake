#include "fake.h"

char* token_tag_str(token_type tag) {
	switch (tag) {
		case token_identifier: return "Identifier";
		case token_colon: return ":";
		case token_string: return "\"";
		case token_eof: return "EOF";
		case token_curly_l: return "{";
		case token_curly_r: return "}";
		case token_comma: return ",";
		case token_paren_l: return "(";
		case token_paren_r: return ")";
	}
	return "";
}
