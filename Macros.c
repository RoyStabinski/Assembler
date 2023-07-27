#include "Macros.h"
#include "Parser.h"
#include "Data.h"
#include "Utills.h"
#include <stdlib.h>
#include <string.h>


#define MCR "mcr"
#define ENDMCR "endmcr"


int macro_check(char *, int *, int *, char **); /* Returns if the macro is valid*/
int endmacro_check(char *, int *, int *); /* Returns if the end macro is valid */
char *macro_verifed_malloc(FILE *);/* Counting how many lines to endmcr */

FILE *Macros(FILE *as, char *filename)
{
	char line[MAX_LINE_LENGTH + 2];
	char *newline_ptr; /* In order to remove the last new line*/
	char *macro_name;
	char *macro_content; 
	int index;
	int counter; /* Gets from ignore_whitespace + count_characters */
	int line_offset; /* In order to print parts of the line */
	int in_macro = 0;
	struct hashtable *macros = new_hashtable();

	FILE *am;
	strcat(filename, ".am");
	am = open_file(filename, "w");

	while (fgets(line, MAX_LINE_LENGTH + 2, as) != NULL)
	 {
		line_offset = 0;
		index = 0;

		ignore_whitespace(line, &index);
		counter = count_characters(line, &index);

		if (line[index-counter] == ';' || counter == 0)
			continue;

		if (!in_macro)
			if (macro_check(line, &index, &counter, &macro_name))
			 {
				macro_content = macro_verifed_malloc(as);
				in_macro = 1;
			} else {
				/* While the line isnt empty */
				while (counter > 0)
				 {
					macro_name = copy2(&line[index-counter], counter);
					/* In case of the macro content doesnt represnt the macro name */
					if ((macro_content = hashtable_get_str(macros, macro_name)) != NULL && opeations_get(macro_name) == UNKNOWN_SYMBOL) 
					{
						line[index-counter] = '\0'; /* In order to print before the macro */
						fputs(&line[line_offset], am);
						fputs(macro_content, am); 
						line_offset = index; /* In order to print after the macro */
					}
					free(macro_name);
					if (ignore_whitespace(line, &index) == 0)
						break;
					counter = count_characters(line, &index);
				}

				fputs(&line[line_offset], am);
			}
		else
			if (endmacro_check(line, &index, &counter))
			 {
				in_macro = 0;
				newline_ptr = strrchr(macro_content, '\n');

				if (newline_ptr != NULL)
					*newline_ptr = '\0';

				hashtable_set_str(macros, macro_name, macro_content);
				free(macro_name); /* We can free it up because all the data already copied*/
				free(macro_content);
			} else
				strcat(macro_content, line);
	}

	hashtable_clear(macros);

	close_file(am);
	am = open_file(filename, "r");
	switch_extension(filename, "");

	return am;
}

char *macro_verifed_malloc(FILE *as)
{
	char line[MAX_LINE_LENGTH + 2]; 
	int index; 
	int	lines = 0; /* Counting how many lines before the end macro */
	int	counter = 0; /* Gets from ignore_whitespace + count_characters */
	fpos_t position; /* Saving the position */
	char *ptr; 

	fgetpos(as, &position);

	while (fgets(line, MAX_LINE_LENGTH + 2, as) != NULL)
	 {
		index = 0;
		ignore_whitespace(line, &index);
		counter = count_characters(line, &index);
		if (endmacro_check(line, &index, &counter))
			break;
		lines++;
	}

	fsetpos(as, &position);

	ptr = (char *) verifed_malloc(sizeof(char) * ((MAX_LINE_LENGTH + 1) * lines) + 1);
	*ptr = '\0';

	return ptr;
}

int macro_check(char *line, int *index, int *counter, char **macro_name)
{
	if (*counter != sizeof(MCR)-1 || strncmp(MCR, &line[(*index)-(*counter)], *counter) != 0)
		return 0;

	if (ignore_whitespace(line, index) == 0)
		return 0;
	
	if ((*counter = count_characters(line, index)) == 0)
		return 0;

	*macro_name = copy2(&line[(*index)-(*counter)], *counter);
	ignore_whitespace(line, index);
	
	return count_characters(line, index) == 0; /*Double check if content isnt missed*/
}

int endmacro_check(char *line, int *index, int *counter)
{
	if (*counter != sizeof(ENDMCR)-1 || strncmp(ENDMCR, &line[(*index)-(*counter)], *counter) != 0)
		return 0;
		
	ignore_whitespace(line, index);
	return count_characters(line, index) == 0;
}


