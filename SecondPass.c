#include "SecondPass.h"
#include <string.h>
#include "Parser.h"
#include "Utills.h"


void encoding(unsigned int num, FILE *ob);

int second_pass(FILE *am, FILE *ob, char *file_name, struct listnode *instructions, struct listnode *data, struct hashtable *labels, struct hashtable *labelattributes)
{
	struct listnode *list_ptr = instructions->next; 
	char *label_name; 
	char line[MAX_LINE_LENGTH + 2];
	char *op; /* Operation */
	int *label_attribute; 
	int *label_value; 
	int bin_counter = MEMORY_STORAGE_BEGIN; 
	int line_counter = 0; 
	int hash_err = 0; 
	int is_data = 0; 
	int has_external = 0; /* If the code has external*/
	int has_entry = 0; /* If the code has entries */
	int index; /* the current character in the current line */
	int counter; /* Gets from ignore_whitespace + count_characters  */
	FILE *ent, *ext; /* files */
	instruction *inst; 

	strcat(file_name, ".ent");
	ent = open_file(file_name, "w");
	switch_extension(file_name, ".ext");
	ext = open_file(file_name, "w");
	switch_extension(file_name, "");

	
	while (fgets(line, MAX_LINE_LENGTH + 2, am)) {/*while valid*/
		line_counter++;
		index = 0;


		if ((op = strstr(line, ".entry")) == NULL) /*If not entry file*/
			continue;
			continue;


		count_characters(op, &index);
		ignore_whitespace(op, &index);


		counter = count_characters(op, &index);
		label_name = copy2(&op[index-counter], counter);
		label_attribute = hashtable_get_int(labelattributes, label_name); /*Copies the label and his attribute*/


		if (label_attribute != NULL && *label_attribute & LABEL_ENTRY) {

			if (*label_attribute & (LABEL_DATA | LABEL_INSTRUCTION)) {
				has_entry = 1;
				fprintf(ent,"%s\t%d\n", label_name, *hashtable_get_int(labels, label_name));
			} else {
				hash_err = 1;
				error_out(file_name, line_counter, err_undefined_label, label_name);
			}
		}

		free(label_name);
	}


	line_counter = 0;
	while (list_ptr != NULL) {
		
		fprintf(ob, "0%d\t", bin_counter);/*Print the counter*/


		if (!is_data) {
			label_name = NULL;
			inst = list_ptr->value; /*If pointing to insturction*/


			if (inst->line)
				line_counter = inst->line;


			if (inst->islabel) {

				label_name = (char *) inst->value;
				label_attribute = hashtable_get_int(labelattributes, label_name);
				label_value = hashtable_get_int(labels, label_name);/*Copies the label and his attribute*/

				if (label_value == NULL && (label_attribute == NULL || !(*label_attribute & LABEL_EXTERNAL))) {
					hash_err = 1;
					error_out(file_name, line_counter, err_undefined_label, label_name);
				}

				if (label_attribute != NULL) {
					if (*label_attribute & LABEL_EXTERNAL) {
						encoding(ENC_EXTERNAL, ob);
						has_external = 1;
						fprintf(ext,"%s\t%d\n", label_name, bin_counter);
					} else
						encoding(encoding_label(*label_value), ob);
				}
			} else /* So this is a word */
				encoding(((word *)inst->value)->field, ob);

			/* If the instructions are done */
			if (!list_ptr->next) {
				is_data = 1;
				list_ptr = data;
			}
		} else /* So this is data */
			encoding(((word *) list_ptr->value)->field, ob);

		fputc('\n', ob);

		bin_counter++;
		list_ptr = list_ptr->next;
	}

	close_file(ob);
	close_file(ent);
	close_file(ext);

	if (!has_entry) {
		strcat(file_name, ".ent");
		remove(file_name);
		switch_extension(file_name, "");
	}

	if (!has_external) {
		strcat(file_name, ".ext");
		remove(file_name);
		switch_extension(file_name, "");
	}

	return hash_err;
}

void encoding(unsigned int num, FILE *ob)
{
	int index;
	for (index = 1 << (WORD_LENGTH-1); index > 0; index /= 2)
		fputc((num & index) ? '/' : '.', ob);
}

