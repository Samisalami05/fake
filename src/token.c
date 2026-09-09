#include "fake.h"
#include "lex.h"

char* token_tag_str(TokenType tag) {
	switch (tag) {
		case TOKEN_IDENTIFIER: return "Identifier";
		case TOKEN_COLON: return ":";
		case TOKEN_STRING: return "String";
		case TOKEN_EOF: return "EOF";
		case TOKEN_CURLY_L: return "{";
		case TOKEN_CURLY_R: return "}";
		case TOKEN_COMMA: return ",";
		case TOKEN_PAREN_L: return "(";
		case TOKEN_PAREN_R: return ")";
		case TOKEN_EQUALS: return "=";
		case TOKEN_DOLLAR: return "$";
		case TOKEN_AT_SIGN: return "@";
	}
	return "Unknown";
}
