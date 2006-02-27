#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <ctype.h>
#if HAVE_STRING_H || !defined(HAVE_CONFIG_H)
# include <string.h>
#endif

#include <gmp.h>
#include <mpfr.h>

#include "calculator.h"		       /* for report_error */
#include "list.h"
#include "variables.h"

#define THE_VALUE 0
#define THE_STRUCTURE 2
#define THE_EXPRESSION 4
#define HASH_LENGTH = 101

List them = NULL;

/* Hidden, internal functions */
static void *getvar_core(char *key, int all_or_nothing);

void initvar(void)
{				       /*{{{ */
}				       /*}}} */

/* This function deletes all the variables and frees all the memory. */
void cleanupvar(void)
{				       /*{{{ */
    struct variable *freeme;

    while ((freeme = (struct variable *)getHeadOfList(them)) != NULL) {
	free(freeme->key);
	if (freeme->exp == 1) {
	    free(freeme->expression);
	} else {
	    mpfr_clear(freeme->value);
	}
	if (freeme->description) {
	    free(freeme->description);
	}
	free(freeme);
    }
}				       /*}}} */

/* Returns the number of variables */
size_t numvars()
{				       /*{{{ */
    return listLen(them);
}				       /*}}} */

void delnvar(size_t i)
{				       /*{{{ */
    struct variable *freeme;

    freeme = (struct variable *)getListElement(them, i);
    if (freeme) {
	free(freeme->key);
	if (freeme->exp == 1) {
	    free(freeme->expression);
	} else {
	    mpfr_clear(freeme->value);
	}
	free(freeme);
    }
}				       /*}}} */

struct variable *getrealnvar(size_t i)
{				       /*{{{ */
    return (struct variable *)peekListElement(them, i);
}				       /*}}} */

struct answer getvar(char *key)
{				       /*{{{ */
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
}				       /*}}} */

void getvarval(mpfr_t out, char *key)
{				       /*{{{ */
    mpfr_t *t = getvar_core(key, THE_VALUE);

    if (t) {
	mpfr_set(out, *t, GMP_RNDN);
    }
}				       /*}}} */

struct answer getvar_full(char *key)
{				       /*{{{ */
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
}				       /*}}} */

struct variable *getvarptr(char *key)
{				       /*{{{ */
    return (struct variable *)getvar_core(key, THE_STRUCTURE);
}				       /*}}} */

/* This function returns 1 if a variable by that key has already been created
 * and returns 0 if a variable by that key has not been created yet */
int varexists(char *key)
{				       /*{{{ */
    struct variable *cursor = NULL;
    ListIterator li = NULL;

    if (!them || !strlen(key) || listLen(them) == 0)
	return 0;

    li = getListIterator(them);
    while ((cursor = (struct variable *)nextListElement(li)) != NULL) {
	if (!strncmp(cursor->key, key, strlen(cursor->key) + 1))
	    break;
	else
	    cursor = NULL;
    }
    freeListIterator(li);
    return cursor ? 1 : 0;
}				       /*}}} */

static void *getvar_core(char *key, int all_or_nothing)
{				       /*{{{ */
    struct variable *cursor = NULL;
    ListIterator li = NULL;

    if (!them || key == NULL || *key == 0 || listLen(them) == 0) {
	return NULL;
    }

    li = getListIterator(them);
    while ((cursor = (struct variable *)nextListElement(li)) != NULL) {
	if (!strncmp(cursor->key, key, strlen(cursor->key) + 1))
	    break;
	else
	    cursor = NULL;
    }
    freeListIterator(li);
    if (cursor) {
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
}				       /*}}} */

/* this adds the key-expression pair to the list.
 * if the key already exists, change the value to this */
int putexp(char *key, char *value, char *desc)
{				       /*{{{ */
    struct variable *cursor = NULL;

    if (*key == 0)
	return -1;

    cursor = getvar_core(key, THE_STRUCTURE);
    if (!cursor) {
	// variable named "key" doesn't exist yet, so allocate memory
	cursor = calloc(sizeof(struct variable), 1);
	addToList(&them, cursor);
    }
    // by this point, we have a variable (cursor) in the list (them)
    if (cursor->key) {
	if (cursor->expression) {
	    free(cursor->expression);
	} else {
	    mpfr_clear(cursor->value);
	}
	if (cursor->description) {
	    free(cursor->description);
	}
    } else {
	cursor->key = (char *)strdup(key);
    }
    // by this point, the cursor has been prepared to accept the exp/desc
    cursor->expression = (char *)strdup(value);
    if (desc != NULL) {
	cursor->description = (char *)strdup(desc);
    } else {
	cursor->description = NULL;
    }
    cursor->exp = 1;
    return 0;
}				       /*}}} */

/* this adds the key-value pair to the list.
 * if the key already exists, change the value to this */
int putval(char *key, mpfr_t value, char *desc)
{				       /*{{{ */
    struct variable *cursor = NULL;

    if (key == NULL || *key == 0) {
	return -1;
    }

    cursor = getvar_core(key, THE_STRUCTURE);
    if (!cursor) {
	// variable named "key" doesn't exist yet, so allocate memory
	cursor = calloc(1, sizeof(struct variable));
	addToList(&them, cursor);
    }
    // by this point, we have a variable (cursor) in the list (them)
    // but key may not exist, and value may not be properly initialized
    if (!cursor->key) {
	cursor->key = (char *)strdup(key);
	mpfr_init(cursor->value);
    } else if (cursor->expression) {
	free(cursor->expression);
	cursor->expression = NULL;
	mpfr_init(cursor->value);
    }
    if (cursor->description) {
	free(cursor->description);
    }
    cursor->exp = 0;
    // by this point, the variable has a key, and an initialized value,
    // and is ready to receive the correct value/desc
    if (desc != NULL) {
	cursor->description = (char *)strdup(desc);
    } else {
	cursor->description = NULL;
    }
    mpfr_set(cursor->value, value, GMP_RNDN);
    return 0;
}				       /*}}} */
