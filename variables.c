#include "config.h"
#include <ctype.h>
#include <pwd.h>        /* for getpwent */
#include <sys/types.h>  /* for getpwent */
#include <stdio.h>      /* for cuserid */
#include <unistd.h>     /* for getlogin */
#include "calculator.h" /* for report_error */
#include "variables.h"

#define THE_VALUE 0
#define THE_STRUCTURE 2
#define THE_EXPRESSION 4
#define HASH_LENGTH = 101

struct variable *them = NULL;
int contents = 0;

/* Hidden, internal functions */
static void * getvar_core (char * key, int all_or_nothing);

void initvar (void)
{
	int i;
	for (i=0;i<256;++i) {
		conf.charkey[i] = i;
	}
	memcpy(conf.charunkey, conf.charkey, sizeof(conf.charkey));
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

struct answer getnvar_full (int i)
{
	struct variable * t = getrealnvar(i);
	struct answer ans;
	if (! t) {
		ans.val = 0.00;
		ans.err = 1;
		ans.exp = NULL;
	} else if (t->exp) {
		ans.val = 0.00;
		ans.exp = t->expression;
		ans.err = 0;
	} else {
		ans.val = t->value;
		ans.err = 0;
		ans.exp = NULL;
	}
	return ans;
}

struct answer getnvar (int i)
{
	struct variable * t = getrealnvar(i);
	struct answer ans;
	ans.exp = NULL;
	if (!t) {
		ans.val = 0.00;
		ans.err = 1;
	} else if (t->exp) {
		ans.val = 0.00;
		ans.err = 1;
	} else {
		ans.val = t->value;
		ans.err = 0;
	}
	return ans;
}

struct variable * getrealnvar (int i)
{
	int j;
	struct variable *cursor = them;

	for (j=0;j<i;++j)
		cursor = cursor->next;

	return cursor;
}

struct answer getvar (char * key)
{
	struct answer ans;
	double *t = getvar_core(key, THE_VALUE);
	if (t) {
		ans.val = *t;
		ans.err = 0;
		ans.exp = NULL;
	} else {
		ans.val = 0.00;
		ans.exp = NULL;
		ans.err = 1;
	}
	return ans;
}

struct answer getvar_full (char * key)
{
	struct answer ans;
	double *t = getvar_core(key, THE_VALUE);
	if (t) {
		ans.val = *t;
		ans.err = 0;
		ans.exp = NULL;
	} else {
		char * c = getvar_core(key, THE_EXPRESSION);
		ans.val = 0.00;
		if (c) {
			ans.exp = c;
			ans.err = 0;
		} else {
			ans.exp = NULL;
			ans.err = 1;
		}
	}
	return ans;
}

struct variable * getvarptr (char *key)
{
	return (struct variable *) getvar_core(key, THE_STRUCTURE);
}

static void * getvar_core (char * key, int all_or_nothing)
{
	struct variable *cursor = them;

	if (! cursor) return NULL;
	if (! strlen(key)) return NULL;
	
	while (cursor && cursor->key && strncmp(cursor->key, key, strlen(key))) {
		cursor = cursor->next;
	}
	if (cursor && cursor->key && ! strncmp(cursor->key, key, strlen(key))) {
		switch (all_or_nothing) {
			case THE_VALUE:
				if (cursor->exp) {
					return NULL;
				} else
					return &(cursor->value);
			case THE_STRUCTURE:
				return cursor;
			case THE_EXPRESSION:
				return cursor->expression;
		}
	}
	return NULL;
}

int putexp (char * key, char * value)
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
		if (cursor->expression)
			free(cursor->expression);
		cursor->expression = (char*)strdup(value);
		cursor->exp = 1;
		return 0;
	} else {
		contents++;
		cursor->key = (char*)strdup(key);
		cursor->expression = (char*)strdup(value);
		cursor->exp = 1;
		return 0;
	}
}

int putval (char * key, double value)
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
		cursor->expression = NULL;
		cursor->exp = 0;
		return 0;
	} else {
		contents++;
		cursor->key = (char*)strdup(key);
		cursor->value = value;
		cursor->expression = NULL;
		cursor->exp = 0;
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
	retval = putval(key, strtod(value,NULL));
	--value;
	*value = '=';
	return retval;
}

