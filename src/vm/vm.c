#include "set.c"
#include "../format.c"

int main(int argc, char** argv) {

#ifdef BRUTE_FORCE_PROGRAM
	printw("USING HARDCODDED PROGRAM");
	Machine m = init_machine(program,arrlen(program));
#endif

#ifndef BRUTE_FORCE_PROGRAM
	if (argc != 2) {
		printf("\nUSAGE:\n"
				"\t set ./c_at_instruction_set.caic"
			  );
		return -1;
	}
	size_t proglen = 0;
	Instruction* program = load_from_file(argv[1], &proglen);
	Machine m = init_machine(program,proglen); 
#endif

	time_ms_t before, after;

	before = time_in_microsec();
	execute_program(&m);
	after = time_in_microsec();


#ifndef BRUTE_FORCE_PROGRAM
	free(program);
#endif

	return m.accum_register;
}
