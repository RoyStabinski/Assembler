#include "Parser.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Utills.h"
/*Same as the language function "free"*/
void free_memory_instruction(void *index)
{
	free(((instruction *) index)->value);
	free(index);
}

/* Enum for the machine instruction structure*/
enum opbitlocations
 {
	bit0_ARE,
	bit1_ARE,
	bit2_destination_operand,
	bit3_destination_operand,
	bit4_source_operand,
	bit5_source_operand,
	bit6_opcode,
	bit7_opcode,
	bit8_opcode,
	bit9_opcode,
	bit10_parameter2,
	bit11_parameter2,
	bit12_parameter1,
	bit13_parameter1
};

unsigned int encoding_label(int val)
{
	int index = ENC_RELOCATABLE;
	index |= val << 2;
	return index;
}

void add_instruction_to_linkedlist(void *value, int islabel, int line, struct listnode **instruction_ptr, int *instruction_counter)
{
	instruction *inst = (instruction *) verifed_malloc(sizeof(*inst));
	inst->value = value;
	inst->islabel = islabel;
	inst->line = line;
	(*instruction_ptr)->next = linkedlist_create_node(inst, free_memory_instruction);
	*instruction_ptr = (*instruction_ptr)->next;
	(*instruction_counter)++;
}

/* Add num to the insruction list - by using encoding the number and check conditions. */
int addnumtoinstructions(char *n, struct listnode **instruction_ptr, int *instruction_counter)
{
	instruction *inst;
	if (!encoding_number(n, instruction_ptr, instruction_counter))
		return 0;

	inst = (instruction *) verifed_malloc(sizeof(*inst));
	inst->value = (word *) (*instruction_ptr)->value;
	inst->islabel = 0;
	inst->line = 0;
	((word *) inst->value)->field <<= 2;
	(*instruction_ptr)->value = inst;
	(*instruction_ptr)->free = free_memory_instruction;

	return 1;
}

enum parser_err_val encoding_operation(char *opname, enum operation opcode, int line, struct listnode **params, struct listnode **instruction_ptr, int *instruction_counter)
{
	struct listnode *param_ptr = *params;
	word *first = (word *) verifed_malloc(sizeof(*first)), *registers = NULL;
	int address_method = 0;
	int methods = 0;

	first->field = 0;
	add_instruction_to_linkedlist(first, 0, line, instruction_ptr, instruction_counter);

	
	first->field |= (opcode) << bit6_opcode;

	/* Checking for parameters*/
	if (is_jmp_operation(opcode))
	{
		if(!check_label((char *)param_ptr->value))
			return parser_wrong_label;

		first->field |= (param_ptr->next == NULL ? ADDRESS_DIRECT : ADDRESS_JUMP_WITH_PARAMS) << 2;
		add_instruction_to_linkedlist(copy1((char *) param_ptr->value), 1, line, instruction_ptr, instruction_counter);
		param_ptr = param_ptr->next;
	}
	while (param_ptr != NULL)
	 {
		address_method = which_address_method(param_ptr->value);
		methods <<= 2;
		methods |= address_method;
		if (registers != NULL && address_method != ADDRESS_DIRECT_REGISTER)
		 {
			add_instruction_to_linkedlist(registers, 0, line, instruction_ptr, instruction_counter);
			registers = NULL;
		}
		switch (address_method) /* Checking addressing methods cases*/
		{
		case ADDRESS_INSTANT:
			if (!addnumtoinstructions(&((char *) param_ptr->value)[1], instruction_ptr, instruction_counter))
			 {
				*params = param_ptr;
				return parser_wrong_num;
			}
			break;

		case ADDRESS_ERROR:
			*params = param_ptr;
			return parser_wrong_num;

		case ADDRESS_DIRECT:
			if(!check_label((char *)param_ptr->value))
				return parser_wrong_label;
			add_instruction_to_linkedlist(copy1(param_ptr->value), 1, line, instruction_ptr, instruction_counter);
			break;

		case ADDRESS_DIRECT_REGISTER:
			if (registers == NULL)
			 {
				registers = (word *) verifed_malloc(sizeof(*registers));
				registers->field = 0;
			}
			registers->field |= atoi(&((char *) param_ptr->value)[1]) << (param_ptr->next == NULL ? 2 : 8);
			break;

		default:
			break;
		}
		param_ptr = param_ptr->next;
	}
	if (registers != NULL)
		add_instruction_to_linkedlist(registers, 0, line, instruction_ptr, instruction_counter);

	first->field |= methods << (is_jmp_operation(opcode) ? bit10_parameter2 : bit2_destination_operand);

	switch (opcode)/*Checking operations cases*/
	 {
	case LEA:
		if (!(((first->field & (1 << bit5_source_operand)) >> bit5_source_operand) ^ ((first->field & (1 << bit4_source_operand)) >> bit4_source_operand)))
			return parser_wrong_source_param;

	case MOV:
	case ADD:
	case SUB:
	case NOT:
	case CLR:
	case INC:
	case DEC:
	case JMP:
	case BNE:
	case RED:
	case JSR:

		if (!(first->field & ((1 << bit3_destination_operand) | (1 << bit2_destination_operand))))
			return parser_wrong_destnation_param;
		break;

	default:
		break;
	}

	return parser_valid;
}

int encoding_number(char *s, struct listnode **data_ptr, int *data_counter)
{
	int negative;
	word *w;

	if (!check_num(s))
		return 0;

	negative = *s == '-';
	w = (word *) verifed_malloc(sizeof(*w));

	if (*s == '-' || *s == '+')
		s++;

	w->field = atoi(s) - negative;
	if (negative)
		w->field = ~w->field;

	(*data_ptr)->next = linkedlist_create_node(w, free);
	*data_ptr = (*data_ptr)->next;
	(*data_counter)++;

	return 1;
}

enum parser_err_val encoding_string(char *s, struct listnode **data_ptr, int *data_counter)
{
	struct listnode *temp;
	char *end;
	word *w;
	int index = 0;

	if (*s != '"')
		return parser_quotes;

	end = strrchr(s, '"') + 1;
	ignore_whitespace(end, &index);

	if (s == end-1 || count_characters(end, &index) > 0)
		return parser_unfinished_string;

	for (s++; s != end; s++)
	 {
		if (!isprint(*s))
			return parser_wrong_char;

		w = (word *) verifed_malloc(sizeof(*w));
		w->field = s == end-1 ? '\0' : *s; /* Terminate at EOF */
		temp = linkedlist_create_node(w, free);

		if (*data_ptr != NULL)
			(*data_ptr)->next = temp;
		*data_ptr = temp;

		(*data_counter)++;
	}
	return parser_valid;
}

enum parser_err_val parsing_parameters(char *line, int *index, int paramamount, struct listnode **head)
{
	int counter = 0;
	int j = 0;
	int jmp_with_parameters = 0;
	char *param;
	char *jump_to_end;
	struct listnode *n = NULL, *temp;

	if (paramamount == PARAM_UNKNOWN)
		return parser_valid;

	if (paramamount == PARAM_JUMP) 
	{
		if (strchr(&line[*index], '(') != NULL)
		 {
			jmp_with_parameters = 1;
			jump_to_end = strchr(&line[*index], ')');

			if (jump_to_end == NULL)
				return parser_invalid_jump;

			jump_to_end++;
			j = 0;
			ignore_whitespace(jump_to_end, &j);

			if (count_characters(jump_to_end, &j))
				return parser_invalid_jump;

			*(--jump_to_end) = '\0';
		} else {
			jmp_with_parameters = 0;
			paramamount = PARAM_SINGLE;
		}
	}
	while ((counter = count_characters(line, index)) > 0 && paramamount != 0)
	 {
		param = copy_to(&line[(*index)-counter], paramamount == PARAM_JUMP ? '(' : ',');
		*index -= counter;
		j = 0;

		if (count_characters(param, &j) == 0)
			return paramamount == PARAM_JUMP ? parser_invalid_jump : parser_unexpected_command;

		if ((counter = ignore_whitespace(param, &j)) > 0)
		 {
			if (jmp_with_parameters || count_characters(param, &j) > 0)
				return parser_unexpected_space;

			param[j-counter] = '\0';
		}
		temp = linkedlist_create_node(param, free);

		if (n != NULL)
			(n)->next = temp;

		n = temp;
		if (*head == NULL)
			*head = n;

		*index += j;
		if (line[*index] == (paramamount == PARAM_JUMP ? '(' : ','))
			(*index)++;
		else if (paramamount == PARAM_JUMP)
			return parser_invalid_jump;

		if (ignore_whitespace(line, index) > 0 && jmp_with_parameters)
			return parser_unexpected_space;

		paramamount--;
	}
	if (paramamount > 0)
		return parser_low_params;

	else if (paramamount == 0 && counter > 0)
		return parser_extra_params;

	return parser_valid;
}

enum addressmethod which_address_method(char *s)
{
	if (s[0] == '#')
		return check_num(&s[1]) ? ADDRESS_INSTANT : ADDRESS_ERROR;
	else if (is_register(s))
		return ADDRESS_DIRECT_REGISTER;

	return ADDRESS_DIRECT;
}

int check_num(char *s)
{
	if (*s != '-' && *s != '+' && !isdigit(*s))
		return 0;

	for (s++; *s != '\0' && *s != EOF; s++)
		if (!isdigit(*s))
			return 0;

	return 1;
}

int check_label(char *s)
{
	int counter;
	if (!isalpha(*s) || opeations_get(s) != UNKNOWN_SYMBOL)
		return 0;

	for (counter = 1, s++; *s != '\0' && *s != EOF; s++, counter++)
		if (!isalnum(*s))
			return 0;

	return counter < 31;
}

int check_space(char a)
{
	return a == ' ' || a == '\t';
}

int out_of_line(char a)
{
	return a == '\n' || a == '\0' || a == EOF;
}

int ignore_whitespace(char line[], int *index)
{
	int counter = 0;
	for (; check_space(line[*index]) && !out_of_line(line[*index]); (*index)++, counter++)
		;
	return counter;
}

int count_characters(char line[], int *index)
{
	int counter = 0;
	for (; !check_space(line[*index]) && !out_of_line(line[*index]); (*index)++, counter++)
		;
	return counter;
}

int counting__to(char *s, char a)
{
	int counter;
	for (counter = 0; s[counter] != a && !out_of_line(s[counter]); counter++)
		;
	return counter;
}

char *copy_to(char *s, char a)
{
	return copy2(s, counting__to(s, a));
}

