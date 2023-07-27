#include "FirstPass.h"
#include "Parser.h"
#include "Utills.h"
#include <string.h>

FILE *first_pass(FILE *am, char *file_name, struct listnode *instructions, struct listnode *data, struct hashtable *labels, struct hashtable *labelattributes)
{
	char line[MAX_LINE_LENGTH + 2]; 
	char *label_name; 
	char *operation_name; 
	int index; 
	int counter; /* Gets from ignore_whitespace + count_characters */
	int label_defintion; 
	int paramamount; /* The amount of parameter at given line */
	int *label_attribute; 
	int line_counter = 0; 
	int data_counter = 0;
	int instruction_counter = 0; 
	int line_out_of_bounds = 0; /* If the line is longet then max size */
	int hash_err = 0; /* Errors in file */
	enum operation opcode; /* The opcode */
	struct listnode *params; 
	struct listnode *instructionptr = instructions; /* Final node pointer  */
	struct listnode *dataptr = data; /* Final node pointer */
	struct hashnode *attributes_ptr; 
	FILE *ob; 

	while (fgets(line, MAX_LINE_LENGTH + 2, am) != NULL)
	 {
		line_counter++;
		line_out_of_bounds = 0;

		while (strchr(line, '\n') == NULL && strlen(line) > MAX_LINE_LENGTH)
		 {
			if (!line_out_of_bounds) {
				error_out(file_name, line_counter, err_out_of_line);/*Print suitable error message*/
				line_out_of_bounds = 1;
				hash_err = 1;
			}
		
			if (fgets(line, MAX_LINE_LENGTH + 2, am) == NULL)
				break;
		}
		if (line_out_of_bounds)
			continue;

		index = 0;
		label_defintion = 0;
		label_name = NULL;
		params = NULL;

		ignore_whitespace(line, &index);
		counter = counting__to(&line[index], ':');

		/* In case of defined lable */
		if (line[index+counter] == ':') 
		{
			label_name = copy2(&line[index], counter);
			if (opeations_get(label_name) != UNKNOWN_SYMBOL) 
			{
				hash_err = 1;
				error_out(file_name, line_counter, err_label_symbol, label_name);
			} else if (!check_label(label_name))
			 {
				hash_err = 1;
				error_out(file_name, line_counter, err_invalid_label_name, label_name);
			} else
				label_defintion = 1;
			index += counter+1;
			ignore_whitespace(line, &index);
			counter = count_characters(line, &index);

			if (counter == 0)
				warning_out(file_name, line_counter, warning_empty_label, label_name);

		} else
			counter = count_characters(line, &index);

		if (counter == 0)
			continue;

		operation_name = copy2(&line[index-counter], counter);
		opcode = opeations_get(operation_name);
		ignore_whitespace(line, &index);

		paramamount = symbol_amount(opcode);
		switch (parsing_parameters(line, &index, paramamount, &params)) 
		{
		case parser_unexpected_space:
			hash_err = 1;
			error_out(file_name, line_counter, err_param_unexcpected_space);
			continue;

		case parser_unexpected_command:
			hash_err = 1;
			error_out(file_name, line_counter, err_param_unexcpected_command);
			continue;

		case parser_low_params:
			hash_err = 1;
			error_out(file_name, line_counter, paramamount == PARAM_JUMP ? err_param_jump : err_low_param, paramamount);
			continue;

		case parser_extra_params:
			hash_err = 1;
			error_out(file_name, line_counter, paramamount == PARAM_JUMP ? err_param_jump : err_extra_param, paramamount);
			continue;

		case parser_invalid_jump:
			hash_err = 1;
			error_out(file_name, line_counter, err_param_jump);
			continue;

		default:
			break;
		}

		/* Checks label defintion */
		if (label_defintion && (is_operation(operation_name) || is_data_directive(opcode)))
		 {
			label_attribute = hashtable_get_int(labelattributes, label_name);
			/* In case of the label is already defined before */
			if (hashtable_get_int(labels, label_name) != NULL || (label_attribute != NULL && *label_attribute & LABEL_EXTERNAL)) 
			{
				hash_err = 1;
				error_out(file_name, line_counter, err_label_defined, label_name);
			}
		}

		if (is_directive(operation_name))
		 {
			if (is_data_directive(opcode)) 
			{ 
				if (label_defintion)
				 {
					hashtable_set_int(labels, label_name, data_counter); /*Storing the label as data */
					hashtable_add_bit(labelattributes, label_name, LABEL_DATA);
				}

				if (opcode == DIRECTIVE_DATA) 
				{ 
					while (params != NULL) 
					{
						if (!encoding_number((char *) params->value, &dataptr, &data_counter))
						 {
							hash_err = 1;
							error_out(file_name, line_counter, err_wrong_data_num, (char *) params->value);
						}
						params = linkedlist_clear_next(params);
					}
				} else 
				{ 
					switch (encoding_string(&line[index], &dataptr, &data_counter)) 
					{
					case parser_quotes:
						hash_err = 1;
						error_out(file_name, line_counter, err_quotes);
						break;

					case parser_unfinished_string:
						hash_err = 1;
						error_out(file_name, line_counter, err_unfinished_string);
						break;

					case parser_wrong_char:
						hash_err = 1;
						error_out(file_name, line_counter, err_ascii);
						break;

					default:
						break;
					}
				}
			} else 
			{ 
				if (label_defintion)
				 {
					warning_out(file_name, line_counter, warning_unused_label, label_name);
					free(label_name);
					label_defintion = 0;
				}

				label_name = copy1((char *) params->value);
				label_attribute = hashtable_get_int(labelattributes, label_name);

				if (!check_label(label_name)) 
				{
					hash_err = 1;
					error_out(file_name, line_counter, err_invalid_label_name, label_name);

				/* Looking for entries and externals copies */
				} else if ((opcode == DIRECTIVE_ENTRY || hashtable_get_int(labels, label_name) == NULL) && (label_attribute == NULL || !(*label_attribute & (LABEL_ENTRY | LABEL_EXTERNAL))))
					hashtable_add_bit(labelattributes, label_name, (opcode == DIRECTIVE_EXTERN ? LABEL_EXTERNAL : LABEL_ENTRY));/*Storing according to condition*/
				else 
				{
					hash_err = 1;
					error_out(file_name, line_counter, err_label_defined, label_name);
				}
				free(label_name);
			}

		} else if (is_operation(operation_name))
		 {
			if (label_defintion) 
			{
				
				hashtable_set_int(labels, label_name, MEMORY_STORAGE_BEGIN + instruction_counter);
				hashtable_add_bit(labelattributes, label_name, LABEL_INSTRUCTION); /*Storing as instruction*/
			}

			switch (encoding_operation(operation_name, opcode, line_counter, &params, &instructionptr, &instruction_counter)) 
			{
			case parser_wrong_num:
				error_out(file_name, line_counter, err_wrong_data_num, (char *) params->value);
				break;

			case parser_wrong_label:
				error_out(file_name, line_counter, err_invalid_label_name, (char *) params->value);
				break;

			case parser_wrong_source_param:
				error_out(file_name, line_counter, err_opcode_wrong_source, operation_name);
				break;

			case parser_wrong_destnation_param:
				error_out(file_name, line_counter, err_opcode_wrong_dest, operation_name);
				break;

			default:
				break;
			}
		} else {
			hash_err = 1;
			error_out(file_name, line_counter, err_wrong_instruction, operation_name);/*Suitable message for undefined instruction*/
		}

		if (label_defintion)
			free(label_name);
		free(operation_name);
		linkedlist_clear(params);
	}

	/* Add the IC to data labels */
	for (index = 0; index < HASHTABLE_CAPA; index++) 
	{
		for (attributes_ptr = labelattributes->tab[index]; attributes_ptr != NULL; attributes_ptr = attributes_ptr->next)
		 {
			label_attribute = (int *) attributes_ptr->value;
			if (label_attribute != NULL && *label_attribute & LABEL_DATA)
				hashtable_set_int(labels, attributes_ptr->string_key, *hashtable_get_int(labels, attributes_ptr->string_key) + MEMORY_STORAGE_BEGIN + instruction_counter);
		}
	}

	/* Memory check */
	if (instruction_counter + data_counter > (MEMORY_STORAGE_END - MEMORY_STORAGE_BEGIN))
	 {
		hash_err = 1;
		error_out(file_name, line_counter, err_binary_over_flew); /*Suitable message in case of using to much memory*/
	}

	strcat(file_name, ".ob");
	ob = open_file(file_name, "w");
	switch_extension(file_name, "");

	fprintf(ob, "%d %d\n", instruction_counter, data_counter);
	if (hash_err) {
		close_file(ob);
		ob = NULL;
	}
	
	return ob;
}


