#include "set.c"

int main(int argc, char** argv) {

#ifdef BRUTE_FORCE_PROGRAM




	if (argc != 2) {
		printf("\nUSAGE:\n"
				"\t set ./c_at_instruction_set.caic"
			  );
		return -1;
	}

	Machine m = init_machine(program,arrlen(program));
#endif



#ifndef BRUTE_FORCE_PROGRAM
	size_t proglen = 0;
	Instruction* program = load_from_file(argv[1], &proglen);
	Machine m = init_machine(program,proglen); 
#endif


#ifdef DEBUG
	show_debug_statistic(m);
#endif



	time_ms_t before, after;

	before = time_in_microsec();
	execute_program(&m);
	after = time_in_microsec();



#ifdef DEBUG
	printf(FMT_PADDING);
	printf("------------ TIME -------------\n");
	printf("elapsed execution time in ms: %llu",(after-before) );
	printf(FMT_PADDING);
	printf("--------- REGISTERS -----------\n");
	for(uint i = 0; i < INT_REG_COUNT; i++) {
		printf("r%d = %d, fr%d = %f\n",
				i,m.iregisters[i],
				i,m.fregisters[i]
			  );
	}

	printf(FMT_PADDING "STACK PRINT: {\n");
	for(uint i = 0; i < 64; i++) {
		if (i % 16 == 0) {
			printf("\n");
		}
		printf(" [%i] ",STACK[i]);
	}
	printf("\n\n}\t stack_top : %lu\n",m.stack_top);
	printf(FMT_PADDING );



	printf("--------- DISASSEBLED CODE: ------------\n");
	/* print_program_asm(program,proglen); */
	print_program_asm(m.program,m.program_size);
	printf("\n----------------------------------------\n");
#endif

#ifndef BRUTE_FORCE_PROGRAM
	free(program);
#endif

	return m.accum_register;
}
