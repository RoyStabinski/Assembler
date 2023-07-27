/*  The parser main job is to scan the given file,
	parses its assembly code and then creates the program suitable output files, 
    and print errors and warning in case of it was detected. */

#ifndef PARSER_H
#define PARSER_H

#include "Data.h"
#include "Utills.h"

#define MEMORY_STORAGE_BEGIN 100
#define MEMORY_STORAGE_END 256

#define MAX_LINE_LENGTH 80
#define WORD_LENGTH 14

enum parser_err_val {
	parser_valid,
	parser_unexpected_space,
	parser_unexpected_command,
	parser_low_params,
	parser_extra_params,
	parser_quotes,
	parser_unfinished_string,
	parser_wrong_char,
	parser_invalid_jump,
	parser_wrong_num,
	parser_wrong_label,
	parser_wrong_destnation_param,
	parser_wrong_source_param
};

typedef struct word {
	unsigned int field : WORD_LENGTH; /*Can be by max the WORD_LENGTH*/
} word;

typedef struct instruction { /*Contains the attributes of the insruction*/
	void *value;
	int islabel;
	int line; 
} instruction;


unsigned int encoding_label(int); /*Encoding the label and put the parameters into a list */
enum parser_err_val encoding_operation(char *, enum operation, int, struct listnode **, struct listnode **, int *); /* Encoding the operation and puts the parameters into a list */
int encoding_number(char *, struct listnode **, int *); /*Encoding the string as a number and determine if the number is valid or not*/
enum parser_err_val encoding_string(char *, struct listnode **, int *); /*Encoding the string and in case of wrong datas returns a suitable error message*/
enum parser_err_val parsing_parameters(char *, int *, int, struct listnode **);/*Parasing the parameters and put them into list - in case of error , printing a suitable error message*/
enum addressmethod which_address_method(char *s); /* Determine which addressing method is suitable to the string */

int check_space(char);/* Check if the char is a space */
int out_of_line(char c);/*Checks if the char is out of the line(checks by size)*/
int check_num(char *);/*Checks if the number is valid*/
int check_label(char *);/*Checks if the label is valid(By label rules)*/
int ignore_whitespace(char [], int *);/*Jump over space white spaces */
int count_characters(char [], int *);/*Counting how many chars (not including white spaces)*/
int counting__to(char *, char);/*Couting the string to specific point*/
char *copy_to(char *, char);/*Copies the string from index to index*/

#endif

