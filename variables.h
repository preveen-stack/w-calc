#ifndef KBW_VARIABLES
#define KBW_VARIABLES

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct variable {
	char * key;
	double value;
	struct variable *next;
};

double * getvar (char * key);
double * getnvar (int);
struct variable * getrealnvar (int);
int putvar (char * key, double value);
int putvarc (char * keyvalue);
void initvar (void);
void delnvar (int);

#endif