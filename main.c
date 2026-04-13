#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "structs.c"

enum TokenValues {
	EOD = -1, // End of program data marker
	_NULL, // NOP and padding
	VARIABLE,
	DATA_PTR_REF,
	PROG_PTR_REF,
	ASSIGN,
	ADDRESS_ASSIGN,
	DECLARE_REF,
	DEREF,
	MULTI,
	DIV,
	PLUS,
	INC,
	MINUS,
	DEC,
	EQUIV,
	NOT_EQUIV,
	LESS,
	GREATER,
	LESS_EQUAL,
	GREATER_EQUAL,
	RIGHT_ARROW,
	LEFT_ARROW,
	BEGIN_SCOPE,
	END_SCOPE,
	DEFINE_SCOPE_ITERATIONS,
	BREAK_FROM_SCOPE,
	INFINITE_LOOP,
	DEFINE_DATA_MEM_SIZE,
	INT_LITERAL,
	EOS // end of statement
};

enum STATUS {
	FAIL = -1,
	SUCCESS,
	SYNTAX_ERROR,
	OUT_OF_BOUNDS
};
char* interpreter_err_msg;
size_t data_mem_size = 128; // size of allocated memory for interpreter data (in bytes)

/// Returns pointer to first int in array
i32* parse(FILE* source_file);
/// Returns the program's exit status. 
/// Interpret the given program, allocate memory equal to program_size + data_mem_size (defined by the program or is 128 bytes)
i32 run(i32* program, size_t program_size);

i32 main(int argc, char** argv) {
	const char* help_message = "--help       | -h      : print the help message\n--verbose      | -v      : verbose information about parser and runtime states\n--export       | -x       : export the binary for the program\n--input <filename> | -f <filename> : use the following file as a program\n--output <filename> | -o <filename> : define the name for the exported program (defaults to program.bin)\n";

	bool verbose = false;

	bool use_existing_file = false;
	char* name_of_input_file = NULL;

	bool export_binary = false;
	char* name_of_output_file = NULL;

	// process command line arguments
	for(size_t i = 1; i < argc; i++) {
		char* curr_arg = argv[i];
		
		if(strcmp(curr_arg, "--verbose") == 0 || strcmp(curr_arg, "-v") == 0) verbose = true;

		if(strcmp(curr_arg, "--help") == 0 || strcmp(curr_arg, "-h") == 0) printf("%s", help_message);
		
		if(strcmp(curr_arg, "--export") == 0 || strcmp(curr_arg, "-x") == 0) export_binary = true;
		
		if(strcmp(curr_arg, "--input") == 0 || strcmp(curr_arg, "-f") == 0) {
			if(argc > i + 1) {
				use_existing_file = true;
				name_of_input_file = malloc(strlen(argv[i + 1]) + 1);
				if(name_of_input_file == NULL) {
					fprintf(stderr, "[ERROR] Failed to allocate memory for input filename\n");
					return 1;
				}
				strcpy(name_of_input_file, argv[i + 1]);
				i++;
			} else {
				printf("Missing a file name for input flag.");
				return 1;
			}
		}
		
		if(strcmp(curr_arg, "--output") == 0 || strcmp(curr_arg, "-o") == 0) {
			if(argc > i + 1) {
				export_binary = true;
				name_of_output_file = malloc(strlen(argv[i + 1]) + 1);
				if(name_of_output_file == NULL) {
					fprintf(stderr, "[ERROR] Failed to allocate memory for output filename\n");
					return 1;
				}
				strcpy(name_of_output_file, argv[i + 1]);
				i++;
			} else {
				printf("Missing a file name for output flag.");
				return 1;
			}
		}
	}

	if(argc == 1) {
		printf("No flags passed in.");
	} else {
		printf("Flags:\n  Verbose? %d\n  Input file? %d Name: %s\n  Export binary? %d Name: %s\n", 
		       verbose, 
		       use_existing_file, 
		       name_of_input_file ? name_of_input_file : "(none)",
		       export_binary, 
		       name_of_output_file ? name_of_output_file : "(none)");
	}

	if (use_existing_file) {
		printf("Using existing file...\n");

		FILE* source_file;
		errno_t err = fopen_s(&source_file, name_of_input_file, "r");
		if(err != 0) {
			fprintf(stderr, "[ERROR] Cannot open file %s.\n\tError: %s\n", name_of_input_file, strerror(err));
			return 1;
		}
		
		printf("Parsing %s...\n", name_of_input_file);

		i32* compiled_program = parse(source_file);
		
		size_t program_len = 0;
		while(compiled_program[program_len] != EOD) {
			program_len++;
		}
		program_len++;

		printf("Compiled program (modified slightly for readability):\n");
		for(size_t i = 0; i < program_len; i++) {
			if(compiled_program[i] == EOS) {
				printf(";\n");
			} else if(compiled_program[i] == EOD) {
				printf("\nEOD\n");
			} else {
				printf("%d ", compiled_program[i]);
			}
		}
		printf("\n");

		// export program to file
		if(export_binary) {
			printf("Exporting compiled program to %s...\n", name_of_output_file);

			if(name_of_output_file == NULL) name_of_output_file = "a.bin";

			FILE* exported_file;
			errno_t export_errors = fopen_s(&exported_file, name_of_output_file, "w");
			if(export_errors != 0) {
				fprintf(stderr, "[ERROR] Cannot export file %s.\n\tError: %s\n", name_of_output_file, strerror(export_errors));
				return 1;
			}
		
			fwrite(compiled_program, sizeof(i32), program_len, exported_file);
			fclose(exported_file);
			printf("Successfully exported program to %s.\n", name_of_output_file);
		}

		i32 status = run(compiled_program, program_len);

		if(name_of_input_file) free(name_of_input_file);
		if(name_of_output_file) free(name_of_output_file);
		free(compiled_program);

		// run program
		return 0;
	}

	return 0;
}

//! NEED TO REFACTOR TO USE SHUNTING YARD AND OUTPUT POSTFIX
i32* parse(FILE* source_file) {
	char *source_code;
	size_t code_len = 0;
	int c;

	size_t buf_size = 1024;
	source_code = malloc(sizeof(char) * buf_size);

	while((c = fgetc(source_file)) != EOF) {
		if(code_len >= buf_size - 1) {
			buf_size *= 2;
			source_code = realloc(source_code, buf_size);
			if(source_code == NULL) {
				fprintf(stderr, "[ERROR] Failed to reallocate buffer\n");
				exit(1);
			}
		}
		source_code[code_len++] = c;
	}

	source_code[code_len] = '\0'; // null terminate
	
	printf("Source code:\n%s\n", source_code);

	i32* compiled_code = (int*)malloc(sizeof(i32) * (code_len * 2));
	if(compiled_code == NULL) {
		fprintf(stderr, "[ERROR] Failed to allocate memory for compiled code\n");
		free(source_code);
		exit(1);
	}

	size_t write_index = 0;
	i32 integer_buf_size;
	char* buf;
	int var_index;

	for(size_t i = 0; i < code_len; i++) {
		char curr_tok = source_code[i];
		switch(curr_tok) {}
	}

	if(write_index >= code_len * 2) {
		fprintf(stderr, "[ERROR] Write index exceeded allocated buffer size\n");
		free(source_code);
		free(compiled_code);
		exit(1);
	}
	
	compiled_code[write_index] = EOD;

	fclose(source_file);
	free(source_code);

	return compiled_code;
}

i32 run(i32* program, size_t program_size) {
	printf("[DEBUG] Beginning program...\n");
	if(data_mem_size == 0) data_mem_size = 128; // if user overwrites data_mem_size with `#0;` forcefully replace it with 128 

	printf("[DEBUG] Allocating memory space, size: %llu\n", sizeof(i32) * (program_size + data_mem_size));
	// reserved memory space for program and data
	i32* mem_space = (i32*)malloc(sizeof(i32) * (program_size + data_mem_size));
	size_t total_memory_size = program_size + data_mem_size;

	printf("[DEBUG] Initializing variable table, program counter, and data pointer...\n");
	// program counter
	i32* prog_ptr = mem_space;
	// points to somewhere in data
	i32* data_ptr = mem_space + program_size;
	// variable table for interpreter
	Map* vtable = (Map*)malloc(sizeof(Map));

	// evaluation stacks
	Stacki32* result_stack = build();

	// initialize memory
	memcpy(prog_ptr, program, program_size);
	memset(data_ptr, 0, data_mem_size);

	// interpreter variables
	Variable* left;
	Variable* right;

	// begin running program
	while(*prog_ptr != EOD) {
		switch(*prog_ptr) {
			case EOD:
				return SUCCESS;
			case _NULL:
				// do nothing, equivalent to NOP
				break;
			case VARIABLE:
			case DATA_PTR_REF:
			case PROG_PTR_REF:
			case ASSIGN:
			case ADDRESS_ASSIGN:
			case DECLARE_REF:
			case DEREF:
			case MULTI:
			case DIV:
			case PLUS:
			case INC:
			case MINUS:
			case DEC:
			case EQUIV:
			case NOT_EQUIV:
			case LESS:
			case GREATER:
			case LESS_EQUAL:
			case GREATER_EQUAL:
			case RIGHT_ARROW:
			case LEFT_ARROW:
			// not sure how I am supposed to handle scopes
			case BEGIN_SCOPE:
			case END_SCOPE:
			case DEFINE_SCOPE_ITERATIONS:
			case INFINITE_LOOP:
			case BREAK_FROM_SCOPE:
			case INT_LITERAL:
			case EOS: // end of statement
			default: break;
		}

		prog_ptr += 1;
		if(is_out_of_bounds(prog_ptr, mem_space, mem_space + total_memory_size)) {
			interpreter_err_msg = "Program Pointer ran out of bounds";
			return OUT_OF_BOUNDS;
		}
	}

	// clean up runtime memory space
	free(mem_space);
	free(vtable);
	free(result_stack);

	return FAIL;
}
