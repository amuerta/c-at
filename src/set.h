#pragma once
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>


#define USE_STACK 1
/* #define DEBUG */
//#define DEBUG_STACK


#ifndef USE_STACK
	assert(false && "TODO: IMPLEMENT MBLK | MMAP BACKEND")
#endif

#define FMT_PADDING "\n\n\n\n"

#define INT_REG_COUNT 		16
#define FLT_REG_COUNT 		16

#define FRAME_COUNT		128
#define STACK_SIZE 	   	16384
#define BASE_HEAP_SIZE 	65535

typedef char byte;
typedef unsigned long long time_ms_t;

typedef enum {
	I_HALT 	,
				// stops program execution

	I_ENTRY ,   // endicates entry
				// used to endicate end of the 
				// header block in byte code file
	
	I_DLOAD , 
	I_FLOAD , 
				// loads data pointed by reg1 to reg0
	I_DSAVE	, 
	I_FSAVE	, 
				// saves data pointed by reg0 to stack memory 
				// at address in reg[1]
	/* I_DCSAVE,  */ // TODO:?
	/* I_FCSAVE,  */
	/* I_PCSAVE,  */

	/* I_SAVEBL, */
	/* I_SAVEBR, */
				// saves a byte of data to left and right of 
				// the rigister
				// BL [XX __ __ __] : [__ __ __ XX] BR
	/* I_SAVEDL, */
	/* I_SAVEDR, */ // TODO:?
				// saves a DB (double byte) of data to left 
				// and right of the rigister
				// DL [XX XX __ __] : [__ __ XX XX] DR

	I_SET  	, 
	I_FSET	,
				// sets  data inside register reg0 with value in reg1
	
	I_PUSH 	,
	I_POP	,
				// TODO: IMPLEMENT MORE PUSH/POP VARIATIONS
				// pushes data onto a stack from register r0
				// pops data from a stack to a register r0
	I_REC	,
	I_CALL	,
				// rec - stards recording data for new frame
				//
				// call - uses data from begining of a record to 
				// perform a procedure call, creating a new call 
				// frame.
	I_GETAC	, 	
				// getac - copies value from reg0 into special
				// accumulator register, used as return 
	I_INC	,
	I_DEC	,
				// increment or decrement register 'reg0' by 1
	I_ADD 	, 
	I_SUB 	, 
	I_DIV	,
	I_MUL	,		// decimal register opcodes
					
	I_FADD 	, 
	I_FSUB 	, 
	I_FDIV	,
	I_FMUL	,		// floating register opcodes
					
				// adds,subtracts,mutliplies,divides data 
				// inside register reg1 and reg2 and stores it in reg0
	I_AND	,
	I_OR	,

	// CAST
	I_CU2I	,
	I_CI2U	,
	I_CF2I	,
	I_CI2F	,
	I_CU2F	,
	I_CF2U	,
				// C - cast
				// I - integer 	(32 bit)
				// F - float 	(32 bit)
				// U - Unsigned (32 bit)
				// 2 - to
				//
				// CI2F = cast int to float

	// COMPARISONS

	I_CMPE 	, 
	I_CMPEG , 
	I_CMPEL , 
	I_CMPG 	, 
	I_CMPL	,

	I_FCMPE , 
	I_FCMPEG, 
	I_FCMPEL, 
	I_FCMPG , 
	I_FCMPL	,
				// CMP - compare
				// E - equal
				// L - less than
				// G - greater than
				// F - float
				//
				// example: CMPEL - compare if equal or less than
				//
				// all instructions take a word register index
				// as first argument, and second and third 
				// register indexes as comparison subjects
				//
				// arguments have higher precedence depending
				// if they are first, it means that if we doing 
				// cmpg comparison between reg2 and reg3, 
				// it will be performed read as:
				// 
				// "is reg2 greater than reg3"
				// 
				// 'F' before CMP means the comparison will be done
				// on float registers. However, result in reg0 will
				// be written to word register anyways


	// Logic for constant address 
	// conditional jumps
	I_JMP  	,
	I_EJMP 	,
	I_NEJMP ,
				// JMP - jump to address literall dval
				// 
				// EJMP - jump to address literall dval
				// if values in reg0 and reg1 are equal
				//
				// NEJMP - jump to address literall dval
				// if values in reg0 and reg0 are NOT equal

				// Logic for regiseter stored
				// conditional jumps
	I_RJMP  	,
	I_ERJMP 	,
	I_NERJMP ,
				// RJMP - jump to address stored in reg0
				// ERJMP - jump to address in reg0 
				// 		if reg1 and reg2 are equal
				//
				// NERJMP - jump to address in reg0 
				// 		if reg1 and reg2 are NOT equal
	I_PRINT	, 
				// print reg1 of bytes from stack address 
				// in reg0 from stream defineddefined inn reg2
	I_REG_DUMP,
				// dump register value in reg0
	I_RET		,
				// puts whatever  you saved
				// at r1 into acumm register
				// which indicates single return value
	I_NOP	,
} Inst;

typedef union {
	byte			as_i8;
	short			as_i16;
	int				as_i32;
	float			as_f32;
	uint			as_ptr;
	double  		as_f64;
} UniformStrg;

typedef struct {
	Inst			operation		;
	byte			argument_reg[3]	;
	UniformStrg 	storage			;
} Instruction;

// VM has stack memory separated from call stack
// despite both being used together.
//
// CallFrame stores information on where program has to return 
// to procced correct program execution.
//
// Frame memory is an array of frames where top most call frame
// is currently run procedure.
//
// All the variables are located on the stack, and Frame memory 
// contains address to place in stack where are both 
// procedure input and data values are stored

// Upon performing a call, what was put on top of the stack 
// will be treated as inputs, and offseted acoordingly 
// when creating and adding new CallFrame and adding it
// into a list

// Function arguments are just regular data vm can operate on
// they are being copied to stack

/*
 
	Fadd:
		rec
		pop 	@r1
		pop		@r2
		add		@r0 @r1 @r2
		ret		@r0

	_entry:
		rec	
		push 	22
		push 	22
		call 	add

		getac 	@r0
		printnum @r0

  +------------------+
  |	 add             |			  
  |	+--------------+ |			  
  |	|    locals    | |-------+    +--------------+ 0xFF
  |	+              + |       +--->|  add locals  | 
  |	|              | |-----+      |--------------| 
  |	+--------------+ |     +----->|  add input   | 
  |	|    ret ptr   | |			  +--------------+  <----------+  
  |	+--------------+ |     +----->| main locals  |             |
  |	 main			 |     |      |--------------|             |
  |	+--------------+ |     |  +-->|  main input  |             |
  |	|    locals    | |-----+  |   +--------------+ 0x00 <------+
  |	+              + |        |                               stack base pointers
  |	|              | |--------+
  |	+--------------+ |
  |	|    ret ptr   | |
  |	+--------------+ |
  +------------------+

*/

typedef struct CallFrame {
	size_t 				rip; 		// resume/return instruction pointer
	uint  				bfp; 		// base frame pointer
	uint  				lvp; 		// local variable pointer
} CallFrame;

typedef struct {
	byte platform;
	byte major_version;
	
} MachineHeader;

typedef struct {

	// Header
	MachineHeader 	cfg;

	size_t 			pc; 				// program counter
	size_t          program_size;
	Instruction*  	program;

	// registers
	uint* 			iregisters; 			// integer
	float* 			fregisters;				// floating-point
	uint 			accum_register;			// accumulator register
	byte*			wide_accum_register;	// 2kb space
				
	// Stack and call frame
	size_t			frames;
	size_t 			stack_top;
	CallFrame 		frame_memory[FRAME_COUNT];
	byte*			stack_memory;
	byte* 			heap_address_space;

	// WM State 
	byte 			is_running;
} Machine;


// INSTRUCTION DEFINITIONS

Instruction i_fset(byte reg,float imm) {
	return (Instruction) {
		.operation = I_FSET,
		.argument_reg = {reg},
		.storage.as_f32 = imm,
	};
}

#define FSET(R,S) \
		i_fset((R),(S))

#define SET(R,S)							\
		(Instruction) {						\
			.operation = I_SET,				\
			.argument_reg = {(R)},			\
			.storage.as_i32 = (S),			\
		}

#define PUSH(...) 							\
		(Instruction) {						\
			.operation = I_PUSH,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define POP(...) 							\
		(Instruction) {						\
			.operation = I_POP,				\
			.argument_reg = {__VA_ARGS__},	\
		}

#define REC() 								\
		(Instruction) {						\
			.operation = I_REC,				\
			.argument_reg = {},				\
		}

#define CALL(ADDR) 							\
		(Instruction) {						\
			.operation = I_CALL,			\
			.storage.as_i32 = (ADDR),		\
		}

#define GETAC(REG) 							\
		(Instruction) {						\
			.operation = I_CALL,			\
			.argument_reg = { (REG) },		\
		}

#define DLOAD(...) 							\
		(Instruction) {						\
			.operation = I_DLOAD,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define FLOAD(...) 							\
		(Instruction) {						\
			.operation = I_FLOAD,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define DSAVE(...) 							\
		(Instruction) {						\
			.operation = I_DSAVE,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define FSAVE(...) 							\
		(Instruction) {						\
			.operation = I_FSAVE,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define INC(...)							\
		(Instruction) {						\
			.operation = I_INC,				\
			.argument_reg = {__VA_ARGS__},	\
		}

#define DEC(...)							\
		(Instruction) {						\
			.operation = I_DEC,				\
			.argument_reg = {__VA_ARGS__},	\
		}

#define ADD(...) \
		(Instruction) {						\
			.operation = I_ADD,				\
			.argument_reg = {__VA_ARGS__},	\
		}

#define FADD(...) \
		(Instruction) {						\
			.operation = I_FADD,			\
			.argument_reg = {__VA_ARGS__},	\
		}


#define SUB(...) \
		(Instruction) {						\
			.operation = I_SUB,				\
			.argument_reg = {__VA_ARGS__},	\
		}

#define FSUB(...) \
		(Instruction) {						\
			.operation = I_FSUB,			\
			.argument_reg = {__VA_ARGS__},	\
		}


#define DIV(...) \
		(Instruction) {						\
			.operation = I_DIV,				\
			.argument_reg = {__VA_ARGS__},	\
		}


#define FDIV(...) 							\
		(Instruction) {						\
			.operation = I_FDIV,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define MUL(...) 							\
		(Instruction) {						\
			.operation = I_MUL,				\
			.argument_reg = {__VA_ARGS__},	\
		}

#define FMUL(...) 							\
		(Instruction) {						\
			.operation = I_FMUL,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define AND(...)							\
		(Instruction) {						\
			.operation = I_AND,				\
			.argument_reg = {__VA_ARGS__},	\
		}

#define OR(...)								\
		(Instruction) {						\
			.operation = I_OR,				\
			.argument_reg = {__VA_ARGS__},	\
		}

// CAST

#define CU2I(...)							\
		(Instruction) {						\
			.operation = I_CU2I,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define CI2U(...)							\
		(Instruction) {						\
			.operation = I_CI2U,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define CF2I(...)							\
		(Instruction) {						\
			.operation = I_CF2I,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define CI2F(...)							\
		(Instruction) {						\
			.operation = I_CI2F,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define CU2F(...)							\
		(Instruction) {						\
			.operation = I_CU2F,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define CF2U(...)							\
		(Instruction) {						\
			.operation = I_CF2U,			\
			.argument_reg = {__VA_ARGS__},	\
		}

// COMPARISONS

	// word comparison
#define CMPE(...)							\
		(Instruction) {						\
			.operation = I_CMPE,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define CMPEG(...)							\
		(Instruction) {						\
			.operation = I_CMPEG,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define CMPEL(...)							\
		(Instruction) {						\
			.operation = I_CMPEL,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define CMPG(...)							\
		(Instruction) {						\
			.operation = I_CMPG,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define CMPL(...)							\
		(Instruction) {						\
			.operation = I_CMPL,			\
			.argument_reg = {__VA_ARGS__},	\
		}

	// float comparison
#define FCMPE(...)							\
		(Instruction) {						\
			.operation = I_FCMPE,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define FCMPEG(...)							\
		(Instruction) {						\
			.operation = I_FCMPEG,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define FCMPEL(...)							\
		(Instruction) {						\
			.operation = I_FCMPEL,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define FCMPG(...)							\
		(Instruction) {						\
			.operation = I_FCMPG,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define FCMPL(...)							\
		(Instruction) {						\
			.operation = I_FCMPL,			\
			.argument_reg = {__VA_ARGS__},	\
		}


// register stored address jumps

#define RJMP(r)								\
		(Instruction) {						\
			.operation = I_JMP,				\
			.argument_reg = {r},			\
		}
//.storage.as_i32 = (s) 			

#define ERJMP(...)							\
		(Instruction) {						\
			.operation = I_EJMP,			\
			.argument_reg = {__VA_ARGS__},	\
		}

#define NERJMP(...)							\
		(Instruction) {						\
			.operation = I_NEJMP,			\
			.argument_reg = {__VA_ARGS__},	\
		}

// regular jumps

#define JMP(s)								\
		(Instruction) {						\
			.operation = I_JMP,				\
			.storage.as_i32 = (s) 			\
		}
//.argument_reg = {r},			

#define EJMP(a,r0,r1)						\
		(Instruction) {						\
			.operation = I_EJMP,			\
			.argument_reg = { r0, r1 },		\
			.storage.as_i32 = (a)			\
		}

#define NEJMP(a,r0,r1)						\
		(Instruction) {						\
			.operation = I_NEJMP,			\
			.argument_reg = { r0, r1 },		\
			.storage.as_i32 = (a)			\
		}

#define REG_DUMP(...) 						\
		(Instruction) {						\
			.operation = I_REG_DUMP,		\
			.argument_reg = {__VA_ARGS__},	\
		}

/* #define PRINT(B,L,S) 						\ */
/* 		(Instruction) {						\ */
/* 			.operation = I_PRINT,			\ */
/* 			.argument_reg = {(B),(L),(S)},	\ */
/* 		} */


#define PRINT() 							\
		(Instruction) {						\
			.operation = I_PRINT,			\
			.argument_reg = {},				\
		}

#define HALT()		 						\
		(Instruction) {						\
			.operation = I_HALT,			\
		}

#define RET(...)		 					\
		(Instruction) {						\
			.operation = I_RET,				\
			.argument_reg = {__VA_ARGS__},	\
		}

#define arrlen(A) \
	sizeof( (A) )/sizeof( (A)[0] )

// NO STORAGE
#define NS 0 
