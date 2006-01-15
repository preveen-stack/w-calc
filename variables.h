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

#include <gmp.h>
#include <mpfr.h>

struct variable
{
	char *key;
	char *expression;
	char *description;
	mpfr_t value;
	unsigned int exp:1;
	struct variable *next;
};

struct answer
{
	mpfr_t val;
	char *exp;
	char *desc;
	unsigned int err:1;
};

// requires a working mpfr_t
void getvarval(mpfr_t out, char *key);

// requires a working mpfr_t (duh)
int putval(char *key, mpfr_t value, char *desc);
int putexp(char *key, char *value, char *desc);
int varexists(char *key);
void initvar(void);
void delnvar(size_t);
void cleanupvar(void);
size_t numvars();

/* ******************************
 * NOTE!
 * If you use these functions, they return
 * fully functioning mpfr_t structures that must be
 * mpfr_clear()'d when you're done with 'em.
 */

/* getvar returns only the value, or an error if it doesn't have one */
struct answer getvar(char *key);

/* getvar returns whatever is known about the variable, or an error if
 * it doesn't exist */
struct answer getvar_full(char *key);

/* THIS function, however, exposes the innards of the variable system.
 * do not under any circumstance un-initialize the mpfr_t
 */
struct variable *getrealnvar(size_t);

#endif
