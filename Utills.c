#include "Utills.h"
#include "Data.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void *verifed_malloc(size_t s)
{
	void *ptr = malloc(s);
	if (ptr == NULL) {
		printf("Error!\n File cannot get memory");
		exit(1);
	}
	return ptr;
}

FILE *open_file(char *filename, char *mode)
{
	FILE *f = fopen(filename, mode);
	if (f == NULL) {
		printf("Error!\n File cannot be open (%s)\n", filename);
		exit(1);
	}
	return f;
}

int close_file(FILE *file)
{
	int i = fclose(file);
	if (i == EOF) {
		printf("Error!\n File cannot be closed\n");
		exit(1);
	}
	return i;
}

char **message;

void error_type_prepare(void)
{
	message = (char **) verifed_malloc(sizeof(*message) * err_counter);

	message[err_invalid_label_name] = "Error!\nLabel can't be longer then 30 characters and need to start with a letter followed by numbers or letters.(%s)";
	message[err_label_symbol] = "Error!\nLabel can't be a defined instruction(of language). (%s)";
	message[err_label_defined] = "Error!\nLabel can't defined more then once. (%s)";
	message[err_undefined_label] = "Error!\nUndefined label. (%s)";
	message[warning_empty_label] = "Error!\nLabel defined without instruction. (%s)";
	message[warning_unused_label] = "Error!\nIrrelevent label definition. (%s)";

	message[err_param_unexcpected_space] = "Error!\nThere is unplanned space in the parameters list.";
	message[err_param_unexcpected_command] = "Error!\nThere is unplanned command in parameters list.";
	message[err_low_param] = "Error!\nThere are not enough parameters in the parameters list.\nNeed to be (%d) parameters.";
	message[err_extra_param] = "Error!\nThere are too many parameters in the parameters list.\nNeed to be (%d) parameters.";
	message[err_param_jump] = "Error!\njmp operator's label need to be followed by 2 parameters  or without parameters at all.";

	message[err_opcode_wrong_source] = "Error!\nWrong source registers for %s.";
	message[err_opcode_wrong_dest] = "Error!\nWrong destination registers for %s.";
	message[err_wrong_data_num] = "Error!\nWrong value. (%s)";

	message[err_quotes] = "Error!\nThe declartion of string must contains at beginning the quote char (\")";
	message[err_unfinished_string] = "Error!\nThe string isnt finished,strings must contains quotes char at the end (\").";
	message[err_ascii] = "Error!\nThe string must be filled only by Ascii characters.";

	message[err_out_of_line] = "Error!\nThe line size is at most 80.";
	message[err_binary_over_flew] = "Error!\nBinary file has memory storage of 256bits at most.";

	message[err_wrong_instruction] = "Error!\nWrong instruction. (%s)";
}

void error_type_clear(void)
{
	free(message);
}

void printing_format(char *, char *, int, char *, va_list);

void warning_out(char *filename, int line, enum error_type e, ...)
{
	va_list args;
	char *s = message[e];
	va_start(args, e);
	printing_format("warning", filename, line, s, args);
	va_end(args);
}

void error_out(char *filename, int line, enum error_type e, ...)
{
	va_list args;
	char *s = message[e];
	va_start(args, e);
	printing_format("error", filename, line, s, args);
	va_end(args);
}


void printing_format(char *prefix, char *filename, int line, char *s, va_list args)
{
	printf("%s: %s.am:%d - ", prefix, filename, line);
	vprintf(s, args);
	printf("\n");
}



char *copy2(char *s, size_t lim)
{
	char *n = (char *) verifed_malloc(sizeof(char) * (lim + 1));
	strncpy(n, s, lim);
	n[lim] = '\0';
	return n;
}

char *copy1(char *s)
{
	return copy2(s, strlen(s));
}

void switch_extension(char *s, char *ext)
{
	*strrchr(s, '.') = '\0';
	strcat(s, ext);
}

struct hashtable *operations, *directives, *registers;
enum paramamount *paramamounts;
int paramamountsize;

void operations_make(void)
{
	/*Definition*/

	operations = new_hashtable();
	hashtable_set_int(operations, "mov", MOV);
	hashtable_set_int(operations, "cmp", CMP);
	hashtable_set_int(operations, "add", ADD);
	hashtable_set_int(operations, "sub", SUB);
	hashtable_set_int(operations, "not", NOT);
	hashtable_set_int(operations, "clr", CLR);
	hashtable_set_int(operations, "lea", LEA);
	hashtable_set_int(operations, "inc", INC);
	hashtable_set_int(operations, "dec", DEC);
	hashtable_set_int(operations, "jmp", JMP);
	hashtable_set_int(operations, "bne", BNE);
	hashtable_set_int(operations, "red", RED);
	hashtable_set_int(operations, "prn", PRN);
	hashtable_set_int(operations, "jsr", JSR);
	hashtable_set_int(operations, "rts", RTS);
	hashtable_set_int(operations, "stop", STOP);

	/*Definition*/

	directives = new_hashtable();
	hashtable_set_int(directives, ".data", DIRECTIVE_DATA);
	hashtable_set_int(directives, ".string", DIRECTIVE_STRING);
	hashtable_set_int(directives, ".entry", DIRECTIVE_ENTRY);
	hashtable_set_int(directives, ".extern", DIRECTIVE_EXTERN);

	/*Definition*/

	registers = new_hashtable();
	hashtable_set_int(registers, "r0", REGISTER);
	hashtable_set_int(registers, "r1", REGISTER);
	hashtable_set_int(registers, "r2", REGISTER);
	hashtable_set_int(registers, "r3", REGISTER);
	hashtable_set_int(registers, "r4", REGISTER);
	hashtable_set_int(registers, "r5", REGISTER);
	hashtable_set_int(registers, "r6", REGISTER);
	hashtable_set_int(registers, "r7", REGISTER);

	/*Definition*/

	paramamountsize = operations->size + directives->size;
	paramamounts = (enum paramamount *) verifed_malloc(sizeof(*paramamounts) * paramamountsize);
	paramamounts[JMP] = PARAM_JUMP;
	paramamounts[BNE] = PARAM_JUMP;
	paramamounts[JSR] = PARAM_JUMP;
	paramamounts[MOV] = PARAM_TWO;
	paramamounts[CMP] = PARAM_TWO;
	paramamounts[ADD] = PARAM_TWO;
	paramamounts[SUB] = PARAM_TWO;
	paramamounts[LEA] = PARAM_TWO;
	paramamounts[NOT] = PARAM_SINGLE;
	paramamounts[CLR] = PARAM_SINGLE;
	paramamounts[INC] = PARAM_SINGLE;
	paramamounts[DEC] = PARAM_SINGLE;
	paramamounts[RED] = PARAM_SINGLE;
	paramamounts[PRN] = PARAM_SINGLE;
	paramamounts[RTS] = PARAM_NONE;
	paramamounts[STOP] = PARAM_NONE;
	paramamounts[DIRECTIVE_STRING] = PARAM_UNKNOWN;
	paramamounts[DIRECTIVE_DATA] = PARAM_LIST;
	paramamounts[DIRECTIVE_ENTRY] = PARAM_SINGLE;
	paramamounts[DIRECTIVE_EXTERN] = PARAM_SINGLE;
}

void operations_clear(void)
{
	hashtable_clear(operations);
	hashtable_clear(directives);
	hashtable_clear(registers);
	free(paramamounts);
}

enum operation opeations_get(char *op)
{
	int *code = hashtable_get_int(operations, op);
	if (code == NULL)
		code = hashtable_get_int(directives, op);
	if (code == NULL)
		code = hashtable_get_int(registers, op);
	return code == NULL ? UNKNOWN_SYMBOL : *code;
}

enum paramamount symbol_amount(enum operation op)
{
	if (op < 0 || op >= paramamountsize)
		return PARAM_UNKNOWN;
	return paramamounts[op];
}

int is_operation(char *s)
{
	return hashtable_get_int(operations, s) != NULL;
}

int is_directive(char *s)
{
	return hashtable_get_int(directives, s) != NULL;
}

int is_register(char *s)
{
	return hashtable_get_int(registers, s) != NULL;
}

int is_jmp_operation(enum operation operator)
{
	return operator == JMP || operator == BNE || operator == JSR;
}

int is_data_directive(enum operation operator)
{
	return operator == DIRECTIVE_DATA || operator == DIRECTIVE_STRING;
}


