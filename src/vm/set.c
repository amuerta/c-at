#include "set.h"
#include "../assembler/disassembler.c"
#include "../../lib/cliargs.h"


// TODO:
// unsinged value specific operations
// scmp singned cmp
// ucmp unsigned cmp
// fcmp float cmp

#define DEBUG

uint 	D_REGISTERS[INT_REG_COUNT];
float 	F_REGISTERS[FLT_REG_COUNT];

byte STACK[STACK_SIZE];


Machine init_machine(Instruction *p, size_t pl) {
	return (Machine) {
		.pc = 0,
		.program = p,
		.program_size = pl,
		.iregisters = D_REGISTERS,
		.fregisters = F_REGISTERS,
		.stack_memory = (USE_STACK)? 
			(STACK) : (NULL), 		// TODO
		.frame_memory = {0},
		.heap_address_space = NULL,	// TODO

		.is_running = 1	,
		.core_logger = NULL
	};
}

Instruction inst_next(Machine *m) {
	return m->program[m->pc++];
}


void log_regs(FILE* log,uint* iregisters, float* fregisters) {
	assert(FLT_REG_COUNT == INT_REG_COUNT && "BROKEN ASSUMPTION HERE");
	for(uint i = 0; i < FLT_REG_COUNT; i++)
		fprintf(log,
				"\t | $dr%d: %d,\t  $fr%d: %f\n",
				i,iregisters[i],
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
			size_t jmp_addr) 
{
	cframe[(*frames)] = (CallFrame) {
		.rip = (*pc),
		.lvp = addr,
	};
	(*pc) = jmp_addr;
	(*frames)++;
}

void execute_op(Machine *m) {
		Instruction inst = inst_next(m);
		
		uint* 	d_reg = m->iregisters;
		float* 	f_reg = m->fregisters;

		byte op 	= 	inst.operation	;
		byte r0		= 	inst.argument_reg[0];
		byte r1		=	inst.argument_reg[1];
		byte r2		=	inst.argument_reg[2];

		UniformStrg immd 	=   inst.storage;
		/* uint immd 	=   inst.storage; */

		// CRASH CHECK
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
				if (m->core_logger) {
					fprintf(
							m->core_logger,
							"CALL (frames, ip): %lu, %lu\n",
							m->frames,
							m->pc
						   );
				}
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
			
			// Casts
			
			case I_CU2I	:
				d_reg[r0] = (int)  d_reg[r1];
				break;

			case I_CI2U	:
				d_reg[r0] = (uint) d_reg[r1];
				break;

			case I_CF2I	:
				d_reg[r0] = (int)  f_reg[r1];
				break;

			case I_CI2F	:
				f_reg[r0] = (float)d_reg[r1];
				break;

			case I_CU2F	:
				f_reg[r0] = (float)d_reg[r1];
				break;

			case I_CF2U	:
				d_reg[r0] = (uint) f_reg[r1];
				break;


			// Word (u32 / i32) register comparisons

			case I_CMPE: 
				d_reg[r0] = (d_reg[r1] == d_reg[r2]);
				break; 

			case I_CMPEG: 
				d_reg[r0] = (d_reg[r1] >= d_reg[r2]);
				break; 

			case I_CMPEL: 
				d_reg[r0] = (d_reg[r1] <= d_reg[r2]);
				break; 

			case I_CMPG: 
				d_reg[r0] = (d_reg[r1] >  d_reg[r2]);
				break; 

			case I_CMPL: 
				d_reg[r0] = (d_reg[r1] <  d_reg[r2]);
				break;

			// Floating point register comparisons

			case I_FCMPE: 
				d_reg[r0] = (f_reg[r1] == f_reg[r2]);
				break; 

			case I_FCMPEG: 
				d_reg[r0] = (f_reg[r1] >= f_reg[r2]);
				break; 

			case I_FCMPEL: 
				d_reg[r0] = (f_reg[r1] <= f_reg[r2]);
				break; 

			case I_FCMPG: 
				d_reg[r0] = (f_reg[r1] >  f_reg[r2]);
				break; 

			case I_FCMPL: 
				d_reg[r0] = (f_reg[r1] <  f_reg[r2]);
				break;

			// Constant-defined Jumps 
			// use literall address to jump to.

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

			// Register Jumps
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
				{
					if (m->core_logger) {
						fprintf(
							m->core_logger,
							"WRITE(s:%d,b: ptr(%d),l:%d)\n",
							d_reg[0],d_reg[1],d_reg[2]
						);
					}
					int r = write(d_reg[0],&STACK[d_reg[1]],d_reg[2]);
					assert(r && "ERROR PERFORMING WRITE");
				}			
				break;

			case I_REG_DUMP:
				if (m->core_logger) {
					fprintf(
						m->core_logger,
						"REGDUMP (register %d): \t%d\n",
						r0,
						d_reg[r0]
					);
				}
				break;

			case I_RET:
				m->accum_register = d_reg[r0];
				if (m->frames) {
					if (m->core_logger) {
						fprintf(
							m->core_logger,
							"RET (frames, ip): %lu, %lu\n",
							m->frames,
							m->frame_memory[m->frames-1].rip
						);
					}
					// TODO: fix this pc -+ 1 bug
					m->pc = m->frame_memory[m->frames-1].rip - 1;
					m->frame_memory[m->frames-1] = (CallFrame) {0};
					m->frames--;
				} else {
					m->is_running = false;
					if (m->core_logger) {
						fprintf(
							m->core_logger,
							"ROOTRET (frames, ip): %lu, %lu\n",
							m->frames,
							m->frame_memory[m->frames].rip
						); 
					}
				}
				break;

			case I_NOP:
				break;

			default:
				fprintf(stderr,"ERROR: UNSUPPORTED INSTRUCTION");
				break;

		}
}


time_ms_t time_in_microsec(void) {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

void execute_program(Machine* m) {
	while(m->pc < m->program_size && m->is_running) {
		execute_op(m);



#ifdef DEBUG_STACK
#endif

	}
}

#ifdef OLD_EXAMPLES
void old_example(void) {
}
#endif

void log_debug_registers(FILE* logger, Machine* m) {
	fprintf (logger," $PC: dec(%zu)",m->pc				);
	fprintf (logger,"----------------------------\n"	);
	log_regs(logger,	m->iregisters,m->fregisters		);
	fprintf (logger,"----------------------------\n"	);
}

void log_debug_statistic(Machine m) {
	printf("\n\n");
	printf("----------- INFO --------------\n");
	printf("sizeof(Machine): %lu\n",	sizeof(Machine));
	printf("sizeof(Instruction): %lu\n",sizeof(Instruction));
	printf("sizeof(STACK): %lu\n",sizeof(byte)*STACK_SIZE);
	printf("sizeof(CALLFRAME): %lu\n",sizeof(CallFrame)*FRAME_COUNT);
	printf("-------------------------------\n");
	printf(FMT_PADDING);
	
}


