#include "Data.h"
#include "Utills.h"
#include <stdlib.h>
#include <string.h>

/* Hash value of the string key */
unsigned int hash_value(char *string_key)
{
	unsigned int value;
	for (value = 0; *string_key != '\0'; string_key++)
		value = *string_key + 31 * value;

	return value % HASHTABLE_CAPA;
}

struct hashtable *new_hashtable(void)
{
	int index;
	struct hashtable *m = (struct hashtable *) verifed_malloc(sizeof(*m));
	m->size = 0;
	for (index = 0; index < HASHTABLE_CAPA; index++)
		m->tab[index] = NULL;

	return m;
}

void hashtable_clear(struct hashtable *a)
{
	struct hashnode *n, *temp;
	int index;
	if (a == NULL)
		return;
	for (index = 0; index < HASHTABLE_CAPA; index++)
	 {
		n = a->tab[index];
		while (n != NULL) 
		{
			temp = n->next;
			free(n->string_key);
			free(n->value);
			free(n);
			n = temp;
		}
		a->tab[index] = NULL;
	}
	free(a);
}

struct hashnode *getnode(struct hashtable *a, char *string_key)
{
	
	struct hashnode *n;
	if (a == NULL)
		return NULL;
	for (n = a->tab[hash_value(string_key)]; n != NULL; n = n->next)
		if (strcmp(n->string_key, string_key) == 0)
			return n;
	return NULL;
}

int *hashtable_get_int(struct hashtable *a, char *string_key)
{
	struct hashnode *n;
	return a == NULL || (n = getnode(a, string_key)) == NULL ? NULL : (int *) n->value; /*Returns the value or null*/
}


char *hashtable_get_str(struct hashtable *m, char *string_key)
{
	struct hashnode *n;
	return m == NULL || (n = getnode(m, string_key)) == NULL ? NULL : (char *) n->value; /*Returns the value or null*/
}

/* prepares a new node and returns it to later have its value set */
struct hashnode *make_node(struct hashtable *m, char *string_key)
{
	struct hashnode *n;
	unsigned int hashval;
	if (m == NULL)
		return NULL;

	if ((n = getnode(m, string_key)) == NULL) 
	{
		n = (struct hashnode *) verifed_malloc(sizeof(*n));
		n->string_key = (char *) verifed_malloc(sizeof(char) * strlen(string_key));
		strcpy(n->string_key, string_key);
		hashval = hash_value(string_key);
		n->next = m->tab[hashval];
		m->tab[hashval] = n;
		m->size++;
	} else
		free(n->value);
	return n;
}

void *hashtable_set_int(struct hashtable *m, char *string_key, int value)
{
	struct hashnode *n;
	if (m == NULL || (n = make_node(m, string_key)) == NULL)
		return NULL;
	n->value = verifed_malloc(sizeof(value));
	*((int *) n->value) = value;
	return n->value;
}

void *hashtable_set_str(struct hashtable *m, char *string_key, char *value)
{
	struct hashnode *n;
	if (m == NULL || (n = make_node(m, string_key)) == NULL)
		return NULL;
	n->value = verifed_malloc(sizeof(char) * strlen(value));
	strcpy(n->value, value);
	return n->value;
}

void hashtable_add_bit(struct hashtable *m, char *string_key, int value)
{
	int *n;
	if (m == NULL)
		return;

	n = hashtable_get_int(m, string_key); 
	hashtable_set_int(m, string_key, n == NULL ? value : ((*n) | value)); /*Gets the string key value to int*/
}

struct listnode *linkedlist_create_node(void *value, void (*mfree)(void *))
{
	struct listnode *n = (struct listnode *) verifed_malloc(sizeof(*n));
	n->value = value;
	n->next = NULL;
	n->free = mfree;
	return n;
}

struct listnode *linkedlist_clear_next(struct listnode *n)
{
	struct listnode *tmp;
	if (n == NULL)
		return NULL;
	tmp = n->next;
	n->free(n->value);
	free(n);
	return tmp;
}

void linkedlist_clear(struct listnode *n)
{
	while (n != NULL)
		n = linkedlist_clear_next(n);
}

