#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string_slice.h>

typedef char* string;

/*
FULL INSTRUCTION SET:
(meta instructions)
#128 // allocate 128 bytes for data space (only valid at the very beginning of a program)

(variable instructions)
C0 : 12 // set address of variable
C0 = 10 // set value of variable
C1 = &C0 // store the address of a variable

(system pointer instructions)
<- P // move left program ptr
<- D // move left data ptr
-> P // move right p ptr
-> D // move right d ptr
P : 5 // move program ptr
D : 12 // move data ptr
P = 9 // set value at program ptr
D = 12 // set value at data ptr

(arithmetic instructions)
C0 ++ // inc
C0 -- // dec
C0 + 12 // add
C0 - 12 // sub
C0 * 1 // multiplication
C0 / 1 // integer division

(conditional instructions will run the following instruction if true)
C0 == C1 // is equal
C0 > C1 // is greater
C0 >= C1 // is greater or equal
C0 < C1 // is less
C0 <= C1 // is less or equal
C0 != C1 // is not

(scopes and control flow instructions)
[ $10 ... ] // loop 10 times
[ ~ ... ] // loop infinitely
[ ~ ... C0 != C1 ^ ] // break from infinite loop if C0 is not equal to C1
C0 == C1 [ -> P ] // if C0 is equal to C1, move the program pointer once to the right
*/

/*
OP CODES:
-1 : end of program data
0 : null
1 : variable (C), following integer represents the index in the v-table
2 : data pointer reference (D)
3 : program pointer reference (P)
4 : =
5 : :
6 : &
7 : *
8 : /
9 : +
10 : ++
11 : -
12 : --
13 : ==
14 : !=
15 : <
16 : >
17 : <=
18 : >=
19 : -> (move data or program pointer once to the right)
20 : <- (move data or program pointer once to the left)
21 : [ (begin scope)
22 : ] (end scope)
23 : $ (define number of iterations to run scope, following integer represents number of iterations)
24 : ^ (break from loop)
25 : ~ (define infinite loop)
26 : # (data space allocation instruction)
*/

enum TokenValues {
	EOD = -1, // End of program data marker
	_NULL, // NOP and padding
	VARIABLE,
	DATA_PTR_REF,
	PROG_PTR_REF,
	ASSIGN,
	COLON,
	AMPERSAND,
	ASTRICK,
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


/*
Steps:
1. Compile program to array of integers
2. Malloc space to store program and to store additional working memory
		- defined by user in the command line args?
		- defined by user in program data using `#<mem_size>`?
3. Initialize program pointer and data pointer
4. Begin incrementing the program pointer and running the instructions
5. Continue until program reaches an end of program data instruction (-1)
*/

/// Returns pointer to first int in array
i32* parse(FILE* source_file);
/// Returns the program status
i32 run(i32* program);

/*
Command line arguments:
--help							| -h						: print the help message
--verbose						| -v						: verbose information about parser and runtime states
--export						| -x						: export the binary for the program
--input <filename>	| -f <filename> : use the following file as a program
--output <filename> | -o <filename> : define the name for the exported program (defaults to program.bin)
*/

i32 main(int argc, char** argv) {

	printf("Argument count: %d\n", argc);
	printf("Arguments:\n");
	for(size_t i = 0; i < argc; i++) {
		printf("  %s\n", argv[i]);
	}

	const char* help_message = "--help       | -h      : print the help message\n--verbose      | -v      : verbose information about parser and runtime states\n--export       | -x       : export the binary for the program\n--input <filename> | -f <filename> : use the following file as a program\n--output <filename> | -o <filename> : define the name for the exported program (defaults to program.bin)\n";

	bool verbose = false;

	bool use_existing_file = false;
	char* name_of_input_file;

	bool export_binary = false;
	char* name_of_output_file;

	// convert all the static strings to slices
	StringSlice verbose_slice = string_to_slice("--verbose");
	StringSlice v_slice = string_to_slice("-v");
	StringSlice help_slice = string_to_slice("--help");
	StringSlice h_slice = string_to_slice("-h");
	StringSlice export_slice = string_to_slice("--export");
	StringSlice x_slice = string_to_slice("-x");
	StringSlice input_slice = string_to_slice("--input");
	StringSlice f_slice = string_to_slice("-f");
	StringSlice output_slice = string_to_slice("--output");
	StringSlice o_slice = string_to_slice("-o");

	// process command line arguments
	for(size_t i = 1; i < argc; i++) {
		// convert args to slices
		StringSlice curr_arg = string_to_slice(argv[i]);
		

		if(streql(&curr_arg, &verbose_slice) || streql(&curr_arg, &v_slice)) verbose = true;

		if(streql(&curr_arg, &help_slice) || streql(&curr_arg, &h_slice)) printf("%s", help_message);
		
		if(streql(&curr_arg, &export_slice) || streql(&curr_arg, &x_slice)) export_binary = true;
		
		if(streql(&curr_arg, &input_slice) || streql(&curr_arg, &f_slice)) {
			StringSlice next_arg;
			if(argc > i + 1) { next_arg = string_to_slice(argv[i + 1]); }
			else {
				printf("Missing a file name for input flag.");
				return 1;
			}

			use_existing_file = true;
			name_of_input_file = slice_to_string(next_arg);
		}
		
		if(streql(&curr_arg, &output_slice) || streql(&curr_arg, &o_slice)) {
			StringSlice next_arg;
			if(argc > i + 1) { next_arg = string_to_slice(argv[i + 1]); }
			else {
				printf("Missing a file name for output flag.");
				return 1;
			}

			export_binary = true;
			name_of_output_file = slice_to_string(next_arg);
		}
	}

	if(argc == 1) {
		printf("No flags passed in.");
	} else {
		// debug for flags
		printf("Flags:\n  Verbose? %d\n  Input file? %d Name: %s\n  Export binary? %d Name: %s\n", verbose, use_existing_file, name_of_input_file, export_binary, name_of_output_file);
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
		size_t program_len = *(&compiled_program + 1) - compiled_program;

		// export program to file
		if(export_binary) {
			printf("Exporting compiled program to %s...\n", name_of_output_file);

			FILE* exported_file;
			errno_t export_errors = fopen_s(&exported_file, name_of_output_file, "wb");
			if(export_errors != 0) {
				fprintf(stderr, "[ERROR] Cannot export file %s.\n\tError: %s\n", name_of_output_file, strerror(err));
				return 1;
			}
		
			fwrite(compiled_program, program_len * sizeof(i32), program_len, exported_file);
			fclose(exported_file);
			printf("Successfully exported program to %s.\n", name_of_output_file);
		}

		i32 status = run(compiled_program);

		// run program
		return 0;
	}

	return 0;
}

i32* parse(FILE* source_file) {
	char *temp;
	size_t n = 0;
	int c;

	size_t buf_size = 1;
	temp = malloc(sizeof(char) * buf_size);

	while((c = fgetc(source_file)) != EOF) {
		temp[n++] = c;
	}

	temp[n] = '\0'; // null terminate

	StringSlice source_code = string_to_slice(temp);

	free(temp);

	i32* compiled_code = (int*)malloc(sizeof(i32) * (source_code.len + 1));
	size_t write_index = 0;

	for(size_t i = 0; i < source_code.len; i++) {
		char curr_tok = source_code.begin[i];
		switch(curr_tok) {
			case 'C':
				compiled_code[write_index] = VARIABLE;
				size_t next = i + 1;
				i32 integer_buf_size = 10; // the MAX_I32 value has 10 digits
				char* buf = (char*)malloc(sizeof(char) * integer_buf_size);
				for(size_t j = 0; j < integer_buf_size; j++) {
					if(isdigit(source_code.begin[i + j])) {
						buf[j] = source_code.begin[i + j];
					} else {
						buf[j] = '\0'; // null terminate
						i += j; // consume all the integer characters
						break;
					}
				}

				if(strlen(buf) == 0) {
					fprintf(stderr, "[ERROR] Variable reference or declaration does not have an variable index following the `C` symbol.\n");
					exit(1);
				}

				int var_index = atoi(buf);
				compiled_code[next] = var_index;

				free(buf);

				write_index++;
				break;
			case 'D':
				compiled_code[write_index] = DATA_PTR_REF;
				write_index++;
				break;
			case 'P':
				compiled_code[write_index] = PROG_PTR_REF;
				write_index++;
				break;
			case 'I':
				compiled_code[write_index] = INT_LITERAL;

				integer_buf_size = 10; // the MAX_I32 value has 10 digits
				buf = (char*)malloc(sizeof(char) * integer_buf_size);
				for(size_t j = 0; j < integer_buf_size; j++) {
					if(isdigit(source_code.begin[i + j])) {
						buf[j] = source_code.begin[i + j];
					} else {
						buf[j] = '\0'; // null terminate
						i += j; // consume all the integer characters
						break;
					}
				}

				if(strlen(buf) == 0) {
					fprintf(stderr, "[ERROR] Expected an integer following `I` symbol.\n");
					exit(1);
				}

				var_index = atoi(buf);
				compiled_code[next] = var_index;

				free(buf);

				write_index++;
				break;
			case '=':
				if(source_code.begin[i + 1] != '=') {
					compiled_code[write_index] = ASSIGN;
				} else {
					compiled_code[write_index] = EQUIV;
					write_index++;
				}
				write_index++;
				break;
			case ':':
				compiled_code[write_index] = COLON;
				write_index++;
				break;
			case '&':
				compiled_code[write_index] = AMPERSAND;
				write_index++;
				break;
			case '*':
				compiled_code[write_index] = ASTRICK;
				write_index++;
				break;
			case '/':
				compiled_code[write_index] = DIV;
			case '+':
				if(source_code.begin[i + 1] != '+') {
					compiled_code[write_index] = INC;
				} else {
					compiled_code[write_index] = PLUS;
					write_index++;
				}
				write_index++;
				break;
			case '-':
				if(source_code.begin[i + 1] != '-') {
					compiled_code[write_index] = DEC;
				} else if(source_code.begin[i + 1] == '>'){
					compiled_code[write_index] = RIGHT_ARROW;
					write_index++;
				} else {
					compiled_code[write_index] = MINUS;
					write_index++;
				}
				write_index++;
				break;
			case '!':
				if(source_code.begin[i + 1] == '=') {
					compiled_code[write_index] = NOT_EQUIV;
					write_index++;
				} else {
					fprintf(stderr, "[ERROR] Expected `=` following `!` symbol.\n");
					exit(1);
				}
				write_index++;
				break;
			case '<':
				if(source_code.begin[i + 1] != '=') {
					compiled_code[write_index] = LESS; 
				} else if(source_code.begin[i + 1] == '-') {
					compiled_code[write_index] = LEFT_ARROW;
				} else {
					compiled_code[write_index] = LESS_EQUAL;
					write_index++;
				}
				write_index++;
				break;
			case '>':
				if(source_code.begin[i + 1] != '=') {
					compiled_code[write_index] = GREATER; 
				} else {
					compiled_code[write_index] = GREATER_EQUAL;
					write_index++;
				}
				write_index++;
				break;
			case '[':
				compiled_code[write_index] = BEGIN_SCOPE;
				write_index++;
				break;
			case ']':
				compiled_code[write_index] = END_SCOPE;
				write_index++;
				break;
			case '$':
				compiled_code[write_index] = DEFINE_SCOPE_ITERATIONS;
				next = i + 1;
				integer_buf_size = 10; // the MAX_I32 value has 10 digits
				buf = (char*)malloc(sizeof(char) * integer_buf_size);
				for(size_t j = 0; j < integer_buf_size; j++) {
					if(isdigit(source_code.begin[i + j])) {
						buf[j] = source_code.begin[i + j];
					} else {
						buf[j] = '\0'; // null terminate
						i += j; // consume all the integer characters
						break;
					}
				}

				if(strlen(buf) == 0) {
					fprintf(stderr, "[ERROR] Expected an integer following `$` symbol.\n");
					exit(1);
				}

				var_index = atoi(buf);
				compiled_code[next] = var_index;

				free(buf);

				write_index++;
				break;
			case '~':
				compiled_code[write_index] = INFINITE_LOOP;
				write_index++;
				break;
			case '^':
				compiled_code[write_index] = BREAK_FROM_SCOPE;
				write_index++;
				break;
			case '#':
				compiled_code[write_index] = DEFINE_DATA_MEM_SIZE;
				next = i + 1;
				integer_buf_size = 10; // the MAX_I32 value has 10 digits
				buf = (char*)malloc(sizeof(char) * integer_buf_size);
				for(size_t j = 0; j < integer_buf_size; j++) {
					if(isdigit(source_code.begin[i + j])) {
						buf[j] = source_code.begin[i + j];
					} else {
						buf[j] = '\0'; // null terminate
						i += j; // consume all the integer characters
						break;
					}
				}

				if(strlen(buf) == 0) {
					fprintf(stderr, "[ERROR] Expected an integer following `#` symbol.\n");
					exit(1);
				}

				var_index = atoi(buf);
				compiled_code[next] = var_index;

				free(buf);

				write_index++;
				break;
			case ';':
				compiled_code[write_index] = EOS;
				write_index++;
				break;
			default: break;
		}
	}

	compiled_code[source_code.len + 1] = EOD;

	fclose(source_file);

	return compiled_code;
}

i32 run(i32* program) {
	return FAIL;
}
