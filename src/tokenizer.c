
typedef enum {
	TOKEN_BINARY_OPERATOR_PLUS,
	TOKEN_BINARY_OPERATOR_PLUSPLUS,
	TOKEN_BINARY_OPERATOR_MINUS,
	TOKEN_BINARY_OPERATOR_MINUSMINUS,
	TOKEN_BINARY_OPERATOR_MULTIPLY,
	TOKEN_BINARY_OPERATOR_DEVIDE,
	TOKEN_BINARY_OPERATOR_MOD,
	TOKEN_BINARY_OPERATOR_ASIGN,
	TOKEN_BINARY_OPERATOR_EQUAL,
	TOKEN_BINARY_OPERATOR_EQUAL_OR_LESS,
	TOKEN_BINARY_OPERATOR_EQUAL_OR_MORE,
	TOKEN_BINARY_OPERATOR_MORE,
	TOKEN_BINARY_OPERATOR_LESS,
	TOKEN_BINARY_OPERATOR_AND,
	TOKEN_BINARY_OPERATOR_OR,
	TOKEN_BINARY_OPERATOR_NOT,

	TOKEN_SYMBOL_CURLY_OPEN,
	TOKEN_SYMBOL_CURLY_CLOSE,
	TOKEN_SYMBOL_PAREN_OPEN,
	TOKEN_SYMBOL_PAREN_CLOSE,
	TOKEN_SYMBOL_SQUARE_OPEN,
	TOKEN_SYMBOL_SQUARE_CLOSE,
	TOKEN_SYMBOL_DOT,
	TOKEN_SYMBOL_COLON,
	TOKEN_SYMBOL_COMMA,
	TOKEN_SYMBOL_AT,
	TOKEN_SYMBOL_DOLLAR,
	TOKEN_SYMBOL_PERCENT,
	TOKEN_SYMBOL_QUESTION_MARK,
	TOKEN_SYMBOL_EXPRESSION_END,
	TOKEN_SYMBOL_HASH,

	TOKEN_WORD,
	TOKEN_WORD_LITERALL_NUMBER,
	TOKEN_WORD_LITERALL_STRING,

	TOKEN_NULL,
	TOKEN_SPACE,
	TOKEN_EOB,
} TokenType;

typedef struct {
	char*		literall;
	/* size_t		len; */
	TokenType   type;
} Token;

bool is_character(char c) {
	return (
		   (c >= 'A' && c <= 'Z') ||
		   (c >= 'a' && c <= 'z') 
	);
}

bool is_space(char c) {
	return (c == ' ' || c == '\t');
}

bool is_number(char c) {
	return (c >= '0' && c <= '9');
}

#define NONE -1
char next_symbol(char* buffer, size_t length, size_t pos) {
	if (length <= pos)
		return NONE;

	return buffer[pos+1];
}


bool token_is_subset   (Token* src,
						size_t inital_index, 
						size_t check_count, 
						Token* subset) 
{
	bool equal = false;
	for (uint i = 0; i < inital_index+check_count; i++)
		equal = equal && (src[inital_index+i].type == subset[i].type);
	return equal;
}


#define ERROR 0
#define DEFAULT_LITERAL_SIZE 1024
Token token_next(char* buffer,
			   size_t len, 
			   size_t *bp) 
{

	char word_buffer[DEFAULT_LITERAL_SIZE];

	Token result = {NULL,TOKEN_NULL};

	uint read_index = 0;
	int read_number_literall = false;
	int read_word 			 = false;

	if (len <= *bp) {
		result.type = TOKEN_EOB;
		return result;
	}
		

	//while(read) {
	

		char c = buffer[*bp];

		if (is_space(c))
		{
			*bp += 1;
			result.type = TOKEN_SPACE;
			return result;
		}


		/* read_number_literall = true; */
		/* if (c=='.' && next_symbol(buffer,len,*bp) == '.') */
		/* { */
		/* 	result.type = number_literall */
		/* 	result.literall = calloc(strlen(word_buffer),1); */
		/* 	strcpy(result.literall,word_buffer); */
		/* } */

		if (is_number(c)) {
			read_number_literall = true;
		}
		else if (is_character(c)) {
			read_word = true;
		}


		while (read_word)
		{
			word_buffer[read_index] = c;
			c = next_symbol(buffer,len,*bp+read_index);
			read_index++;
			if (!is_number(c) && !is_character(c)) 
			{
				result.type = TOKEN_WORD;
				result.literall = calloc(strlen(word_buffer)+1,1);
				strcpy(result.literall,word_buffer);
				read_word = false;
				*bp += read_index;
				return result;
			}
		}

		while (read_number_literall)
		{
			word_buffer[read_index] = c;
			c = next_symbol(buffer,len,*bp+read_index);
			read_index++;
			if (!is_number(c)) 
			{
				result.type = TOKEN_WORD_LITERALL_NUMBER;
				result.literall = calloc(strlen(word_buffer)+1,1);
				strcpy(result.literall,word_buffer);
				read_number_literall = false;
				*bp += read_index;
				return result;
			}
		}


		switch (c) {

			case '\n':
			case ';':
				result.type = TOKEN_SYMBOL_EXPRESSION_END;
			break;

			case '+':
				if (next_symbol(buffer,len,*bp)=='+') //{
					result.type =  TOKEN_BINARY_OPERATOR_PLUSPLUS;
					/* if (next_symbol(buffer,len,*bp+1)=='+') */
					/* 	assert(ERROR && "SYNTAX ERROR: more than 2 pluses"); */
				//}
				else 
					result.type =  TOKEN_BINARY_OPERATOR_PLUS;
				
			break;

			case '-':
				if (next_symbol(buffer,len,*bp)=='-') 
					result.type = TOKEN_BINARY_OPERATOR_MINUSMINUS;
				else 
					result.type =  TOKEN_BINARY_OPERATOR_MINUS;
			break;


			case '=':
				if (next_symbol(buffer,len,*bp)=='=') 
					result.type = TOKEN_BINARY_OPERATOR_EQUAL;
				else 
					result.type =  TOKEN_BINARY_OPERATOR_ASIGN;
			break;


			case '(':
				result.type = TOKEN_SYMBOL_PAREN_OPEN; break;
			case ')':
				result.type = TOKEN_SYMBOL_PAREN_CLOSE; break;


			case '{':
				result.type = TOKEN_SYMBOL_CURLY_OPEN; break;
			case '}':
				result.type = TOKEN_SYMBOL_CURLY_CLOSE; break;

			case '.':
				result.type = TOKEN_SYMBOL_DOT; break;


			case ':':
				result.type = TOKEN_SYMBOL_COLON; break;

			case ',':
				result.type = TOKEN_SYMBOL_COMMA; break;

			case '#':
				result.type = TOKEN_SYMBOL_HASH; break;

			default: break;
		}
	//}


	switch (result.type) {

		case TOKEN_WORD_LITERALL_NUMBER:
			//*bp += strlen(result.literall);
		break;
		case TOKEN_BINARY_OPERATOR_PLUSPLUS:
		case TOKEN_BINARY_OPERATOR_MINUSMINUS:
		case TOKEN_BINARY_OPERATOR_EQUAL:
			*bp += 2;
		break;

		default: *bp += 1; break;
	}

	return result;
}


void token_print(Token t) {
	if (t.type!=TOKEN_SPACE)
		switch(t.type) {


			case TOKEN_BINARY_OPERATOR_PLUS: 
				printf(" TOKEN_BINARY_OPERATOR_P\t"); 
				break;

			case TOKEN_BINARY_OPERATOR_PLUSPLUS: 
				printf(" TOKEN_BINARY_OPERATOR_PP\t"); 
				break;

			case TOKEN_BINARY_OPERATOR_MINUS: 
				printf(" TOKEN_BINARY_OPERATOR_M\t"); 
				break;

			case TOKEN_BINARY_OPERATOR_MINUSMINUS: 
				printf(" TOKEN_BINARY_OPERATOR_MM\t"); 
				break;

			case TOKEN_BINARY_OPERATOR_ASIGN: 
				printf(" TOKEN_BINARY_OPERATOR_ASIGN\t"); 
				break;

			case TOKEN_BINARY_OPERATOR_EQUAL: 
				printf(" TOKEN_BINARY_OPERATOR_EQ\t"); 
				break;

			case TOKEN_SYMBOL_PAREN_OPEN:
				printf(" TOKEN_SYM_PAREN_OPEN\t"); 
				break;

			case TOKEN_SYMBOL_PAREN_CLOSE:
				printf(" TOKEN_SYM_PAREN_CLOSE\t"); 
				break;

			case TOKEN_SYMBOL_CURLY_OPEN:
				printf(" TOKEN_SYM_CURLY_OPEN\t"); 
				break;

			case TOKEN_SYMBOL_CURLY_CLOSE:
				printf(" TOKEN_SYM_CURLY_CLOSE\t"); 
				break;

			case TOKEN_SYMBOL_DOT: 
				printf(" TOKEN_SYMBOL_DOT\t"); 
				break;

			case TOKEN_WORD_LITERALL_NUMBER: 
			    printf(" TOKEN_LITERALL_NUMBER (%s)\t",
			   		 t.literall);
		 		break;

			case TOKEN_WORD: 
				printf(" TOKEN_WORD: \"%s\"\t",
						t.literall);
				break;

			case TOKEN_SYMBOL_EXPRESSION_END: 
				printf(" ;;\n");
				break;

			case TOKEN_SYMBOL_COMMA: 
				printf(" TOKEN_SYMBOL_COMMA\t");
				break;

			case TOKEN_SYMBOL_COLON: 
				printf(" TOKEN_SYMBOL_COLON\t");
				break;

			case TOKEN_SYMBOL_HASH:
				printf(" TOKEN_SYMBOL_HASH\t");
				break;

			default: 
				printf("UKNOWN TOKEN\n");
				break;
		}
}

#define TOKENS_BLOCK_SIZE 1024

void free_tokenlist(Token *t, size_t list_size) {
	for(uint i = 0; i < list_size; i++) {
		switch(t->type) {

			case TOKEN_WORD_LITERALL_NUMBER: 
				free(t[i].literall);
				break;

			case TOKEN_WORD: 
				free(t[i].literall);
				break;

			default: 
				if (t[i].literall)
					free(t[i].literall);
				break;			
		}

	}
	free(t);
}

void tokens_list_append(Token *list,size_t *len,size_t *pos,Token t) {
	list[*pos] = t;
	(*pos)++;
}

