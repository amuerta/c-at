#include "tester.h"

//------------------------------------------//
//			TEST PROGRAMS BYTE CODE   		//
//------------------------------------------//

Instruction simple_loop[] = {
	SET			(	0,		0			), // 	i 		  		= 0
	SET			(	1,		1			), // 	$const one 		= 1
	SET			(	2,		10			), // 	max 		  	= 10
	SET 		(	3,		4			), // 	jmp		  		= $loop
                                           //
	REG_DUMP	(	0, 					), // loop:	
	ADD			(	0,		0,		1	), // 	print(i)
	NEJMP		(   3,		0,		2	), // 	i = i + one
                                           //	if (i != 10) goto loop;
	RET			(	0	)                         //
};                                         // 	halt


Instruction and_or[] = {
	SET			(	1,		0			),
	SET			(	2,		1			),

	OR 			(	0,		1,		2	),
	REG_DUMP	(	0,	),
	AND 		(	0,		1,		2	),
	REG_DUMP	(	0,	),

	RET			(	0	),
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
	RET			(   0 	)
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
	RET			( 1 )
};


Instruction function_call[] = {
	// main:
	SET			(0,	44), 	// 0
	SET			(1,	22), 	// 1
	REC			(),     	// 2
	PUSH		(0),	// 3
	PUSH		(1),	// 4
	CALL		(7),	// 5
	RET			(),

	// add:
	POP			(0), 	// 8
	POP			(1),
	ADD			(0,0,1),
	REG_DUMP	(0),
	RET			(0),
};

Instruction recursive_function_call[] = {
	// PSEUDO-CODE:
	// function recursive_plus_one(int n) -> int:
	// 	begin
	// 		n = n + 1
	// 		reg_print (n)
	// 		if (n != 5) then
	// 			recursive_plus_one(n)
	// 		else 
	// 			return n
	// 		endif
	// 	end
	// 			
	//
	// function _entry() -> int:
	// 	begin
	// 		n = 0
	// 		recursive_plus_one(n)
	// 		return n
	// 	end
	SET			(0,0),		
	REC			(),
	PUSH		(0),
	CALL		(5),
	RET			(0),

	POP			(),
	SET			(1,1),
	SET			(2,5),
	ADD			(0,0,1),
	REG_DUMP	(0),
	NEJMP		(12,0,2),
	RET			(0),

	REC			(),
	PUSH		(0),
	CALL		(5),
	RET			(0)
};

#define TEST_UNIT(PROG,RET) \
	{ (RET), getname(PROG), (PROG), arrlen((PROG)) }

TestProgram RAW_TESTS_LIST[] = 
{
	TEST_UNIT(	simple_loop	, 	10	),
	TEST_UNIT(	and_or, 		0	),
	TEST_UNIT(	inc_dec, 		2	),
	TEST_UNIT(	print, 			0	),
	TEST_UNIT(	function_call, 	66	),
	TEST_UNIT(	recursive_function_call, 	5	)
};

const size_t RAW_TESTS_LIST_LEN = arrlen(RAW_TESTS_LIST);
