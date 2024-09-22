#include "set.h"
#include "disassembler.c"
#include "../lib/cliargs.h"


// TODO:
// unsinged value specific operations
// scmp singned cmp
// ucmp unsigned cmp
// fcmp float cmp

#define DEBUG

uint 	D_REGISTERS[INT_REG_COUNT];
float 	F_REGISTERS[FLT_REG_COUNT];

byte STACK[STACK_SIZE];
byte FRAME[STACK_SIZE];



Machine init_machine(Instruction *p, size_t pl) {
	return (Machine) {
		.pc = 0,
		.program = p,
		.program_size = pl,
		.dregisters = D_REGISTERS,
		.fregisters = F_REGISTERS,
		.stack_memory = (USE_STACK)? 
			(STACK) : (NULL), 		// TODO
		.frame_memory = {0},
		.heap_address_space = NULL,	// TODO

		.is_running = 1				
	};
}

Instruction inst_next(Machine *m) {
	return m->program[m->pc++];
}


void print_regs(uint* dregisters, float* fregisters) {
	assert(FLT_REG_COUNT == INT_REG_COUNT && "BROKEN ASSUMPTION HERE");
	for(uint i = 0; i < FLT_REG_COUNT; i++)
		printf(
				"\t | $dr%d: %d,\t  $fr%d: %f\n",
				i,dregisters[i],
				i,fregisters[i]
			);
}

void push_decimal 	(byte* memory,size_t* top, uint value) {
	memcpy(&memory[(*top)+=4],&value,sizeof(uint));
}

uint pop_decimal 	(byte* memory, size_t* top) 
{
	uint value = 0;
	memcpy(&value,&memory[(*top)],sizeof(uint));
	(*top)-=4;
	return value;
}

uint load_decimal	(byte* memory, uint address) 
{
	uint value = 0;
	memcpy(&value,&memory[address],sizeof(uint));
	return value;
}

void save_decimal	(byte* memory, uint address, uint value) 
{
	memcpy(&memory[address],&value,sizeof(uint));
}

float load_float	(byte* memory, uint address) 
{
	float value = 0;
	memcpy(&value,&memory[address],sizeof(float));
	return value;
}

void save_float	(byte* memory, uint address, float value) 
{
	memcpy(&memory[address],&value,sizeof(float));
}

void record (CallFrame* cframe,size_t*frames, size_t addr) {
	cframe[(*frames)] = (CallFrame) {
		.bfp = addr,
	};
}

void call	(CallFrame* cframe,
			size_t* frames,
			size_t* pc, 
			size_t addr,
			size_t jmp_addr) {
	cframe[(*frames)] = (CallFrame) {
		.rip = (*pc),
		.lvp = addr,
	};
	(*pc) = jmp_addr;
	printf("\tMORE (rip): %lu\n",cframe[(*frames)].rip);
	(*frames)++;
}

void execute_op(Machine *m) {

		Instruction inst = inst_next(m);
		
		uint* 	d_reg = m->dregisters;
		float* 	f_reg = m->fregisters;

		byte op 	= 	inst.operation	;
		byte r0		= 	inst.argument_reg[0];
		byte r1		=	inst.argument_reg[1];
		byte r2		=	inst.argument_reg[2];

		UniformStrg immd 	=   inst.storage;
		/* uint immd 	=   inst.storage; */
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
						);
				break;

			default: break;
		}

		switch ((Inst)op) {
			
			case I_ENTRY:
				assert(0 && "TODO: ENTRY IS CURRENTLY NOT USED");
				break;

			case I_HALT:
				m->is_running = 0;
				break;
			
			case I_PUSH:
				push_decimal(STACK,&m->stack_top, d_reg[r0]);
				break;

			case I_POP:
				d_reg[r0] = pop_decimal(STACK, &m->stack_top); 
				break;

			case I_REC:
				record (m->frame_memory,&m->frames, m->stack_top);
				break;

			case I_CALL:
				m->pc += 1;
				call   (m->frame_memory,&m->frames,&m->pc, m->stack_top,immd.as_i32);
				printf("CALL (frames, ip): %lu, %lu\n",m->frames,m->pc);
				break;

			case I_GETAC:
				d_reg[r0] 	= m->accum_register;
				break;

			case I_INC:	
					d_reg[r0]++;
				break;

			case I_DEC:
					d_reg[r0]--;
				break;

			case I_ADD:
				d_reg[r0]	= d_reg[r1] + d_reg[r2];
				break;
			
			case I_SUB:
				d_reg[r0]	= d_reg[r1] - d_reg[r2];
				break;
			
			case I_MUL:
				d_reg[r0]	= d_reg[r1] * d_reg[r2];
				break;
			
			case I_DIV: 
				d_reg[r0]	= d_reg[r1] / d_reg[r2];
				break;
			
			case I_FADD:
				f_reg[r0]	= f_reg[r1] + f_reg[r2];
				break;

			case I_FSUB:
				f_reg[r0]	= f_reg[r1] - f_reg[r2];
				break;
			
			case I_FDIV:
				f_reg[r0]	= f_reg[r1] / f_reg[r2];
				break;
			
			case I_FMUL:
				f_reg[r0]	= f_reg[r1] * f_reg[r2];
				break;
			
			case I_DLOAD:
					d_reg[r0] = load_decimal(STACK,d_reg[r1]);
				break;

			case I_FLOAD:
					f_reg[r0] = load_float(STACK,r1);
				break;
			
			case I_DSAVE:
					save_decimal(STACK,d_reg[r1],d_reg[r0]);
				break;

			case I_FSAVE:
					save_float(STACK,d_reg[r1],f_reg[r0]);
				break;
			
			case I_FSET:
				f_reg[r0] = immd.as_f32;
				break;

			case I_SET:
				d_reg[r0] = immd.as_i32;
				break;
			
			case I_AND:
				d_reg[r0] = (d_reg[r1] && d_reg[r2]);
				break;
			
			case I_OR:
				d_reg[r0] = (d_reg[r1] || d_reg[r2]);
				break;

				// Jump
				// unlike RJMP jump to litreall address
				// passed as first argument
			
			case I_JMP:
				m->pc = (uint)immd.as_i32;
				break;

			case I_EJMP:
				if (d_reg[r1] == d_reg[r2]) 
					m->pc = (uint)immd.as_i32;
				break;

			case I_NEJMP:
				if (d_reg[r1] != d_reg[r2]) 
					m->pc = (uint)immd.as_i32;
				break;

				// Register Jump
				// performs jump to location stored in reg0
			case I_RJMP:
				m->pc = d_reg[r0];
				break;

			case I_ERJMP:
				if (d_reg[r1] == d_reg[r2]) 
					m->pc = d_reg[r0];
				break;

			case I_NERJMP:
				if (d_reg[r1] != d_reg[r2]) 
					m->pc = d_reg[r0];
				break;

			case I_PRINT:

				#ifdef DEBUG
					printf("WRITE(s:%d,b: ptr(%d),l:%d)\n",
							d_reg[0],d_reg[1],d_reg[2]);
				#endif

				int r = write(d_reg[0],&STACK[d_reg[1]],d_reg[2]);
				assert(r && "ERROR PERFORMING WRITE");
				break;

			case I_REG_DUMP:
				printf("REGISTER r%d VALUE: \t%d\n",r0,d_reg[r0]);
				break;

			case I_RET:
				m->accum_register = d_reg[r0];
				if (m->frames) {
					printf("RET (frames, ip): %lu, %lu\n",
							m->frames,
							m->frame_memory[m->frames-1].rip
					);
					// TODO: fix this pc -+ 1 bug
					m->pc = m->frame_memory[m->frames-1].rip - 1;
					m->frame_memory[m->frames-1] = (CallFrame) {0};
					m->frames--;
				} else {
					m->is_running = false;

					printf("RET (frames, ip): %lu, %lu\n",
							m->frames,
							m->frame_memory[m->frames].rip
					);
				}
				break;

			case I_NOP:
				break;

			default:
				fprintf(stderr,"ERROR: UNSUPPORTED INSTRUCTION");
				break;

		}
}




void execute_program(Machine* m) {
	while(m->pc < m->program_size && m->is_running) {
		execute_op(m);



#ifdef DEBUG_STACK
			printf(" $PC: dec(%zu)",m->pc);
			printf("----------------------------\n");
			print_regs(m->dregisters,m->fregisters);
			printf("----------------------------\n");
#endif

	}
}

#ifdef OLD_EXAMPLES
void old_example(void) {
	// code version
	//
	// 	i 		  		= 0
	// 	$const one 		= 1
	// 	max 		  	= 10
	// 	jmp		  		= $loop
	//
	// loop:	
	// 	print(i)
	// 	i = i + one
	//	if (i != 10) goto loop;
	//
	// 	halt
	Instruction simple_loop[] = {
		SET			(	0,		0			),
		SET			(	1,		1			),
		SET			(	2,		10			),
		SET 		(	3,		4			),
	
		REG_DUMP	(	0, 					),
		ADD			(	0,		0,		1	),
		NEJMP		(   3,		0,		2	),
	
		HALT		()
	};

	Instruction and_or[] = {
		SET			(	1,		0			),
		SET			(	2,		1			),

		OR 			(	0,		1,		2	),
		REG_DUMP	(	0,	),
		AND 		(	0,		1,		2	),
		REG_DUMP	(	0,	),
		
		HALT		(),
	};

	Instruction inc_dec[] = {
		INC			(	0	),
		INC			(	0	),
		INC			(	0	),
		INC			(	0	),
		REG_DUMP	(	0	),
		DEC			(	0	),
		DEC			(	0	),
		REG_DUMP	(	0	),
		HALT		()
	};

	Instruction save_test[] = {
		SET			(0, 1024), 	// value
		SET			(1, 512	), 	// address
		DSAVE		(0,	1	),

		DLOAD		(0,	1	), 	// load
		REG_DUMP    (0),		// print
		HALT		(),
	};

	Instruction print[] = {
		SET			(0,	0),
		SET			(1,	0x0A6C6F6C), // \n l o l
		DSAVE 		(1,	0),

		SET			(0, 1), // $r0 = stream
		SET			(1, 0), // $r1 = buffer addr
		SET 		(2, 4), // $r2 = len
		
		PRINT		()	  , // print $r3, $r1, $r2,
		// print  (stream, *buffer, len)
		HALT		()
	};


	Instruction print_o[] = {
		SET			(0,	0),
		SET			(1,	0x0A6C6F6C), // \n l o l
		DSAVE 		(1,	0),

		SET			(0, 1), // $r0 = stream
		SET			(1, 0), // $r1 = buffer addr
		SET 		(2, 4), // $r2 = len
		
		PRINT		()	  , // print $r3, $r1, $r2,
		// print  (stream, *buffer, len)
		HALT		()
	};

	Instruction recursion[] = {
		// main:
		SET(0,44), 	// 0
		SET(1,22), 	// 1
		REC(),     	// 2
		PUSH(0),	// 3
		PUSH(1),	// 4
		CALL(7),	// 5
		RET(),
		
		// add:
		POP(0), 	// 8
		POP(1),
		ADD(0,0,1),
		REG_DUMP(0),

		SET(1,10),
		REC(),
		PUSH(0),
		PUSH(1),
		CALL(7),

		RET(0),
	};


	Instruction function_call[] = {
		// main:
		SET(0,44), 	// 0
		SET(1,22), 	// 1
		REC(),     	// 2
		PUSH(0),	// 3
		PUSH(1),	// 4
		CALL(7),	// 5
		RET(),
		
		// add:
		POP(0), 	// 8
		POP(1),
		ADD(0,0,1),
		REG_DUMP(0),
		RET(0),
	};
}
#endif




int main(int argc, char** argv) {

#ifdef BRUTE_FORCE_PROGRAM

	Instruction program[] = {
		// main:
		SET(0,44), 	// 0
		SET(1,22), 	// 1
		REC(),     	// 2
		PUSH(0),	// 3
		PUSH(1),	// 4
		CALL(7),	// 5
		RET(),		// 6
		
		// add:
		POP(0), 	// 7
		POP(1), 	// 8
		ADD(0,0,1),	// 9
		REG_DUMP(0),// 10
		REC(),		// 11
		SET(1,10),	// 12
		PUSH(0),	// 13
		PUSH(0),	// 14
		CALL(17),	// 15
		RET(0),		// 16

		// sub
		POP(0), 	// 17
		POP(1),		// 18
		ADD(0,0,1),	// 19
		REG_DUMP(0),// 20
		RET(0),		// 21
	};

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


	execute_program(&m);

#ifdef DEBUG
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
