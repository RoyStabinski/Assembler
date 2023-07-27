#ifndef SECONDPASS_H
#define SECONDPASS_H
#include <stdio.h>
#include "Data.h"

int second_pass(FILE *am, FILE *ob, char *filename, struct listnode *instructions, struct listnode *data, struct hashtable *labels, struct hashtable *labelattributes);

#endif

