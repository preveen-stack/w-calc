#include <stdlib.h>
#include <stdio.h>
#include <math.h> /* for HUGE_VAL */
#include <float.h> /* for DBL_EPSILON */
#include <string.h> /* for bzero() */
/* these are for kbw_rand() */
#include <sys/types.h> /* for stat() and read() */
#include <sys/stat.h>  /* for stat() */
#include <fcntl.h>     /* for open() */
#include <sys/uio.h>   /* for read() */
#include <unistd.h>    /* for read() and close */
#include <time.h>      /* for time() */


#include "calculator.h"
#include "variables.h"

static double *stack = NULL;
static int stacksize = 0;
static int stacklast = -1;

/* variables everyone needs to get to */
double last_answer = 0;
char *pretty_answer = NULL;

/* configuration variables */
short standard_output = 1;
int precision = -1;
short engineering = 0;
short picky_variables = 0;
short use_radians = 0;
short compute = 1;
short output_format = 0;
short print_prefixes = 0;
short use_commas = 0;

/*
 * These are declared here because they're not in any header files.
 * yyparse() is declared with an empty argument list so that it is
 * compatible with the generated C code from yacc/bison.
 * These two lines are taken from http://www.bgw.org/tutorials/programming/c/lex_yacc/main.c
 */
extern int yyparse();
extern int yy_scan_string(const char*);

/* declared here so other people don't mess with it */
static int seed_random (void);

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
	char * temp;
	if ((! stack) || (stacklast < 0)) return;

	last_answer = stack[stacklast];
	stacklast --;

	if (pretty_answer) free(pretty_answer);
	temp = print_this_result(last_answer);
	if (temp) pretty_answer = strdup(temp);
	else pretty_answer = NULL;
}

char *print_this_result (double result)
{
	static char format[10];
	static char *pa = NULL, *tmp;
	extern char *errstring;

	// Find the proper format
	switch (output_format) {
		case DECIMAL_FORMAT:
			tmp = realloc(pa, sizeof(char)*310);
			if (! tmp) { free(pa); pa = "Not Enough Memory"; return pa; } else pa = tmp;
			if (precision > -1 && ! engineering) {
				sprintf(format, "%%1.%if", precision);
			} else if (precision > -1 && engineering) {
				sprintf(format, "%%1.%iE", precision);
			} else {
				sprintf(format,"%%G");
			}
			break;
		case OCTAL_FORMAT:
			tmp = realloc(pa,sizeof(char)*14);
			if (! tmp) { free(pa); pa = "Not Enough Memory"; return pa; } else pa = tmp;
			sprintf(format,print_prefixes?"0%%o":"%%o");
			break;
		case HEXADECIMAL_FORMAT:
			tmp = realloc(pa,sizeof(char)*11);
			if (! tmp) { free(pa); pa = "Not Enough Memory"; return pa; } else pa = tmp;
			sprintf(format,print_prefixes?"0x%%x":"%%x");
			break;
		case BINARY_FORMAT:
			free(pa);
			pa = NULL;
			break;
	}

	if (result == HUGE_VAL) {
		// if it is infinity, print it, regardless of format
		tmp = realloc(pa,sizeof(char)*11);
		if (! tmp) { free(pa); pa = "Not Enough Memory"; return pa; } else pa = tmp;
		sprintf(pa,"Infinity");
	} else if (output_format == DECIMAL_FORMAT) {
		sprintf(pa,format,result);
	} else if (output_format != BINARY_FORMAT) {
		long int temp = result;
		sprintf(pa,format,temp);
	} else {
		int i, place=-1;
		// if it is binary, format it, and print it
		// first, find the upper limit
		for (i=1;place==-1;++i) {
			if (result < pow(2.0,i))
				place = i-1;
		}
		pa = malloc(sizeof(char)*(place+(print_prefixes*2)+1));
		if (! pa) {
			pa = "Not Enough Memory";
			return pa;
		}
		if (print_prefixes) {
			pa[0] = '0';
			pa[1] = 'b';
		}
		// print it
		for (i=print_prefixes*2; place>=0; ++i) {
			double t = pow(2.0,place);
			if (result >= t) {
				pa[i] = '1';
				result -= t;
			} else {
				pa[i] = '0';
			}
			--place;
		}
		pa[i+1] = 0;
	}

	// Internationalize it ;-)
	if (use_commas) {
		*strchr(pa,'.') = ',';
	}

	if (standard_output) {
		if (errstring && strlen(errstring)) {
			fprintf(stderr,"%s\n",errstring);
			free(errstring);
			errstring = NULL;
		}
		printf(" = %s\n",pa);
	}

	return pa;
	
	fflush(stdout);
}

double simple_exp (double first, enum operations op, double second)
{
	if (compute) {
		double trash, temp;
		extern short clamped;
		switch (op) {
			case wor:		temp = (first || second); break;
			case wand:		temp = (first && second); break;
			case wequal:	temp = (first == second); break;
			case wnequal:	temp = (first != second); break;
			case wgt:		temp = (first > second); break;
			case wlt:		temp = (first < second); break;
			case wgeq:		temp = (first >= second); break;
			case wleq:		temp = (first <= second); break;
			case wplus:		temp = (first + second); break;
			case wminus:	temp = (first - second); break;
			case wmult:		temp = (first * second); break;
			case wdiv:		temp = ((second != 0)?(first/second):HUGE_VAL); break;
			case wmod:		temp = ((second != 0)?(fmod(first, second)):HUGE_VAL); break;
			case wexp:		temp = pow(first, second); break;
			default:		temp = 0.0; break;
		}
		if (fabs(modf(temp,&trash)) <= DBL_EPSILON) {
			if (modf(temp,&trash)) clamped = 1;
			return trash;
		}
		return temp;
	} else {
		return 0.0;
	}
}

double uber_function (enum functions func, double input)
{
	if (compute) {
		double temp, trash;
		extern short clamped;
		switch (func) {
			case wsin:		temp = sin(use_radians?input:(input*W_PI/180)); break;
			case wcos:		temp = cos(use_radians?input:(input*W_PI/180)); break;
			case wtan:		temp = tan(use_radians?input:(input*W_PI/180)); break;
			case wasin:		temp = asin(use_radians?input:(input*W_PI/180)); break;
			case wacos:		temp = acos(use_radians?input:(input*W_PI/180)); break;
			case watan:		temp = atan(use_radians?input:(input*W_PI/180)); break;
			case wsinh:		temp = sinh(input); break;
			case wcosh:		temp = cosh(input); break;
			case wtanh:		temp = tanh(input); break;
			case wasinh:	temp = asinh(input); break;
			case wacosh:	temp = acosh(input); break;
			case watanh:	temp = atanh(input); break;
			case wlog:		temp = log10(input); break;
			case wln:		temp = log(input); break;
			case wround:	temp = (fabs(floor(input)-input)>=0.5)?ceil(input):floor(input); break;
			case wneg:		temp = - input; break;
			case wnot:		temp = ! input; break;
			case wabs:		temp = fabs(input); break;
			default:		temp = input; break;
		}
		if (fabs(modf(temp, &trash)) <= DBL_EPSILON) {
			if (modf(temp,&trash)) clamped = 1;
			return trash;
		}
		return temp;
	} else {
		return 0;
	}
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

double fact (int in)
{
	static double *lookup = NULL;
	static int lookuplen = 0;
	if (in < 0) return 0;
	if (in == 0) return 1;
	if (lookup && lookuplen >= in && lookup[in-1])
		return lookup[in-1];
	else {
		if (! lookup) {
			lookup = calloc(sizeof(double),in);
			lookuplen = in;
			lookup[0] = 1;
		}
		if (in > 0) {
			if (lookuplen < in) {
				lookup = realloc(lookup,sizeof(double)*(in+1));
				bzero(lookup+lookuplen, sizeof(double)*(in+1-lookuplen));
			}
			lookup[in-1] = in * fact(in-1);
		} else
			return 1;
		return lookup[in-1];
	}
}

static int seed_random (void)
{
	static char seeded = 0;
	if (! seeded) {
		srandom(time(NULL));
		seeded = 1;
	}
	return 1;
}

double kbw_rand (void)
{
	struct stat ex;
	int fd;
	
	if (! stat(RAND_FILE,&ex) && ex.st_ino) { // if could stat it and it has an inode
		fd = open(RAND_FILE, O_RDONLY);
		if (fd < 0) { // could not open it
			return seed_random && random();
		} else {
			double retval;
			int sizeread = read(fd,&retval,sizeof(double));
			close(fd); // I should check a return value here - but I wouldn't do much with it :-)
			if (sizeread != sizeof(double))
				return seed_random && random();
			else {
				return retval;
			}
		}
	} else {
		return seed_random && random();
	}
}

