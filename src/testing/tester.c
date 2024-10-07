#include "tester.h"
#include "test_list.c"

TestResult run_set_program(TestProgram entry, FILE* log) {
	Machine m = init_machine(
			entry.program,
			entry.program_size
	);

	m.core_logger = log;

	time_ms_t before, after;
	before = time_in_microsec	();
	execute_program(&m);
	after = time_in_microsec	();
	
	TestResult result = {
		.sucess = ( entry.desired_return == m.accum_register ),
		.return_code = m.accum_register,
		.time_elapsed = (after - before)
	};

	return result;
}

bool run_native_tests(TestProgram list[], size_t list_size, size_t* sucessful_tests, FILE* log) {
	TestResult r;
	bool all_tests_passed = true;


	printf("\n\n");

	for(uint i = 0; i < list_size; i++) {
		TestProgram item = list[i];
		
		printf("\t [%sTESTING%s] native: \"%s\"\t",
				LIGHT_BLUE,
				RESET,
				item.name
		);

		fprintf(log,"%s", "\n" "\n--- EXECUTION START ---\n");
		r = run_set_program(item,log);
		fprintf(log,"%s","\n--- EXECUTION END   ---\n");

		all_tests_passed = all_tests_passed && r.sucess;

		if (r.sucess) {
			(*sucessful_tests)++;
			printf("\t\t[%sOK%s]\t time: %llums",
					LIGHT_GREEN,
					RESET,
					r.time_elapsed
					);
		}
		else {
			printf("[%sFAIL%s]\t return: %u",
					RED,
					RESET,
					r.return_code
			);
		}
		printf("\n");
	}
	return all_tests_passed;
}

void run_compiled_tests() {}

int main(void) {
	struct dirent* 	dir_entry;
	DIR*  			directory;
	DIR* 			tests_dir;
	FILE* 			log_file;
	
	bool
		has_asm_compiled_dir,
		has_asm_compiled_files_in_dir,
		test_asm_compiled
	;

	directory = opendir(".");
	assert_w_error(directory != NULL, 
			"Directory like that does not exist, How could this happen??");
	printl("Opened dir");

	assert_w_error(dir_has(dir_entry,directory,".git"),
			"Enviroment you are in is not a github repo"); 
	printl("Found .git dir");
	
	assert_w_error(file_has_str("./.git/config",GIT_REPO),
			"Failed to proof that repo belongs to C-AT project in ./.git/config");
	printl("The directory belongs to C-AT: (" GIT_REPO ")");

	assert_w_error(dir_has(dir_entry,directory,"log"),
			"Didn't found log directory, consider creating one");
	printl("Log directory found");

	test_asm_compiled = (
			TEST_ASM &&
			dir_has(dir_entry,directory,"compiled_tests") &&
			dir_files_count(dir_entry,directory) > 2
	);

	char* log_path = "./log/core_dump";

	clear_file(log_path);
	log_file = open_file_for_logging(log_path);
	


	if (test_asm_compiled) {
		tests_dir = opendir("./compiled_tests");
		printl("Found compiled tests dir"); 
		printf("\tfile count: %u\n",dir_files_count(dir_entry,tests_dir));

		run_compiled_tests();

		closedir(tests_dir);
	} else {
		if (TEST_ASM)
			printw("NOT found compiled tests, skipping this test case");
		else 
			printw("compiled tests disabled via macro, skipping this test case");
	}


	printl("Running native tests");
	size_t sucessful_tests = 0;
	bool native_all_good = run_native_tests(
			TESTS_LIST,
			TESTS_LIST_LEN,
			&sucessful_tests,
			log_file
	);

	printf("\n\t---\t[%sRESULTS%s]\t---\n",
			LIGHT_GREEN, RESET);

	printf("\t  Native test -> [%s]\t %lu/%lu \n",
			(native_all_good)? 
				GREEN"SUCCESS"RESET : 
				RED"FAILURE"RESET,

			TESTS_LIST_LEN,
			sucessful_tests
	);

	printf("\t---\t-------------\t---\n");


	//printf("\n  FILES:\n");
	//print_dir(dir_entry,directory,1);
	
	fclose(log_file);
	closedir(directory);
}
