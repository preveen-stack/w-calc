#include <stdlib.h>
#include <stdio.h>
#include <math.h> /* for HUGE_VAL */
#include <string.h> /* for bzero */
#include "calculator.h"
#include "variables.h"

static double *stack = NULL;
static int stacksize = 0;
static int stacklast = -1;

double last_answer = 0;
char pretty_answer[200];
short standard_output = 1;
int precision = -1;
short engineering = 0;
short picky_variables = 0;
short use_radians = 1;

/*
 * These are declared here because they're not in any header files.
 * yyparse() is declared with an empty argument list so that it is
 * compatible with the generated C code from yacc/bison.
 * This part is taken from http://www.bgw.org/tutorials/programming/c/lex_yacc/main.c
 */
extern int yyparse();
extern int yy_scan_string(const char*);


double parseme (char * pthis)
{
	extern int synerrors;
	int stackcur;
	int len = strlen(pthis);
	char * sanitized;

	synerrors = 0;

	/* Sanitize the input */
	sanitized = calloc(sizeof(char),len+2);
	if (! sanitized) {
		perror("resizing buffer");
		return 0.00;
	}
	sprintf(sanitized,"%s\n",pthis);

	/* Hold my Place */
	stackcur = stacklast + 1;
	/* Evaluate the Expression
		* These two lines borrowed from:
		* http://www.bgw.org/tutorials/programming/c/lex_yacc/main.c
		* and are here primarily for readline suppport
		*/
	yy_scan_string(sanitized);
	yyparse();

	/* return success */
	free(sanitized);
	return last_answer;
}

void report_error (char * err)
{
	extern char * errstring;
	char * tempstring;

	if (errstring) {
		tempstring = calloc(strlen(errstring) + 1 + strlen(err),sizeof(char));
		sprintf(tempstring,"%s\n%s", errstring, err);
		free(errstring);
		errstring = tempstring;
	} else {
		errstring = strdup(err);
	}
}

void print_result (void) {
	if ((! stack) || (stacklast < 0)) return;

	last_answer = stack[stacklast];
	stacklast --;

	print_this_result(last_answer);
}

void print_this_result (double result)
{
	static char format[10];

/*	if (result != HUGE_VAL)
		sprintf(format,"%g",result);
	else
		sprintf(format,"Infinity");*/
//	putvar("a",result);

	if (precision > -1 && ! engineering) {
		sprintf(format, "%%1.%if", precision);
	} else if (precision > -1 && engineering) {
		sprintf(format, "%%1.%iE", precision);
	} else {
		sprintf(format,"%%G");
	}
	if (result != HUGE_VAL)
		sprintf(pretty_answer,format,result);
	else
		sprintf(pretty_answer,"Infinity");

	if (standard_output) {
		printf(" = %s\n",pretty_answer);
	}

	fflush(stdout);
}

void push_value (double in)
{
	/* Make sure the stack is big enough */
	if (! stack) {
		stack = calloc(sizeof(double),W_EXTRA);
		if (stack) {
			stacksize = W_EXTRA;
			stacklast = -1;
		} else {
			perror("allocating answer stack");
			return;
		}
	} else if (stacklast == stacksize-1) {
		double *temp;
		temp = realloc(stack, sizeof(double)*(stacksize+W_EXTRA));
		if (temp) {
			stack = temp;
			stacksize += W_EXTRA;
		} else {
			perror("allocating answer stack");
			return;
		}
	}

	stack[++stacklast] = in;
}

long fact (int in)
{
	static long *lookup = NULL;
	static int lookuplen = 0;
	if (in < 0) return 0;
	if (in == 0) return 1;
	if (lookup && lookuplen >= in && lookup[in-1])
		return lookup[in-1];
	else {
		if (! lookup) {
			lookup = calloc(sizeof(long),in);
			lookuplen = in;
			lookup[0] = 1;
		}
		if (in > 0) {
			if (lookuplen < in) {
				lookup = realloc(lookup,sizeof(long)*(in+1));
				bzero(lookup+lookuplen, sizeof(long)*(in+1-lookuplen));
			}
			lookup[in-1] = in * fact(in-1);
		} else
			return 1;
		return lookup[in-1];
	}
}
