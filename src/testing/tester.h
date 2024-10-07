#include "../vm/set.c"
#include "../format.c"

#pragma once

#define VM_PATH "./bin/set"
#define GIT_REPO "git@github.com:amuerta/c-at.git"

#define TEST_ASM false

typedef struct {
	uint 			desired_return	;
	char* 			name			;
	Instruction* 	program			;
	size_t			program_size	;
} TestProgram;

typedef struct {
	bool 			sucess			;
	byte 			return_code		;
	time_ms_t 		time_elapsed	;
} TestResult;

#ifdef TEST_DEFINITION

#endif
