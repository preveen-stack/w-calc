#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* System Headers */
#if HAVE_STRING_H || !defined(HAVE_CONFIG_H)
# include <string.h>
#endif

/* Internal Headers */
#include "number.h"
#include "calculator.h"                /* for report_error */
#include "list.h"
#include "variables.h"
#include "output.h"

#define THE_VALUE      0
#define THE_STRUCTURE  2
#define THE_EXPRESSION 4
#define HASH_LENGTH    = 101

List them = NULL;

/* Injected Dependencies (Output Functions) */
static void (*display_var)(variable_t *v,
                           unsigned    count,
                           unsigned    digits);

/* Hidden, internal functions */
static void *getvar_core(const char *key,
                         const int   all_or_nothing);

void init_var(void (*dv)(variable_t *, unsigned, unsigned))
{                                      /*{{{ */
    display_var = dv;
}                                      /*}}} */

/* This function deletes all the variables and frees all the memory. */
void cleanupvar(void)
{                                      /*{{{ */
    variable_t *freeme;

    while ((freeme = (variable_t *)getHeadOfList(them)) != NULL) {
        free(freeme->key);
        if (freeme->exp == 1) {
            free(freeme->expression);
        } else {
            num_free(freeme->value);
        }
        if (freeme->description) {
            free(freeme->description);
        }
        free(freeme);
    }
}                                      /*}}} */

/* Returns the number of variables */
size_t numvars()
{                                      /*{{{ */
    return listLen(them);
}                                      /*}}} */

/* prints out all the variables */
void printvariables(void)
{   /*{{{*/
    ListIterator li     = NULL;
    variable_t  *cursor = NULL;
    unsigned     digits = 1;

    if (!them || (listLen(them) == 0)) {
        return;
    }

    {
        unsigned len = listLen(them);
        while (len > 9) {
            digits++;
            len /= 10;
        }
    }

    li     = getListIterator(them);
    cursor = (variable_t *)nextListElement(li);
    if (cursor != NULL) {
        unsigned count = 1;
        display_var(cursor, count++, digits);
        while ((cursor = (variable_t *)nextListElement(li)) != NULL) {
            display_var(cursor, count++, digits);
        }
    }
    freeListIterator(li);
} /*}}}*/

/* returns a list of variables (only the base array is malloc'd, the rest must
 * NOT be freed */
char **listvarnames(void)
{   /*{{{*/
    size_t       i;
    char       **ret    = calloc(listLen(them) + 1, sizeof(char *));
    ListIterator li     = NULL;
    variable_t  *cursor = NULL;

    if (!them || (listLen(them) == 0)) {
        return ret;
    }

    li = getListIterator(them);
    i  = 0;
    while ((cursor = (variable_t *)nextListElement(li)) != NULL) {
        ret[i++] = cursor->key;
    }
    freeListIterator(li);
    return ret;
/*}}}*/ }

void delnvar(const size_t i)
{                                      /*{{{ */
    variable_t *freeme;

    freeme = (variable_t *)getListElement(them, i);
    if (freeme) {
        free(freeme->key);
        if (freeme->exp == 1) {
            free(freeme->expression);
        } else {
            num_free(freeme->value);
        }
        free(freeme);
    }
}                                      /*}}} */

variable_t *getrealnvar(const size_t i)
{   /*{{{ */
    return (variable_t *)peekListElement(them, i);
}                                      /*}}} */

answer_t getvar(const char *key)
{   /*{{{ */
    answer_t ans;
    Number  *t = getvar_core(key, THE_VALUE);

    if (t) {
        num_init_set(ans.val, *t);
        ans.err = 0;
        ans.exp = NULL;
    } else {
        /* it's an error.
         * if you access ans.val, you deserve what you get */
        ans.exp = NULL;
        ans.err = 1;
    }
    return ans;
}                                      /*}}} */

void getvarval(Number      out,
               const char *key)
{                                      /*{{{ */
    Number *t = getvar_core(key, THE_VALUE);

    if (t) {
        num_set(out, *t);
    }
}                                      /*}}} */

answer_t getvar_full(const char *key)
{   /*{{{ */
    answer_t    ans;
    variable_t *var;

    var = getvar_core(key, THE_STRUCTURE);
    if (var && !var->exp) {
        num_init_set(ans.val, var->value);
        ans.err  = 0;
        ans.exp  = NULL;
        ans.desc = var->description;
    } else if (var && var->exp) {
        ans.exp  = var->expression;
        ans.desc = var->description;
        ans.err  = 0;
    } else {
        ans.exp = NULL;
        ans.err = 1;
    }
    return ans;
}                                      /*}}} */

/* This function returns 1 if a variable by that key has already been created
 * and returns 0 if a variable by that key has not been created yet */
int varexists(const char *key)
{                                      /*{{{ */
    variable_t  *cursor = NULL;
    ListIterator li     = NULL;

    if (!them || !strlen(key) || (listLen(them) == 0)) {
        return 0;
    }

    li = getListIterator(them);
    while ((cursor = (variable_t *)nextListElement(li)) != NULL) {
        if (!strncmp(cursor->key, key, strlen(cursor->key) + 1)) {
            break;
        } else {
            cursor = NULL;
        }
    }
    freeListIterator(li);
    return cursor ? 1 : 0;
}                                      /*}}} */

static void *getvar_core(const char *key,
                         const int   all_or_nothing)
{                                      /*{{{ */
    variable_t  *cursor = NULL;
    ListIterator li     = NULL;

    if (!them || (key == NULL) || (*key == 0) || (listLen(them) == 0)) {
        return NULL;
    }

    li = getListIterator(them);
    while ((cursor = (variable_t *)nextListElement(li)) != NULL) {
        if (!strncmp(cursor->key, key, strlen(cursor->key) + 1)) {
            break;
        } else {
            cursor = NULL;
        }
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
}                                      /*}}} */

/* this adds the key-expression pair to the list.
 * if the key already exists, change the value to this */
int putexp(const char *key,
           const char *value,
           const char *desc)
{                                      /*{{{ */
    variable_t *cursor = NULL;

    if (*key == 0) {
        return -1;
    }

    cursor = getvar_core(key, THE_STRUCTURE);
    if (!cursor) {
        // variable named "key" doesn't exist yet, so allocate memory
        cursor = calloc(sizeof(variable_t), 1);
        addToList(&them, cursor);
    }
    // by this point, we have a variable (cursor) in the list (them)
    if (cursor->key) {
        if (cursor->expression) {
            free(cursor->expression);
        } else {
            num_free(cursor->value);
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
}                                      /*}}} */

/* this adds the key-value pair to the list.
 * if the key already exists, change the value to this */
int putval(const char  *key,
           const Number value,
           const char  *desc)
{                                      /*{{{ */
    variable_t *cursor = NULL;

    if ((key == NULL) || (*key == 0)) {
        return -1;
    }

    cursor = getvar_core(key, THE_STRUCTURE);
    if (!cursor) {
        // variable named "key" doesn't exist yet, so allocate memory
        cursor = calloc(1, sizeof(variable_t));
        addToList(&them, cursor);
    }
    // by this point, we have a variable (cursor) in the list (them)
    // but key may not exist, and value may not be properly initialized
    if (!cursor->key) {
        cursor->key = (char *)strdup(key);
        num_init(cursor->value);
    } else if (cursor->expression) {
        free(cursor->expression);
        cursor->expression = NULL;
        num_init(cursor->value);
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
    num_set(cursor->value, value);
    return 0;
}                                      /*}}} */

/* vim:set expandtab: */
