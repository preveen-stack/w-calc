#ifndef KBW_VARIABLES
#define KBW_VARIABLES

#if STDC_HEADERS
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

struct variable
{
	char *key;
	char *expression;
	mpfr_t value;
	unsigned int exp:1;
	struct variable *next;
};

struct answer
{
	mpfr_t val;
	char *exp;
	unsigned int err:1;
};

/* ******************************
 * NOTE!
 * If you use these functions, they return
 * fully functioning mpfr_t structures that must be
 * mpfr_clear()'d when you're done with 'em.
 */

struct answer getvar(char *key);
struct answer getvar_full(char *key);
void getvarval(mpfr_t out, char *key);	// requires a working mpfr_t
int putexp(char *key, char *value);
int putval(char *key, mpfr_t value);
int putvarc(char *keyvalue);
void initvar(void);
void delnvar(int);

/* THIS function, however, exposes the innards of the variable system.
 * do not under any circumstance un-initialize the mpfr_t
 */
struct variable *getrealnvar(int);

#endif
