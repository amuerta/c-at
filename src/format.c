#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#pragma once

#define HCH_STR_IMPLEMENTATION
#include "../lib/str.h"

#define RESET		"\e[0m"
#define RED			"\e[38;2;255;0;0m"
#define GREEN		"\e[38;2;0;255;0m"
#define BLUE 		"\e[38;2;0;0;255m"
#define YELLOW		"\e[38;2;255;255;0m"

#define LIGHT_RED	"\e[38;2;255;128;0m"
#define LIGHT_GREEN	"\e[38;2;128;255;0m"
#define LIGHT_BLUE 	"\e[38;2;0;128;255m"

#define MAGNETA 	"\e[38;2;255;0;255m"
#define PINK		"\e[38;2;255;0;127m"
#define CYAN		"\e[38;2;0;255;255m"


#define	getname(var) #var

void __assert_w_error(bool condition, char* err,char* calle) {
	if (!condition) {
		printf("\n[%sERROR%s] (%s:%s)\t:\n>\t %s \"%s\" %s",
				RED,
				RESET,
				__FILE__,
				calle,
				LIGHT_RED,
				err,
				RESET
		);
		exit(-1);
	}
}
#define assert_w_error(CON,ERR) __assert_w_error((CON),(ERR),(char*)__func__)

void printl(char* msg) {
	printf("\n[%sLOG%s]:\t \"%s\"",
			LIGHT_GREEN,
			RESET,
			msg
	);
}

void printw(char* msg) {
	printf("\n[%sWARN%s]:\t \"%s\"",
			LIGHT_RED, // orange
			RESET,
			msg
	);
}


uint dir_files_count(struct dirent* e, DIR* d) {
	rewinddir(d);
	size_t count = 0;
	while ((e = readdir(d)) != NULL) 
		count++;
	return count;
}

bool dir_has(struct dirent* e, DIR* d, char* name) {
	rewinddir(d);
	while ((e = readdir(d)) != NULL) 
		if (strcmp(e->d_name,name) == 0)
			return true;
	return false;
}

bool print_dir(struct dirent* e, DIR* d, uint indent) {
	rewinddir(d);
	while ((e = readdir(d)) != NULL) 
	{
		for(uint i = 0; i < indent; i++) printf("\t");
		printf("| > %s\n",e->d_name);
	}
	return false;
}

bool file_has_str(char* fpath, char* pattern) {
	FILE* f;
	char c, *buffer;
	size_t f_len;
	size_t p_len;

	f = fopen(fpath,"r");
	
	if (!f) 
		return false;

    fseek(f, 0, SEEK_END);
	f_len = ftell(f);
	rewind(f);

	buffer = calloc(f_len + 1,sizeof(c));
	assert(buffer && "FAILED TO CALLOC ");

	for(uint i = 0; i < f_len; i++) {
		buffer[i] = fgetc(f);
	}

	String str = str_move_cstr(buffer);
	String pat = str_move_cstr(pattern);

	bool result = (str_has_pattern(str,pat) != STR_NOPATTERN) ;
	free(buffer);
	fclose(f);
	return result;
}

FILE* open_file_for_logging(char* fpath) {
	FILE* f; 
	f = fopen(fpath, "a+");
	assert_w_error(f, "Failed to open file for logging");
	if (!f)
		perror("\t> Error");
	return f;
}

void clear_file(char* fpath) {
	FILE* f; 
	f = fopen(fpath, "w+");
	if (f)
		fclose(f);
}
