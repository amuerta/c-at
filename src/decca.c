#include "set.h"
#include "disassembler.c"

int main(int argc, char** argv) {
	if (argc!=2)
		printf("\nUSAGE:\n"
				"\t decca ./c_at_compiled_instructions.caic");
	
	size_t ps = 0;
	Instruction* program = load_from_file(argv[1],&ps);


	printf("PROGRAM ASSEMBLY:\n");
	if (program != NULL) 
		print_program_asm(program,ps);
	printf("\n\nEND OF THE CODE\n");

	free(program);
	return 0;
}

