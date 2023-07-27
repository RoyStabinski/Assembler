/*The Data including 2 data structures that following us thourgh the project.
The first one is - Linked-list 
The Second one is - Hash-tables
*/

#ifndef DATA_H
#define DATA_H

#define HASHTABLE_CAPA 255 /*Capacity*/

struct hashnode {
	struct hashnode *next;
	char *string_key;
	void *value;
};

struct hashtable {
	struct hashnode *tab[HASHTABLE_CAPA];
	int size;
};

struct hashtable *new_hashtable(void);/* Creating a new hash table */
void hashtable_clear(struct hashtable *);/* Clearing the rest of the hash table */
int *hashtable_get_int(struct hashtable *, char *);/* Converting the value of the string key into integer */
char *hashtable_get_str(struct hashtable *, char *);/* Converting the value of the string key into string */
void *hashtable_set_int(struct hashtable *, char *, int);/* Converting the integer into string key */
void *hashtable_set_str(struct hashtable *, char *k, char *v);/* Converting the string into string key*/
void hashtable_add_bit(struct hashtable *, char *, int);/* Adding more field to hash table if needed */


struct listnode {
	struct listnode *next;
	void *value;
	void (*free)(void *);
};


struct listnode *linkedlist_create_node(void *, void (*)(void *)); /*Creating new linked list*/
struct listnode *linkedlist_clear_next(struct listnode *);/*Clear the node and returns the following one*/
void linkedlist_clear(struct listnode *);/*Clear the rest of the linked list*/


#endif

