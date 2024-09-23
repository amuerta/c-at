#include <stdio.h>
#include <stdlib.h>


#include <assert.h>
#include <string.h>
#include <stdbool.h>


#define HCH_ARGS_IMPLEMENTATION

#include "../lib/cliargs.h"

#include "tokenizer.c"
#include "set.h"
#include "disassembler.c"

#define ERR_FEW_OPERANDS "\nERROR: not enough operands for instruction"

#define FULL_DEBUG_VIEW

char* parse_args(char** args, size_t argc, char* desired_arg, char* get_value);

const char* KEYWORDS[] = {
	"halt"	, "entry",
	"dload"	, "fload"	, 
	"dsave"	, "fsave"	,
	"set"	, "fset"	,
	"push"	, "pop"		,
	"rec"	, "call"	,
	"getac" , //"fgetac"  TODO: fgetac	,
	"inc"	, "dec"		, 
	"add"	, "sub"		, "div"		, "mul"	, 
	"fadd "	, "fsub"	, "fdiv"	, "fmul", 
	"and"	, "or"		, 
	"cu2i"	, "ci2u"	,
	"cf2i"	, "ci2f"	,
	"cu2f"	, "cf2u"	,
	"cmpe"	,
	"cmpeg"	, "cmpel"	,
	"cmpg"	, "cmpl"	,
	"fcmpe"	,
	"fcmpeg", "fcmpel"	,
	"fcmpg"	, "fcmpl"	,
	"jmp"	, "ejmp"	, "nejmp"	, 
	"rjmp"	, "erjmp"	, "nerjmp"	, 
	"print"	, "regdump",
	"ret"	,
	"nop"	,
};

#define KEYWORDS_COUNT arrlen(KEYWORDS)

typedef struct {
	byte is_reg_n;
	byte reg_n;
	byte is_value;
	byte is_float;
	byte is_label;
	byte error_id;
	union { float f32; uint dec; } value_literall;
	char* value_label;
} Reg;

typedef struct {
	char* 	name;
	uint 	address;
} Label;

int is_keyword(char* keyword) {
	for (uint i = 0; i < KEYWORDS_COUNT; i++) 
		if (strcmp(KEYWORDS[i],keyword)==0)
			return i;
	return NONE;
}

byte is_decimal(char* str) {
	for(uint i = 0; i < strlen(str); i++)
		if (!(str[i] >= '0' && str[i] <= '9') && str[i] != '-')
			return 0;
	return 1;
}

byte is_float(char* str) {
	for(uint i = 0; i < strlen(str); i++)
		if (!((str[i] >= '0' && str[i] <= '9') && str[i] != '-' && str[i] != '.'))
			return 0;
	return 1;
}

uint get_decimal_value(char* str) {
	return atoi(str);
}

float get_float_value(char* str) {
	return atof(str);
}

Reg parse_reg(char* str) {
	Reg r;

	const uint prefix_size = 3;
	const uint number_size = 9;

	char prefix[prefix_size+1]={0}; // size + NULLTERM
	char number[number_size  ]={0};

	// should be at least "r1": len("r1") >= 2
	if (strlen(str)<2) {
		r.error_id = -1;
	} else if (strlen(str) >= prefix_size) {
		r.error_id = -2;
	}

	for(uint i = 0; i < strlen(str); i++)
		prefix[i]=str[i];

	if (strcmp(prefix,"fr")==0) {
		r.is_float = 1;
		r.is_reg_n = 1;
		for(uint c = 2; c < strlen(str); c++)
			number[c-2] = str[c];
	} else if (str[0]=='r') {
		r.is_reg_n = 1;
		r.is_float = 0;
		r.is_value = 0;
		for(uint c = 1; c < strlen(str); c++)
			number[c-1] = str[c];
	}
	else if (str[0]=='L') {
		r.is_reg_n = 0;
		r.is_float = 0;
		r.is_label = 1;
		r.value_label = str;
		assert(strlen(str)>1 && "You trying to reference a label? Label has to have at least one character in a name!");
	}
	else {
		r.is_reg_n = 0;
		r.is_value = 1;
	
		if (is_decimal(str)) {
			r.is_float = 0;
			r.value_literall.dec = get_decimal_value(str);
			//printf("\t\tDEBUG: get_decimal_value(str) -> %d\n",get_decimal_value(str));
		}

		else if (is_float(str)) { 
			r.is_float = 1;
			r.value_literall.f32 = get_float_value(str);
			//printf("\t\tDEBUG: get_float_value(str) -> %f\n",get_float_value(str));
		}

		else  {
			printf("\t edgecase\t");
		}
	}

	r.reg_n = atoi(number); printf("\tRFVL: %d%d%d%d",
			r.is_reg_n,
			r.is_float,
			r.is_value,
			r.is_label
		);
	return r;
}


void print_labels(Label* l, size_t len) {
	for(uint i = 0; i < len; i++) 
		printf("\t label '%s' \t@address: [ %d ] \n",l[i].name,l[i].address);
}

size_t get_instuction_count (Inst i) {
	size_t ic = 0;
	switch (i) {

		case I_HALT:
		case I_ENTRY:
		case I_REC:
			ic = 0;
			break;

		case I_INC:
		case I_DEC:
		case I_REG_DUMP:
		case I_JMP:
		case I_RET:
		case I_POP:
		case I_PUSH:
		case I_CALL:
		case I_GETAC:
			ic = 1;
			break;

		case I_SET:
		case I_FSET:
		case I_CU2I:
		case I_CI2U:
		case I_CF2I:
		case I_CI2F:
		case I_CU2F:
		case I_CF2U:
			ic = 2;
			break;


		case I_ADD:
		case I_FADD:

		case I_SUB:
		case I_FSUB:

		case I_MUL:
		case I_FMUL:

		case I_DIV:
		case I_FDIV:
		
		case I_AND:
		case I_OR:

		case I_EJMP:
		case I_NEJMP:
		case I_ERJMP:
		case I_NERJMP:

		case I_CMPG:
		case I_CMPL:
		case I_CMPEG:
		case I_CMPEL:
		case I_CMPE:

		case I_FCMPG:
		case I_FCMPL:
		case I_FCMPEG:
		case I_FCMPEL:
		case I_FCMPE:
			ic = 3;
			break;

		default: break;
	}
	return ic;
}

bool is_float_instr(Inst type) {
	switch(type) {
		case I_FADD:
		case I_FSUB:
		case I_FMUL:
		case I_FDIV:
		case I_FSET:
		case I_FLOAD:
		case I_FSAVE:
			return true;
			break;
		default: 
			break;
	}
	return false;
}


Instruction assemble
	(Inst 	i, 
	 uint 	arg0, uint 	arg1,
	 uint 	arg2, uint 	arg3,
	 float 	fval, uint 	dval) 
{

	switch(i) {
		case I_SET:
			return SET(arg0,dval);
			break;

		case I_FSET:
			printf("DEBUG FSET: %f\n",fval);
			return FSET(arg0,fval);
			break;

		case I_PUSH:
			return PUSH(arg0);
			break;

		case I_POP:
			return POP(arg0);
			break;

		case I_REC:
			return REC();
			break;

		case I_CALL:
			return CALL(dval);
			break;

		case I_GETAC:
			return GETAC(arg0);
			break;

		case I_DLOAD:
			return DLOAD(arg0,dval);
			break;

		case I_FLOAD:
			return FLOAD(arg0,dval);
			break;

		case I_DSAVE:
			return DSAVE(arg0,dval);
			break;

		case I_FSAVE:
			return FSAVE(arg0,dval);
			break;

		case I_INC: 
			return INC(arg0);
			break;

		case I_DEC: 
			return DEC(arg0);
			break;

		case I_ADD:
			return ADD(arg0,arg1,arg2);
			break;
	
		case I_SUB:
			return SUB(arg0,arg1,arg2);
			break;

		case I_DIV:
			return DIV(arg0,arg1,arg2);
			break;

		case I_MUL:
			return MUL(arg0,arg1,arg2);
			break;

		case I_FADD:
			return FADD(arg0,arg1,arg2);
			break;
	
		case I_FSUB:
			return FSUB(arg0,arg1,arg2);
			break;

		case I_FDIV:
			return FDIV(arg0,arg1,arg2);
			break;

		case I_FMUL:
			return FMUL(arg0,arg1,arg2);
			break;

		case I_AND:
			return AND(arg0,arg1,arg2);
			break;

		case I_OR:
			return OR(arg0,arg1,arg2);
			break;

		case I_CU2I	:
			return CU2I(arg0,arg1);
			break;

		case I_CI2U	:
			return CI2U(arg0,arg1);
			break;

		case I_CF2I	:
			return CF2I(arg0,arg1);
			break;

		case I_CI2F	:
			return CI2F(arg0,arg1);
			break;

		case I_CU2F	:
			return CU2F(arg0,arg1);
			break;

		case I_CF2U	:
			return CF2U(arg0,arg1);
			break;

		case I_CMPG:
			return CMPG(arg0,arg1,arg2);
			break;

		case I_CMPL:
			return CMPL(arg0,arg1,arg2);
			break;

		case I_CMPEG:
			return CMPEG(arg0,arg1,arg2);
			break;

		case I_CMPEL:
			return CMPEL(arg0,arg1,arg2);
			break;

		case I_CMPE:
			return CMPE(arg0,arg1,arg2);
			break;

		case I_FCMPG:
			return FCMPG(arg0,arg1,arg2);
			break;

		case I_FCMPL:
			return FCMPL(arg0,arg1,arg2);
			break;

		case I_FCMPEG:
			return FCMPEG(arg0,arg1,arg2);
			break;

		case I_FCMPEL:
			return FCMPEL(arg0,arg1,arg2);
			break;

		case I_FCMPE:
			return FCMPE(arg0,arg1,arg2);
			break;

		case I_RJMP:
			return RJMP(dval); 
			break;

		case I_ERJMP:
			return ERJMP(arg0,arg1,arg2);
			break;

		case I_NERJMP:
			return NERJMP(arg0,arg1,arg2);
			break;

		case I_JMP:
			return JMP(dval);
			break;

		case I_EJMP:
			return EJMP(dval,arg1,arg2);
			break;

		case I_NEJMP:
			return NEJMP(dval,arg1,arg2);
			break;

		case I_PRINT:
			return PRINT();
			break;

		case I_REG_DUMP: 
			return REG_DUMP(arg0);
			break;

		case I_HALT:
			return HALT();
			break;

		case I_ENTRY: 
			// TODO;
			return HALT();
			break;

		case I_RET:
			return RET(arg0);
			break;

		case I_NOP: 
			break;
		
		default: 
			printf("ILL INSTRUCTION CODE: %d\n\n", (uint)i);
			assert(0 && "ERROR: INVALID INSTRUCTION!"
					"PERHAPS YOU FORGOT TO IMPLEMENT IT SOMEWHERE?");
			break;
	}
	// unreachable
	// to remove warning
	return HALT();
}

uint find_label(Reg r,Label* labels,size_t label_count,byte* error) 
{
	assert(strlen(r.value_label) >= 2 && "LABEL HAS TO BE AT LEAST 1 char");
	// skip indentifier L from label name
	char* label = r.value_label;
	printf("label : %s\n",label);
	label++; 

	for(uint i = 0; i < label_count; i++)
		if (strcmp(labels[i].name,label)==0)  {
			return labels[i].address;
		}
	(*error) = -1;
	return (uint)0;
}

Instruction assemble_line(	Token* inst_tk, 
							size_t count, 
							Label* labels, 
							size_t labels_count)
{
	Instruction inst = {};
	size_t ic   = 0;
	int type = 0;

	if (!inst_tk[0].literall)
		assert(0 && "ERROR: instruction token value cannot be zero");

	type = is_keyword(inst_tk[0].literall);
	
	if (type<0)
		assert(0 && "ERROR: FIRST SYMBOL SHOULD BE A AN INSTRUCTION NAME.\n");


	ic = get_instuction_count((Inst)type);

	uint regs[4] = {0};
	float fvalue = 0;
	uint  dvalue = 0;

	for(uint t = 0 ; t<count; t++) {
	
		char* current_word = inst_tk[t].literall;

		if(t == 0 && type>=0) {
			inst.operation = (Inst)type;
			#ifdef FULL_DEBUG_VIEW
				printf("\n\t%s",inst_tk[t].literall);
			#endif
		} 
		
		else {

			// error handling
			if (ic+1 < count)
				printf("ERROR: LINE HAS NOT ENOUGH PARAMETRS\n");
			else if (ic+1 > count)
				printf("ERROR: LINE HAS MORE ARGUMENTS THEN NEEDED\n");
			

			else {
				Reg r = parse_reg(current_word);
				#ifdef FULL_DEBUG_VIEW	
					printf("\t%s",current_word);
				#endif

				if (r.is_value && (r.is_float 
					|| is_float_instr((Inst)type))) 
				{
					fvalue = (float)r.value_literall.dec; // unions are werid
					#ifdef FULL_DEBUG_VIEW
						printf("$f(%f)",(float)r.value_literall.dec);
					#endif
				}
	
				else if (r.is_value && !r.is_float) {
					dvalue = r.value_literall.dec;
					#ifdef FULL_DEBUG_VIEW
						printf("$d(%d)",r.value_literall.dec);
					#endif
				}

				else if (r.is_label && !r.is_value && !r.is_reg_n) {
					byte error = 0;
					dvalue = find_label(r,labels,labels_count,&error);
					#ifdef FULL_DEBUG_VIEW
						printf("$LABEL(%d)",dvalue);
					#endif
					if (error) 
						assert(0 && "Label was not found!");
				}
				
				else {
					regs[t-1] = r.reg_n;
					#ifdef FULL_DEBUG_VIEW
						printf("(%d)",r.reg_n);
					#endif
				}
			}
		}
	}

	inst = assemble((Inst) type,
					regs[0],regs[1],
					regs[2],regs[3],
					fvalue, dvalue		
					);
	return inst;
}


void index_labels(	Label* labels,
					Token* list,
					size_t list_size, 
					size_t* label_count) 
{
	size_t lc = 0;
	size_t ic = 0;
	Token tk[32] = {0};

	bool line_is_comment = false;

	for (uint i = 0; i < list_size; i++ ) {

		bool is_line_end = list[i].type == TOKEN_SYMBOL_EXPRESSION_END;
		bool is_hash_symbol = list[i].type == TOKEN_SYMBOL_HASH;
		
		if (line_is_comment) {
			if (is_line_end) {
				line_is_comment = false;
			}
			else continue;
		} else if (is_hash_symbol) {
			line_is_comment = true;
			continue;
		}

		if (!is_line_end) 
		{
			if (!line_is_comment) {
				tk[ic] = list[i];
				ic++;
			} else continue;
		} 
		else 
		{

			bool second_is_colon = tk[1].type == TOKEN_SYMBOL_COLON;
			bool first_is_word =  tk[0].type == TOKEN_WORD;
			
			if (first_is_word && second_is_colon && ic != 0)
			{
				// its probably an instuctions:
				// halt 
				// nop
				if (ic == 1)
					continue;

				labels[*label_count] = (Label) {
					.name = tk[0].literall,
					.address = lc,
				};
				(*label_count)++;
			}

			// not a comment, not empty, not a label already
			if (!line_is_comment && ic != 0 && !second_is_colon)
				lc++;

			ic = 0;
			// reset
			for(uint s = 0; s < ic; s++)
				tk[s] = (Token) {.type=(TokenType)0};
		}
	}

}

Instruction* 
	assemble_program(	Token*  list		, 
						size_t  list_size	,
						size_t* program_size,
						Label*  labels		,
						size_t  label_count	) 
{

	size_t lc = 0;
	size_t ic = 0;
	Token tk[32] = {0};

	Instruction* program = calloc(128,sizeof(Instruction));
	bool line_is_comment = false;

	for (uint i = 0; i < list_size; i++ ) {

		bool is_line_end = list[i].type == TOKEN_SYMBOL_EXPRESSION_END;
		bool is_hash_symbol = list[i].type == TOKEN_SYMBOL_HASH;
		

		if (line_is_comment) {
			if (is_line_end) {
				line_is_comment = false;
			}
			else continue;
		} else if (is_hash_symbol) {
			line_is_comment = true;
			continue;
		}


		if (!is_line_end) 
		{
			if (!line_is_comment) {
				tk[ic] = list[i];
				ic++;
			} 
		} 
		else 
		{
			/* for(uint s = 0; s < ic; s++) */
			/* 	token_print(tk[s]); */
			/* printf("\n"); */


			bool second_is_colon = tk[1].type == TOKEN_SYMBOL_COLON;
			bool first_is_word =  tk[0].type == TOKEN_WORD;
			
			if (first_is_word && second_is_colon && ic != 0)
			{
				ic = 0;
				continue;
			}

			if (ic!=0) { // if line is not empty
				program[lc] = assemble_line(tk,ic,labels,label_count);
				lc++;
			}
	
			for(uint s = 0; s < ic; s++)
				tk[s] = (Token) {.type=(TokenType)0};
			ic = 0;
		}
	}

	(*program_size) = lc;
	return program;
}

#define MAX_SIZE 2048

int main(int argc, char** argv) {

	// TODO: MAKE BUFFER HAVE UNLIMITED COUNT;
	char fb[MAX_SIZE] = {0};
	FILE *f ;
	// TODO: MAKE LABELS HAVE UNLIMITED COUNT;
	Label labels[128] = {0}; 



	// CLI PROCESSING:
	Args a = args_init(argc,argv);

	int   args_out_flag = arg_find 			(a,"-o");
	char* args_out_path = arg_find_value 	(a,"-o");

	if (argc < 2) {
		printf("USAGE:\n"
				"\t caasm ./c_at_assembly_file.caasm"
		);
		return -1;
	}

	if (args_out_flag && !args_out_path) {
		printf("[WARN]:\t"  
				"out path flag found, "
				"but not argument given"
		);
		args_out_path = "./out.caic";
	}
	else if ( !(args_out_flag && args_out_path)) {
		args_out_path = "./out.caic";
	}

	f = fopen(argv[1],"r");

	if (!f) {
		printf("Failed to open file:\t"
				"\"%s\"",
				argv[1]);
		exit(-1);
	}


	char c = 0;
	for(uint fsize = 0; (c=fgetc(f))!=EOF; fsize++) {
		sprintf(fb,"%s%c",fb,c);
		assert(fsize < MAX_SIZE && "EXCEEDED MAXIMUM FILE SIZE");
	}
	
#ifdef FULL_DEBUG_VIEW
	printf("FILE: \n-------------\n"
						 "%s"
				 "\n-------------",fb);
#endif

	Token t;
	size_t file_pos = 0;

	size_t token_list_len = 0;
	size_t token_list_pos = 0;
	Token* list = reallocarray(NULL,TOKENS_BLOCK_SIZE,sizeof(t));

	
	while((t = token_next(fb,strlen(fb),&file_pos)).type != TOKEN_EOB) {
		if (t.type != TOKEN_SPACE)
			tokens_list_append(list,
							   &token_list_len,
							   &token_list_pos,
							   t
			);
	}

#ifdef FULL_DEBUG_VIEW
	printf("\n\n\n\nPARSED: ");
	printf("\n--------------\n");
	
	for(uint i = 0; i < token_list_pos; i++)
		token_print(list[i]);
	
	printf("\n--------------\n");
#endif

	size_t 		ps 		= 0; // program size
	size_t 		lc		= 0; // labels count
	
	// first pass
	index_labels(labels,list,token_list_pos,&lc);

#ifdef FULL_DEBUG_VIEW
	printf("labels_count = %d\n",lc);
	print_labels(labels,lc);
#endif

	// second pass
	Instruction *program = assemble_program(list,
											token_list_pos,
											&ps,
											labels,
											lc			);

#ifdef FULL_DEBUG_VIEW
	printf("\n\n\n\nINST SET GENERATED : ");
	printf("\n--------------");
		print_program_asm(program,ps);
	printf("\n--------------\n");
#endif


	int result = write_to_file(args_out_path,program,ps);

	if (result) {
		printf("Sucessuly written %zu bytes into \"%s\"\n",
				sizeof(Instruction) * ps, args_out_path);
	}

	free(program);
	free_tokenlist(list,token_list_pos);
	if (f)
		fclose(f);

}

