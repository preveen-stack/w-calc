#ifndef KBW_VARIABLES
#define KBW_VARIABLES

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#if HAVE_STRING_H
# include <string.h>
#else
# if !HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
char *strchr(), *strrchr();
#endif
#include <stdlib.h>
#include <stdio.h>

#include "number.h"

struct variable
{
    char *key;
    char *expression;
    char *description;
    Number value;
    unsigned int exp:1;
    struct variable *next;
};

struct answer
{
    Number val;
    char *exp;
    char *desc;
    unsigned int err:1;
};

// requires a working Number
void getvarval(Number out, const char *key);

// requires a working Number
int putval(const char *key, const Number value, const char *desc);

int putexp(const char *key, const char *value, const char *desc);
int varexists(const char *key);
void initvar(void);
void delnvar(const size_t n);
void cleanupvar(void);
size_t numvars(void);
void printvariables(void);

// this returns a char ** that must be freed. DO NOT free its contents
char ** listvarnames(void);

/* ******************************
 * NOTE!
 * If you use these functions, they return
 * fully functioning Number structures that must be
 * num_free()'d when you're done with 'em.
 */

/* getvar returns only the value, or an error if it doesn't have one */
struct answer getvar(const char *key);

/* getvar returns whatever is known about the variable, or an error if
 * it doesn't exist */
struct answer getvar_full(const char *key);

/* THIS function, however, exposes the innards of the variable system.
 * do not under any circumstance un-initialize the Number
 */
struct variable *getrealnvar(const size_t n);

#endif
