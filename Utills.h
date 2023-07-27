/* Utills for errors,symbols and table printing */

#ifndef UTILLS_H
#define UTILLS_H
#include <stdlib.h>
#include <stdio.h>

enum error_type {
	err_invalid_label_name,
	err_label_symbol,
	err_label_defined,
	err_undefined_label,
	warning_empty_label,
	warning_unused_label,

	err_param_unexcpected_space,
	err_param_unexcpected_command,
	err_low_param,
	err_extra_param,
	err_param_jump,

	err_opcode_wrong_source,
	err_opcode_wrong_dest,
	err_wrong_data_num,

	err_quotes,
	err_unfinished_string,
	err_ascii,

	err_wrong_instruction,

	err_out_of_line,
	err_binary_over_flew,

	err_counter
};

void *verifed_malloc(size_t);/*In case of malloc failure*/
FILE *open_file(char *, char *);/*In case of open failure*/
int close_file(FILE *);/*In case of close failure*/

void error_type_prepare(void);/*Setting up the error type messages*/
void error_type_clear(void);/*Clear the memory storage for errors*/
void warning_out(char *, int, enum error_type, ...);/*Printing warning message*/
void error_out(char *, int, enum error_type, ...);/*Printing error message*/
char *copy1(char *);/*Copies specific amount of chars from the string*/
char *copy2(char *, size_t);/*Copies the string*/
void switch_extension(char *, char *);/*Swithich the file extension with another one*/


enum operation { /*Set up according to the asked list in the project*/
	UNKNOWN_SYMBOL = -1,

	MOV,
	CMP,
	ADD,
	SUB,
	NOT,
	CLR,
	LEA,
	INC,
	DEC,
	JMP,
	BNE,
	RED,
	PRN,
	JSR,
	RTS,
	STOP,

	DIRECTIVE_DATA,
	DIRECTIVE_STRING,
	DIRECTIVE_ENTRY,
	DIRECTIVE_EXTERN,

	REGISTER
};

enum encoding {
	ENC_ABSOLUTE,
	ENC_EXTERNAL, 
	ENC_RELOCATABLE 
};

enum addressmethod {
	ADDRESS_ERROR = -1,
	ADDRESS_INSTANT, 
	ADDRESS_DIRECT, 
	ADDRESS_JUMP_WITH_PARAMS,
	ADDRESS_DIRECT_REGISTER 
};

enum paramamount {
	PARAM_LIST = -2,
	PARAM_UNKNOWN,
	PARAM_NONE,
	PARAM_SINGLE,
	PARAM_TWO,
	PARAM_JUMP
};

enum labelattribute {
	LABEL_ENTRY = 1 << 0,
	LABEL_EXTERNAL = 1 << 1,
	LABEL_DATA = 1 << 2,
	LABEL_INSTRUCTION = 1 << 3
};

void operations_make(void);/*Setting up memory storage*/
void operations_clear(void);/*Clearing up the remining storage that was for operartions_make*/
enum operation opeations_get(char *);/*Returns value operation */
enum paramamount symbol_amount(enum operation);/*Returns the amount of operation*/
int is_operation(char *); /*If the inputed string is operation*/
int is_directive(char *); /*If the inputed string is directive*/
int is_register(char *); /*If the inputed string is register*/
int is_jmp_operation(enum operation); /*If the inputed string isjump operation*/
int is_data_directive(enum operation); /*If the inputed string is data directive*/


#endif


