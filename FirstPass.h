/* 
	The file is responsible to do all first pass tasks and contains all required functions to do it.
	The main function here is the *first_pass function which is responsible for the file scanning
	and using other functions who doesnt metioned here to do it.
	 The function also uses program general functions which are not defined here but necessary here.

*/
#ifndef FIRSTPASS_H
#define FIRSTPASS_H

#include <stdio.h>
#include "Data.h"

FILE *first_pass(FILE *, char *, struct listnode *, struct listnode *, struct hashtable *, struct hashtable *);

#endif


