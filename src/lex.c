#include "fake.h"

#include <stdlib.h>
#include <stdio.h>


void printErr(parse_state* state, uint32_t curr, char* message);

void lex(parse_state *state) {
	uint8_t identifier_map[256] = {0};
	for (uint32_t i = 0; i < 256; i++) {
		if (i >= 'a' && i <= 'z') identifier_map[i] = 1;
		if (i >= 'A' && i <= 'Z') identifier_map[i] = 1;
		if (i >= '0' && i <= '9') identifier_map[i] = 1;
		if (i == '_') identifier_map[i] = 1;
	}

	state->token_allocated = 65536;
	state->token_count = 0;
	state->tokens = malloc(sizeof(token)*state->token_allocated);
	uint32_t curr = 0;
dont_tell_johnny: while (curr < state->file_size) {
		//printf("what is happening: '%c'\n", state->file_str[curr]);
		switch (state->file_str[curr]) {
			case ':':
				state->tokens[state->token_count++] = (token){
					.tag = token_colon,
					.index = curr,
				};
				break;
			case ',':
				state->tokens[state->token_count++] = (token){
					.tag = token_comma,
					.index = curr,
				};
				break;
			case '{':
				state->tokens[state->token_count++] = (token){
					.tag = token_curly_l,
					.index = curr,
				};
				break;
			case '}':
				state->tokens[state->token_count++] = (token){
					.tag = token_curly_r,
					.index = curr,
				};
				break;
			case '(':
				state->tokens[state->token_count++] = (token){
					.tag = token_paren_l,
					.index = curr,
				};
				break;
			case ')':
				state->tokens[state->token_count++] = (token){
					.tag = token_paren_r,
					.index = curr,
				};
				break;
			case ' ':
			case '\t':
			case '\n':
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				{
					//printf("breaking bad (this code)\n");
					printErr(state, curr, "breaking bad (this code)"); // Maybe should not be printErr
					exit(1);
				}
			// small letters
			case 97:
			case 98:
			case 99:
			case 100:
			case 101:
			case 102:
			case 103:
			case 104:
			case 105:
			case 106:
			case 107:
			case 108:
			case 109:
			case 110:
			case 111:
			case 112:
			case 113:
			case 114:
			case 115:
			case 116:
			case 117:
			case 118:
			case 119:
			case 120:
			case 121:
			case 122:
			// big letters
			case 65:
			case 66:
			case 67:
			case 68:
			case 69:
			case 70:
			case 71:
			case 72:
			case 73:
			case 74:
			case 75:
			case 76:
			case 77:
			case 78:
			case 79:
			case 80:
			case 81:
			case 82:
			case 83:
			case 84:
			case 85:
			case 86:
			case 87:
			case 88:
			case 89:
			case 90:
				{
					uint32_t add_index = curr;
					for (; curr < state->file_size; curr++) {
						if (0 == identifier_map[(uint8_t)state->file_str[curr]]) break;
					}
					state->tokens[state->token_count++] = (token){
						.tag = token_identifier,
						.index = add_index,
					};
					continue;
				}
				break;
			case '"':
				{
					uint32_t add_index = curr;
					curr++;
					for (; curr < state->file_size; curr++) {
						if (state->file_str[curr] == '"') {
							curr++;
							state->tokens[state->token_count++] = (token){
								.tag = token_string,
								.index = add_index,
							};
							goto dont_tell_johnny;
						}
					}
					printErr(state, curr, "found no matching '\"'");
					exit(1);
				}
				break;
			default: {
						 printErr(state, curr, "illegal character");
						 exit(1);
					 };
		}
		curr += 1;
	}
	state->tokens[state->token_count++] = (token){
		.tag = token_eof,
		.index = curr,
	};
}

str_ref get_token_str(parse_state *state, uint32_t token_index) {
	uint32_t src = state->tokens[token_index].index;
	uint32_t dst = state->tokens[token_index+1].index;

	int i; // new dst
	for (i = src; i < dst; i++) {
		// synchronize with tokenless characters!
		if (state->file_str[i] == ' ') break;
		if (state->file_str[i] == '\t') break;
		if (state->file_str[i] == '\n') break;
	}

	return (str_ref){
		.src = src,
		.len = i-src,
	};
}

uint32_t getLineStart(parse_state* state, uint32_t curr) {
	uint32_t loc = curr;
	while (loc != 0 && state->file_str[loc - 1] != '\n') {
		loc--;
	}
	return loc;
}

// Includes '\n'
uint32_t getLineEnd(parse_state* state, uint32_t curr) {
	uint32_t loc = curr;
	while (loc < state->file_size && state->file_str[loc] != '\n') {
		loc++;
	}
	return loc;
}

// Todo: fix problem with line start/end when colon is not in second command
// Todo: buffer optimize the prints
// Todo: maybe move to a different file so it can be included in main.c
void printErr(parse_state* state, uint32_t curr, char* message) {
	uint32_t lineStart = getLineStart(state, curr);
	uint32_t lineEnd = getLineEnd(state, curr);

	uint32_t tabCount = 0;
	
	//printf("char: %c, start: %u, end: %u\n", state->file_str[curr], lineStart, lineEnd);

	fprintf(stderr, "%u: \033[1;31merror:\033[0m %s\n", curr, message);
	
	// Print line
	for (int i = lineStart; i < lineEnd; i++) {
		char c = state->file_str[i];
		if (c == '\t') tabCount++;
		printf("%c", c);
	}
	
	printf("\n");
	
	// Print empty space for arrow
	for (int i = 0; i < tabCount; i++) {
		printf("\t");
	}
	for (int i = lineStart + tabCount; i < curr; i++) {
		printf(" ");
	}

	printf("\033[1;31m^\033[0m"); // Print red arrow

	printf("\n");
}
