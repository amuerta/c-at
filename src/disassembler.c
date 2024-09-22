#pragma once

// forward declate
void print_program_asm
	(Instruction* program, size_t program_size);


void print_instruction_asm
	(Instruction i);

int write_to_file(char* fpath,Instruction* program, size_t psize) 
{
	FILE* 	fptr	;
	fptr = fopen(fpath,"wb");

	if (!fptr) {
		fprintf(stderr,"ERROR:\tFAILED TO CREATE FILE(%s)\n",
				fpath);
		return -1; // TODO: ERROR ENUM
	}

	fwrite(program,sizeof(Instruction),psize,fptr);
	fclose(fptr);

	return 1;
} 

Instruction* load_from_file(char* fpath,size_t* program_size) 
{

	FILE* 	fptr	;
	char*  	fbuff	;
	size_t 	flen	;
	Instruction* instructions;

	fptr = fopen(fpath, "rb");

	if (!fptr) {
		fprintf(stderr,"ERROR:\tFAILED TO LOAD FILE(%s)\n",
				fpath);
		return NULL; // TODO: ERROR ENUM
	}

	fseek(fptr, 0, SEEK_END);          
	flen = ftell(fptr);             
	rewind(fptr);                      

	fbuff = (byte*)  malloc(flen * sizeof(byte)); 
	fread(fbuff, flen, 1, fptr); 
	fclose(fptr); 

	*program_size = flen / sizeof(Instruction);
	instructions = (Instruction*)fbuff;

	if( !(*program_size % 16) )
		printf( "WARNING: UNEVEN BYTE COUNT FOR INSTRUCTION SET\n");

	return instructions;
}



void print_program_asm(Instruction* program, size_t program_size) 
{
	for(uint i = 0; i < program_size; i++) {
		print_instruction_asm(program[i]);
	}
}

void print_instruction_asm(Instruction i) {
		Instruction inst = i;

		byte op 	= 	inst.operation	;
		byte r0		= 	inst.argument_reg[0];
		byte r1		=	inst.argument_reg[1];
		byte r2		=	inst.argument_reg[2];

		UniformStrg immd 	=   inst.storage;
		DataType t  		= 	inst.operand_type;

		// CRASH CHECK
		switch((Inst)op) {
			case I_FADD:
			case I_FSUB:
			case I_FDIV:
			case I_FMUL:
			case I_FSET:
			case I_FLOAD:
			case I_FSAVE:
				assert(t == f32 && 
						"INSTRUCTION TYPE FOR FLOATS ONLY"
						"ACCEPTS FLOATING POINT CONTEXT  "
						"\t CURRENTLY DISASSEMBLED PROGRAM"
						"IS CORRUPTED AND HAS INVALID FLT OPERAND"
					  );
				break;

			default: break;
		}

		switch ((Inst)op) {
			case I_HALT:
				printf("\n\thalt");
				break;

			case I_INC:
				printf("\n\tinc @r%d",r0);
				break;

			case I_PUSH:
				printf("\n\tpush @r%d",r0);
				break;
	
			case I_POP:
				printf("\n\tpop @r%d",r0);
				break;

			case I_CALL:
				printf("\n\tcall @address(%lu)",(size_t)immd.as_i32);
				break;

			case I_REC:
				printf("\n\trec ");
				break;

			case I_GETAC:
				printf("\n\tgetac @r%d",r0);
				break;

			case I_DEC:
				printf("\n\tdec @r%d",r0);
				break;

			case I_ADD:
				printf("\n\tadd @r%d, @r%d, @r%d",r0, r1, r2);
				break;

			case I_SUB:
				printf("\n\tsub @r%d, @r%d, @r%d",r0, r1, r2);
				break;

			case I_MUL:
				printf("\n\tmul @r%d, @r%d, @r%d",r0, r1, r2);
				break;

			case I_DIV: 
				printf("\n\tdiv @r%d, @r%d, @r%d",r0, r1, r2);
				break;

			case I_FADD:
				printf("\n\tfadd @fr%d, @fr%d, @fr%d",r0, r1, r2);
				break;

			case I_FSUB:
				printf("\n\tfsub @fr%d, @fr%d, @fr%d",r0, r1, r2);
				break;

			case I_FDIV:
				printf("\n\tfdiv @fr%d, @fr%d, @fr%d",r0, r1, r2);
				break;

			case I_FMUL:
				printf("\n\tfmul @fr%d, @fr%d, @fr%d",r0, r1, r2);
				break;

			case I_DLOAD:
				printf("\n\tdload @r%d, @r%d",r0, r1);
				break;

			case I_FLOAD:
				printf("\n\tfload @r%d, @r%d",r0, r1);
				break;

			case I_DSAVE:
				printf("\n\tdsave @r%d, @r%d",r0, r1);
				break;

			case I_FSAVE:
				printf("\n\tfsave @r%d, @r%d",r0, r1);
				break;

			case I_FSET:
				printf("\n\tfset @r%d, $(%f)",r0, (float)immd.as_i32);
				break;

			case I_SET:
				printf("\n\tset @r%d, $(%i)",r0, immd.as_i32);
				break;

			case I_AND:
				printf("\n\tand @r%d, @r%d, @r%d",r0, r1, r2);
				break;

			case I_OR:
				printf("\n\tor @r%d, @r%d, @r%d",r0, r1, r2);
				break;

			case I_JMP:
				printf("\n\tjmp @address(%d)",immd.as_i32);
				break;

			case I_EJMP:
				printf("\n\tejmp @address(%d), @r%d, @r%d",immd.as_i32,r0,r1);
				break;

			case I_NEJMP:
				printf("\n\tnejump @address(%d), @r%d, @r%d",immd.as_i32,r0,r1);
				break;

			case I_PRINT:
				printf("\n\tprint -> sys:write $r{0,1,2}");
				break;

			case I_REG_DUMP:
				printf("\n\treg_dump#debug @r%d",r0);
				break;

			case I_NOP:
				printf("\n\tnop");
				break;

			case I_ENTRY:
				printf("\nENTRY:\n");
				break;

			case I_RET:
				printf("\n\tret @r%d\n",r0);
				break;

			default:
				fprintf(stderr,"\n\tERROR: UNSUPPORTED INSTRUCTION");
				break;

		}
}

