#ifndef KBW_VARIABLES
#define KBW_VARIABLES

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct variable {
	char * key;
	char * expression;
	double value;
	unsigned int exp:1;
	struct variable *next;
};

struct answer {
	double val;
	char * exp;
	unsigned int err:1;
};

struct answer getvar (char * key);
struct answer getvar_full (char * key);
struct answer getnvar (int);
struct variable * getrealnvar (int);
int putexp (char * key, char * value);
int putval (char * key, double value);
int putvarc (char * keyvalue);
void initvar (void);
void delnvar (int);

#endif

