#include <ctype.h>
#include <pwd.h>        /* for getpwent */
#include <sys/types.h>  /* for getpwent */
#include <stdio.h>      /* for cuserid */
#include <unistd.h>     /* for getlogin */
#include "calculator.h" /* for report_error */
#include "variables.h"

#define THE_VALUE 0
#define THE_STRUCTURE 2
#define HASH_LENGTH = 101

struct variable *them = NULL;
int contents = 0;

/* Hidden, internal functions */
static void * getvar_core (char * key, int all_or_nothing);

void initvar (void)
{
}

void delnvar (int i)
{
	int j;
	struct variable *cursor = them, *follower = NULL;

	for (j=0;j<i;++j) {
		follower = cursor;
		cursor = cursor->next;
	}
	if (! follower) {
		if (cursor) {
			them = cursor->next;
			free(cursor->key);
			free(cursor);
		} else {
			return;
		}
	} else {
		follower->next = cursor->next;
		free(cursor->key);
		free(cursor);
	}
	contents--;
}

double * getnvar (int i)
{
	return &(getrealnvar(i)->value);
}

struct variable * getrealnvar (int i)
{
	int j;
	struct variable *cursor = them;

	for (j=0;j<i;++j)
		cursor = cursor->next;

	return cursor;
}

double * getvar (char * key)
{
	return (double *) getvar_core(key, THE_VALUE);
}

struct variable * getvarptr (char *key)
{
	return (struct variable *) getvar_core(key, THE_STRUCTURE);
}

static void * getvar_core (char * key, int all_or_nothing)
{
	struct variable *cursor = them;

	if (! cursor) return NULL;
	
	while (cursor && cursor->key && strncmp(cursor->key, key, strlen(key))) {
		cursor = cursor->next;
	}
	if (cursor && cursor->key && ! strncmp(cursor->key, key, strlen(key))) {
		switch (all_or_nothing) {
			case THE_VALUE:
				return &(cursor->value);
			case THE_STRUCTURE:
				return cursor;
		}
	}
	return NULL;
}

int putvar (char * key, double value)
{
	struct variable *cursor = them;

	if (! key) return -1;

	if (cursor) {
		while (cursor && strncmp(cursor->key,key,strlen(key))>0 && cursor->next) {
			cursor = cursor->next;
		}

		if (strncmp(cursor->key,key,strlen(key))) { // add after cursor
			struct variable *ntemp = cursor->next;
			cursor->next = calloc(sizeof(struct variable),1);
			if (! cursor->next) { // if we can't allocate memory
				cursor->next = ntemp;
				return -1;
			}
			cursor = cursor->next;
			cursor->next = ntemp;
		} else { // change this one

		}
	} else {
		them = cursor = calloc(sizeof(struct variable),1);
	}

	if (cursor->key) {
//		if (cursor->value)
//			free(cursor->value);
		cursor->value = value;
		return 0;
	} else {
		contents++;
		cursor->key = strdup(key);
		cursor->value = value;
		return 0;
	}
}

int putvarc (char * keyvalue)
{
	char *key=keyvalue, *value;
	int retval;
	value = strchr(keyvalue,'=');
	if (value == NULL) return -1;
	*value = 0;
	++value;
	retval = putvar(key, strtod(value,NULL));
	--value;
	*value = '=';
	return retval;
}

