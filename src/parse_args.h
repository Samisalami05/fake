#pragma once
#include <stdint.h>

/*
 * Argument parsing for fake
 *
 * Using arguments, users can set flags, decide what target to execute and get help
 */


/* TODO: when more has been made, support storing configuration and flags
typedef enum {
	fake_role_build, // default, build everything
	fake_role_help, // show help messages
} fake_role;

typedef struct {
	fake_role role;
	// ...
} fake_config ;
*/

void parse_args(char **argv);
