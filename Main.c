/*This is the main function of the project.
overhere is the build-up of the program and the program will run from here */

#include <stdio.h>
#include <string.h>
#include "Utills.h"
#include "Data.h"
#include "Macros.h"
#include "FirstPass.h"
#include "SecondPass.h"

int main(int argc, char **argv)
{
	int index; 
	char *file_name;
	FILE *as, *am, *ob;
	struct hashtable *labels, *labelattributes;
	struct listnode *instructions, *data;

	if (argc == 1) {
		printf("Error!\n There are no files.\n");

		return 1;
	}

	/* Setting up memory storage */
	operations_make();
	error_type_prepare();

	for (index = 1; index < argc; index++)
	 {
		file_name = verifed_malloc(sizeof(char) * (strlen(argv[index]) + 4 + 1)); /* Memory space for the metioned arguments + extra 1 to null terminate*/
		strcpy(file_name, argv[index]);
		strcat(file_name, ".as");
		as = open_file(file_name, "r");

		switch_extension(file_name, ""); /*Removing the extension*/
		am = Macros(as, file_name);
		close_file(as);

		labels = new_hashtable();
		labelattributes = new_hashtable();
		instructions = linkedlist_create_node(NULL, free); /* Garbage value*/
		data = linkedlist_create_node(NULL, free);

		ob = first_pass(am, file_name, instructions, data, labels, labelattributes);

		fseek(am, 0, SEEK_SET);

		if ((ob == NULL || second_pass(am, ob, file_name, instructions, data, labels, labelattributes)) && close_file(am) != EOF)
		 {
			strcat(file_name, ".ent");
			remove(file_name);
			switch_extension(file_name, ".ext");
			remove(file_name);
			switch_extension(file_name, ".ob");
			remove(file_name);
		}

		hashtable_clear(labels);
		hashtable_clear(labelattributes);
		linkedlist_clear(instructions);
		linkedlist_clear(data);
		free(file_name);
	}

	operations_clear();
	error_type_clear();
	return 0;
}


