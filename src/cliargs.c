#define STR_IMPLEMENTATION
#include "str.h"


int str_lib_test(void) {
	String str1;
	String str2;

	char* c_str = calloc(7,sizeof(char));
	memcpy(c_str,"World!",6);
	str1 = str_move_cstr(c_str);
	
	assert(strcmp(c_str,str1.ptr)==0 && "FAIL: strings are not equal");
	printf("\t[OK]\tnot owned cstring put to string \n");

	str_free(&str1);
	if (str_is_empty(str1))
		printf("\t[OK]\tstring cleared, memory freed\n");

	// 

	str1 = str_create("Shrimp");
	str2 = str_dup(str1);
	
	if (str_are_equal(str1,str2))
		printf("\t[OK]\tduped strings are equal\n");
	else 
		assert(false && "Dupped strings are not equal");

	str_free(&str1);
	str_free(&str2);

	str1 = str_create("hello");
	str2 = str_dup(str1);
	str_reverse(&str2);

	char* cpy1 =	str_get_cstr(str1); 
	char* cpy2 =	str_get_cstr(str2); 
	
	if (strcmp(cpy2,"olleh")==0) 
		printf("\t[OK]\tstring sucessfuly reversed: '%s' -> '%s'\n",
			cpy1,cpy2);
	else 
		assert(false && "[FAIL] reverse of a string failed");

	free(cpy1);
	free(cpy2);
	
	str_free(&str1);
	str_free(&str2);
	
	str1 = str_from_cstr("Magnum",0); 
	str2 = str_from_cstr("gn",0); 

	if (str_has_pattern(str1,str2)!=STR_NOPATTERN) 
		printf("\t[OK]\tFound pattern 'gn' in 'Magnum'\n");
	else 
		assert(false && "[FAIL] failed to find pattern");
	
	str_free(&str1);
	str_free(&str2);

	str1 = str_from_cstr("10.1",0);
	str2 = str_from_cstr("-3000",0);

	if (str_is_float(str1) && str_is_integer(str2)) 
		printf("\t[OK]\tnumber recognition is correct\n");
	else
		assert(false && "[FAIL] failed to recognize number\n");
	

	str_free(&str1);
	str_free(&str2);


	str1 = str_create("Hello ");
	str2 = str_create("Hello World!");
	str_append_chars(&str1,"World!");

	if(str_are_equal(str1,str2))
		printf("\t[OK]\tstr_append_chars when str1.len != 0\n");
	else
		assert(false && "[FAIL] Strings are not equal");

	str_free(&str1);
	str_free(&str2);

	str1 = (String) {0};
	str2 = str_create("Hello World!");
	str_append_chars(&str1,"Hello World!");

	if(str_are_equal(str1,str2))
		printf("\t[OK]\tstr_append_chars when str1.len == 0\n");
	else
		assert(false && "[FAIL] Strings are not equal");

	str_free(&str1);
	str_free(&str2);

	str1 = str_create("Jo");
	str2 = str_create("Joe");
	str_append_char(&str1,'e');

	if(str_are_equal(str1,str2))
		printf("\t[OK]\tstr_append_char added char (len!=0)\n");
	else
		assert(false && "[FAIL] Strings are not equal");

	str_free(&str1);
	str_free(&str2);

	str1 = (String) {0};
	str2 = str_create("e");
	str_append_char(&str1,'e');

	if(str_are_equal(str1,str2))
		printf("\t[OK]\tstr_append_char added char (len==0)\n");
	else
		assert(false && "[FAIL] Strings are not equal");

	str_free(&str1);
	str_free(&str2);


	str1 = str_create("JoJo");
	str2 = str_substr(str1,2,2); // "__Jo"
	str_free(&str1);			 // "0123"
	str1 = str_create("Jo");	 // "  ^"
								 // "  12"
	if(str_are_equal(str1,str2))
		printf("\t[OK]\tstr_substr sucessfully created substring from origin\n");
	else
		assert(false && "[FAIL] Strings are not equal");

	str_free(&str1);
	str_free(&str2);

	return 0;
}

int main(void) {
	str_lib_test();
}
