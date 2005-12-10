#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <ctype.h>
//#include <pwd.h>                     /* for getpwent */
//#include <sys/types.h>                   /* for getpwent */
//#include <stdio.h>                       /* for cuserid */
//#include <unistd.h>                      /* for getlogin */
#if HAVE_STRING_H
# include <string.h>
#endif

#include <gmp.h>
#include <mpfr.h>

#include "calculator.h"				   /* for report_error */
#include "variables.h"

#define THE_VALUE 0
#define THE_STRUCTURE 2
#define THE_EXPRESSION 4
#define HASH_LENGTH = 101

struct variable *them = NULL;
int contents = 0;

/* Hidden, internal functions */
static void *getvar_core(char *key, int all_or_nothing);

void initvar(void)
{
}

void cleanupvar(void)
{
	struct variable *cursor = them;

	while (cursor != NULL) {
		free(cursor->key);
		if (cursor->exp == 1) {
			free(cursor->expression);
		} else {
			mpfr_clear(cursor->value);
		}
		cursor = cursor->next;
	}
}

void delnvar(int i)
{
	int j;
	struct variable *cursor = them, *follower = NULL;

	for (j = 0; j < i; ++j) {
		follower = cursor;
		cursor = cursor->next;
	}
	if (!follower) {
		if (cursor) {
			them = cursor->next;
			if (cursor->key) {
				free(cursor->key);
			}
			if (cursor->exp == 0) {
				mpfr_clear(cursor->value);
			} else {
				free(cursor->expression);
			}
			free(cursor);
		} else {
			return;
		}
	} else {
		follower->next = cursor->next;
		if (cursor->key) {
			free(cursor->key);
		}
		if (cursor->exp == 0) {
			mpfr_clear(cursor->value);
		} else {
			free(cursor->expression);
		}
		free(cursor);
	}
	contents--;
}

struct variable *getrealnvar(int i)
{
	int j;
	struct variable *cursor = them;

	for (j = 0; j < i; ++j)
		cursor = cursor->next;

	return cursor;
}

struct answer getvar(char *key)
{
	struct answer ans;
	mpfr_t *t = getvar_core(key, THE_VALUE);

	if (t) {
		mpfr_init_set(ans.val, *t, GMP_RNDN);
		ans.err = 0;
		ans.exp = NULL;
	} else {
		/* it's an error.
		 * if you access ans.val, you deserve what you get */
		ans.exp = NULL;
		ans.err = 1;
	}
	return ans;
}

void getvarval(mpfr_t out, char *key)
{
	mpfr_t *t = getvar_core(key, THE_VALUE);

	if (t) {
		mpfr_set(out, *t, GMP_RNDN);
	}
}

struct answer getvar_full(char *key)
{
	struct answer ans;
	struct variable *var;

	var = getvar_core(key, THE_STRUCTURE);
	if (var && !var->exp) {
		mpfr_init_set(ans.val, var->value, GMP_RNDN);
		ans.err = 0;
		ans.exp = NULL;
		ans.desc = var->description;
	} else if (var && var->exp) {
		ans.exp = var->expression;
		ans.desc = var->description;
		ans.err = 0;
	} else {
		ans.exp = NULL;
		ans.err = 1;
	}
	return ans;
}

struct variable *getvarptr(char *key)
{
	return (struct variable *)getvar_core(key, THE_STRUCTURE);
}

int varexists(char *key)
{
	struct variable *cursor = them;

	if (!cursor)
		return 0;
	if (!strlen(key))
		return 0;

	while (cursor && cursor->key &&
			strncmp(cursor->key, key, strlen(cursor->key)+1)) {
		cursor = cursor->next;
	}
	if (cursor && cursor->key && !strncmp(cursor->key, key, strlen(cursor->key)+1)) {
		return 1;
	} else {
		return 0;
	}
}

static void *getvar_core(char *key, int all_or_nothing)
{
	struct variable *cursor = them;

	if (!cursor)
		return NULL;
	if (!strlen(key))
		return NULL;

	while (cursor && cursor->key && strncmp(cursor->key, key, strlen(cursor->key)+1)) {
		cursor = cursor->next;
	}
	if (cursor && cursor->key && !strncmp(cursor->key, key, strlen(cursor->key)+1)) {
		switch (all_or_nothing) {
			case THE_VALUE:
				if (cursor->exp) {
					return NULL;
				} else {
					return &(cursor->value);
				}
			case THE_STRUCTURE:
				return cursor;
			case THE_EXPRESSION:
				return cursor->expression;
		}
	}
	return NULL;
}

int putexp(char *key, char *value, char* desc)
{
	struct variable *cursor = them;

	if (!key)
		return -1;

	if (cursor) {
		while (cursor && strncmp(cursor->key, key, strlen(cursor->key)+1) > 0 &&
			   cursor->next) {
			cursor = cursor->next;
		}

		if (strncmp(cursor->key, key, strlen(cursor->key)+1)) {	// add after cursor
			struct variable *ntemp = cursor->next;
			cursor->next = calloc(sizeof(struct variable), 1);
			if (!cursor->next) {	   // if we can't allocate memory
				cursor->next = ntemp;
				return -1;
			}
			cursor = cursor->next;
			cursor->next = ntemp;
		} else {					   // change this one

		}
	} else {
		them = cursor = calloc(sizeof(struct variable), 1);
	}

	if (cursor->key) {
		if (cursor->expression) {
			free(cursor->expression);
		} else {
			mpfr_clear(cursor->value);
		}
		if (cursor->description) {
			free(cursor->description);
		}
		cursor->expression = (char *)strdup(value);
		if (desc != NULL) {
			cursor->description = (char *)strdup(desc);
		} else {
			cursor->description = NULL;
		}
		cursor->exp = 1;
		return 0;
	} else {
		contents++;
		cursor->key = (char *)strdup(key);
		cursor->expression = (char *)strdup(value);
		if (desc != NULL) {
			cursor->description = (char *)strdup(desc);
		} else {
			cursor->description = NULL;
		}
		cursor->exp = 1;
		return 0;
	}
}

int putval(char *key, mpfr_t value, char* desc)
{
	struct variable *cursor = them, *temp;

	if (!key)
		return -1;

	if (cursor) {
		while (cursor && strncmp(cursor->key, key, strlen(cursor->key)+1) &&
			   cursor->next) {
			cursor = cursor->next;
		}

		if (!strncmp(cursor->key, key, strlen(cursor->key)+1)) {
			// change this one
		} else {
			// add after cursor
			temp = cursor->next;
			cursor->next = calloc(sizeof(struct variable), 1);
			if (!cursor->next) {	   // if we can't allocate memory
				return -1;
			}
			cursor = cursor->next;
			cursor->next = temp;
			mpfr_init(cursor->value);
			contents++;
		}
	} else {
		them = cursor = calloc(sizeof(struct variable), 1);
		mpfr_init(cursor->value);
		contents = 1;
	}
	/* by this point, cursor points to a fully allocated structure... it may,
	 * however, be missing a key value */

	if (!cursor->key) {
		cursor->key = (char *)strdup(key);
	}
	if (cursor->expression) {
		free(cursor->expression);
		cursor->expression = NULL;
	}
	if (cursor->description) {
		free(cursor->description);
	}
	if (desc != NULL) {
		cursor->description = (char *)strdup(desc);
	} else {
		cursor->description = NULL;
	}
	mpfr_set(cursor->value, value, GMP_RNDN);
	cursor->exp = 0;
	return 0;
}
