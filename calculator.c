#ifndef GUI
#include "config.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <math.h>		/* for HUGE_VAL */
#include <float.h>		/* for DBL_EPSILON */
#include <inttypes.h>	/* for UINT32_MAX */
#include <ctype.h>      /* for isalpha() */
/* these are for kbw_rand() */
#if !STAT_MACROS_BROKEN
# include <sys/types.h>	/* for stat() and read() */
# include <sys/stat.h>	/* for stat() */
# include <fcntl.h>		/* for open() */
# include <sys/uio.h>	/* for read() */
# include <unistd.h>		/* for read() and close */
#else
#define kbw_rand random
#endif

#if STDC_HEADERS
# include <string.h>	/* for memset() */
#else
# if !HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
char *strchr(), *strrchr ();
#endif

#if TIME_WITH_SYS_TIME	/* for time() */
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include "calculator.h"
#include "variables.h"
#include "string_manip.h"
#include "files.h"

static double *stack = NULL;
static int stacksize = 0;
static int stacklast = -1;

/* variables everyone needs to get to */
double last_answer = 0;
char *pretty_answer = NULL;

/* communication with the parser */
char compute = 1;
unsigned int sig_figs = UINT32_MAX;

/* communication with the frontend */
char standard_output = 1;
char not_all_displayed = 0;

struct _conf conf;

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

char * flatten (char * str);

double parseme (char * pthis)
{
	extern int synerrors;
	short numbers = 0;
	int len = strlen(pthis);
	char * sanitized;
	extern char* open_file;

	synerrors = 0;
	sig_figs = UINT32_MAX;

	/* Sanitize the input (add a newline) */
	sanitized = calloc(sizeof(char),len+1);
	if (! sanitized) {
		perror("resizing buffer");
		return 0.00;
	}
	sprintf(sanitized,"%s\n",pthis);
	
	/* Convert to standard notation if there are numbers */
	// are there numbers?
	{
		unsigned int i;
		for (i=0;i<strlen(sanitized);++i) {
			if (isdigit((int)(sanitized[i]))) {
				numbers = 1;
				break;
			}
		}
	}
	if (numbers) {
		unsigned int i;
		for (i=0;i<strlen(sanitized);++i) {
			char errmsg[1000];
			if ((conf.thou_delimiter != '.' &&
					conf.dec_delimiter != '.' &&
					sanitized[i] == '.') ||
					(conf.thou_delimiter != ',' &&
					 conf.dec_delimiter != ',' &&
					 sanitized[i] == ',')) {
				// throw an error
				sprintf(errmsg,"Improperly formatted numbers! (%c,%c)\n",conf.thou_delimiter, conf.dec_delimiter);
				report_error(errmsg);
				synerrors = 1;
				break;
			} else if (sanitized[i] == conf.thou_delimiter)
				sanitized[i] = ',';
			else if (sanitized[i] == conf.dec_delimiter)
				sanitized[i] = '.';
			//		sanitized[i] = conf.charkey[(int)sanitized[i]];
		}
	}

	/* Now, eliminate recursion */
	sanitized = flatten(sanitized);
	
	/* Hold my Place */
//	stackcur = stacklast + 1;
	/* Evaluate the Expression
		* These two lines borrowed from:
		* http://www.bgw.org/tutorials/programming/c/lex_yacc/main.c
		* and are here strictly for readline suppport
		*/
	yy_scan_string(sanitized);
	yyparse();

	if (open_file) {
		char * filename = open_file;
		int retval;
		open_file = NULL;
		retval = loadState(filename);
		if (retval) {
			report_error("Could not load file.");
			report_error((char*)strerror(retval));
		}
	}
	/* return success */
	free(sanitized);
	return last_answer;
}

char * flatten (char * str)
{
	struct variable_list {
		char * varname;
		struct variable_list *next;
	} *vlist = NULL;
	char * curs = str, *eov, *nstr, *ncurs1, *ncurs2;
	char varname[500];
	int i, olen, nlen, changedlen;
	struct answer a;
	char standard_output_save = standard_output;

	standard_output = 0;

	if (*str == '\\') return str;
	curs = strchr(str,'=');
	if (! curs || ! *curs) curs = str;
	
	while (curs && *curs) {
		// search for the first letter of a possible variable
		while (curs && *curs && ! isalpha((int)(*curs))) {
			if (*curs == '\'') {
				curs++;
				while (curs && *curs && *curs != '\'') curs++;
			}
			curs++;
			// skip hex numbers
			if (*curs == 'x' && curs != str && *(curs-1) == '0') {
				curs++;
				while (curs && *curs && ( isdigit((int)(*curs)) ||
							(*curs >= 'a' && *curs <= 'f') ||
							(*curs >= 'A' && *curs <= 'F')))
					curs++;
			}
			// skip binary (not strictly necessary, since b is reserved, but just in case)
			if (*curs == 'b' && curs != str && *(curs-1) == '0')
				curs++;
		}
		if (! *curs) break;

		// pull out that variable
		eov = curs;
		i = 0;
		while (eov && *eov && (isalpha((int)(*eov)) || *eov == '_' || *eov == ':')) {
			varname[i++] = *eov;
			eov++;
		}
		if (i == 0) break;
		varname[i] = 0;
		olen = strlen(varname);

		// if it's a variable, evaluate it
		a = getvar_full(varname);
		if (! a.err) { // it is a var
			// is it in the varlist?
			struct variable_list *vcurs = vlist;
			while (vcurs) {
				if (strcmp(varname,vcurs->varname))
					break;
				vcurs = vcurs->next;
			}
			if (vcurs) {
				report_error("Variable recursion is not allowed.");
				return str;
			}
			// add it to the varlist
			vcurs = malloc(sizeof(struct variable_list));
			vcurs->varname = (char*)strdup(varname);
			vcurs->next = vlist;
			vlist = vcurs;
			if (a.exp) { // it is an expression
				double f = parseme(a.exp);
				sprintf(varname,"%1.15f",f);
			} else { // it is a value
				sprintf(varname,"%1.15f",a.val);
			}
		}
		nlen = strlen(varname);

		// now, put it back in the string
		changedlen = strlen(str) + nlen - olen + 1;
		nstr = malloc(changedlen);
		if (!nstr) { // not enough memory
			exit(1);
		}
		ncurs2 = nstr;
		ncurs1 = str;
		while (ncurs1 != curs) {
			*ncurs2 = *ncurs1;
			++ncurs1;
			++ncurs2;
		}
		ncurs1 = varname;
		while (ncurs1 && *ncurs1) {
			*ncurs2 = *ncurs1;
			++ncurs1;
			++ncurs2;
		}
		curs = ncurs2;
		ncurs1 = eov;
		while (ncurs1 && *ncurs1) {
			*ncurs2 = *ncurs1;
			++ncurs1;
			++ncurs2;
		}
		*ncurs2 = 0;
		free(str);
		str = nstr;
	}
	// free up the vlist
	while (vlist) {
		struct variable_list * freeme = vlist;
		free(vlist->varname);
		vlist = vlist->next;
		free(freeme);
	}
	standard_output = standard_output_save;
	return str;
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
		errstring = (char*)strdup(err);
	}
}

void print_result (void) {
	char * temp;
	if ((! stack) || (stacklast < 0)) return;

	last_answer = stack[stacklast];
	stacklast --;

	if (pretty_answer) free(pretty_answer);
	temp = print_this_result(last_answer);
	if (temp) pretty_answer = (char*)strdup(temp);
	else pretty_answer = NULL;
}

char *print_this_result (double result)
{
	static char format[10];
	static char *pa = NULL, *tmp;
	static char pa_dyn = 1;
	extern char *errstring;
	unsigned int decimal_places = 0;

	/* Build the "format" string, that will be used in an sprintf later */
	switch (conf.output_format) {
		case DECIMAL_FORMAT:
			if (pa_dyn) tmp = realloc(pa, sizeof(char)*310);
			else { tmp = pa = malloc(sizeof(char)*310); pa_dyn = 1; }
			if (! tmp) { free(pa); pa = "Not Enough Memory"; pa_dyn = 0; return pa; }
				else pa = tmp;
			if (conf.precision > -1 && ! conf.engineering) {
				sprintf(format, "%%1.%if", conf.precision);
				decimal_places = conf.precision;
			} else if (conf.precision > -1 && conf.engineering) {
				sprintf(format, "%%1.%iE", conf.precision);
				decimal_places = conf.precision;
			} else {
				sprintf(format,"%%G");
				if (fabs(result) < 10.0) {
					decimal_places = 6;
				} else if (fabs(result) < 100.0) {
					decimal_places = 4;
				} else if (fabs(result) < 1000.0) {
					decimal_places = 3;
				} else if (fabs(result) < 10000.0) {
					decimal_places = 2;
				} else if (fabs(result) < 100000.0) {
					decimal_places = 1;
				} else {
					decimal_places = 0;
				}
			}
			break;
		case OCTAL_FORMAT:
			if (pa_dyn) tmp = realloc(pa,sizeof(char)*14);
			else { tmp = pa = malloc(sizeof(char)*14); pa_dyn = 1; }
			if (! tmp) { free(pa); pa = "Not Enough Memory"; pa_dyn = 0; return pa; }
				else pa = tmp;
			sprintf(format,conf.print_prefixes?"%%#o":"%%o");
			break;
		case HEXADECIMAL_FORMAT:
			if (pa_dyn) tmp = realloc(pa,sizeof(char)*11);
			else { tmp = pa = malloc(sizeof(char)*11); pa_dyn = 1; }
			if (! tmp) { free(pa); pa = "Not Enough Memory"; pa_dyn = 0; return pa; }
				else pa = tmp;
			sprintf(format,conf.print_prefixes?"%%#x":"%%x");
			break;
		case BINARY_FORMAT:
			// Binary Format can't just use a format string, so
			// we have to handle it later
			if (pa_dyn) free(pa);
			pa = NULL;
			pa_dyn = 1;
			break;
	}

	if (result == HUGE_VAL) {
		// if it is infinity, print "Infinity", regardless of format
		if (pa_dyn) tmp = realloc(pa,sizeof(char)*11);
		else { tmp = pa = malloc(sizeof(char)*11); pa_dyn = 1; }
		if (! tmp) { free(pa); pa = "Not Enough Memory"; pa_dyn = 0; return pa; }
		else pa = tmp;
		sprintf(pa,"Infinity");
		not_all_displayed = 0;
	} else {
		switch (conf.output_format) {
			char * curs;
			case DECIMAL_FORMAT:
			{
				double junk;
				/* This is the big call */
				if (fabs(modf(result, &junk)) != 0.0 ||
						conf.engineering || ! conf.print_ints) {
					sprintf(pa,format,result);
				} else {
					sprintf(pa,"%1.0f",result);
				}
				/* was it as good for you as it was for me?
				 * now, you must localize it */
				{ unsigned int index;
					for (index=0;index<strlen(pa);++index) {
						if (pa[index] == '.')
							pa[index] = conf.dec_delimiter;
					}
				}
				switch (conf.rounding_indication) {
					case SIMPLE_ROUNDING_INDICATION:
						not_all_displayed = (modf(result*pow(10,decimal_places),&junk))?1:0;
						break;
					case SIG_FIG_ROUNDING_INDICATION:
						if (sig_figs < UINT32_MAX) {
							unsigned int t = count_digits(pa);
							if (pa[0] == '0') --t;
							else if (pa[0] == '-' && pa[1] == '0') --t;
							not_all_displayed = t != sig_figs;
						} else {
							not_all_displayed = 0;
						}
						break;
					default:
					case NO_ROUNDING_INDICATION:
						not_all_displayed = 0;
						break;
				}
			}
				break;
			case HEXADECIMAL_FORMAT:
				curs = pa+(conf.print_prefixes?2:0);
			case OCTAL_FORMAT:
				curs = pa+(conf.print_prefixes?1:0);
				{
					long int temp = result;
					unsigned int t = 0;
					sprintf(pa,format,temp);
					if (conf.rounding_indication == SIG_FIG_ROUNDING_INDICATION) {
						if (sig_figs < UINT32_MAX) {
							while (curs && *curs) {
								++t;
								++curs;
							}
							not_all_displayed = t != sig_figs;
						} else {
							not_all_displayed = 0;
						}
					} else {
						not_all_displayed = 0;
					}
				}
					break;
			case BINARY_FORMAT:
			{
				int i, place=-1;
				// if it is binary, format it, and print it
				// first, find the upper limit
				for (i=1;place==-1;++i) {
					if (result < pow(2.0,i))
						place = i-1;
				}
				pa = calloc(sizeof(char),(place+(conf.print_prefixes*2)+1));
				if (! pa) {
					pa = "Not Enough Memory";
					pa_dyn = 0;
					return pa;
				}
				if (conf.print_prefixes) {
					pa[0] = '0';
					pa[1] = 'b';
				}
				// print it
				for (i=conf.print_prefixes*2; place>=0; ++i) {
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

				if (sig_figs < UINT32_MAX) {
					if (conf.rounding_indication == SIG_FIG_ROUNDING_INDICATION) {
						not_all_displayed = count_digits(pa+(conf.print_prefixes?2:0)) != sig_figs;
					} else {
						not_all_displayed = 0;
					}
				} else {
					not_all_displayed = 0;
				}
			} // binary format
		} // switch
	} // if

	if (standard_output) {
		if (errstring && strlen(errstring)) {
                    extern int scanerror;
			fprintf(stderr,"%s\n",errstring);
			free(errstring);
			errstring = NULL;
                        scanerror = 0;
		}
		printf("%s%s\n",conf.print_equal?(not_all_displayed?" ~= ":" = "):(not_all_displayed?"~":""),pa);
	}

	return pa;
	
	fflush(stdout);
}

double simple_exp (double first, enum operations op, double second)
{
	if (compute) {
		double trash, temp;
		switch (op) {
			case wor:		temp = (first || second); break;
			case wand:		temp = (first && second); break;
			case wequal:	temp = (first == second); break;
			case wnequal:	temp = (first != second); break;
			case wgt:		temp = (first > second); break;
			case wlt:		temp = (first < second); break;
			case wlshft:    temp = ((int)first << (int)second); break;
			case wrshft:    temp = ((int)first >> (int)second); break;
			case wgeq:		temp = (first >= second); break;
			case wleq:		temp = (first <= second); break;
			case wplus:		temp = (first + second); break;
			case wminus:	temp = (first - second); break;
			case wmult:		temp = (first * second); break;
			case wdiv:		temp = ((second != 0)?(first/second):HUGE_VAL); break;
			case wmod:		temp = ((second != 0)?(fmod(first, second)):HUGE_VAL); break;
			case wexp:		temp = pow(first, second); break;
			case wbor:		temp = (int)first | (int)second; break;
			case wband:		temp = (int)first & (int)second; break;
			default:		temp = 0.0; break;
		}
		if (conf.precision_guard && fabs(modf(temp,&trash)) <= DBL_EPSILON) {
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
		switch (func) {
			case wsin:		temp = sin(conf.use_radians?input:(input*W_PI/180)); break;
			case wcos:		temp = cos(conf.use_radians?input:(input*W_PI/180)); break;
			case wtan:		temp = tan(conf.use_radians?input:(input*W_PI/180)); break;
			case wcot:      temp = 1.0/tan(conf.use_radians?input:(input*W_PI/180)); break;
			case wasin:		temp = asin(input)*(conf.use_radians?1:(180/W_PI)); break;
			case wacos:		temp = acos(input)*(conf.use_radians?1:(180/W_PI)); break;
			case watan:		temp = atan(input)*(conf.use_radians?1:(180/W_PI)); break;
			case wacot:     temp = atan(1/input)*(conf.use_radians?1:(180/W_PI)); break;
			case wsinh:		temp = sinh(input); break;
			case wcosh:		temp = cosh(input); break;
			case wtanh:		temp = tanh(input); break;
			case wcoth:     temp = 1/tanh(input); break;
			case wasinh:	temp = asinh(input); break;
			case wacosh:	temp = acosh(input); break;
			case watanh:	temp = atanh(input); break;
			case wacoth:    temp = atanh(1/input); break;
			case wlog:		temp = log10(input); break;
			case wlogtwo:	temp = log(input)/log(2); break;
			case wln:		temp = log(input); break;
			case wround:	temp = rint(input); break;
//			case wround:	temp = (fabs(floor(input)-input)>=0.5)?ceil(input):floor(input); break;
			case wneg:		temp = - input; break;
			case wnot:		temp = ! input; break;
			case wabs:		temp = fabs(input); break;
			case wsqrt:		temp = sqrt(input); break;
			case wfloor:	temp = floor(input); break;
			case wceil:		temp = ceil(input); break;
			case wrand:     temp = fmod(fabs(kbw_rand()),input) * ((input>=0)?1:-1); break;
			case wirand:    temp = (abs(kbw_int_rand()) % (int) input) * ((input>=0)?1:-1); break;
			case wcbrt:		temp = cbrt(input); break;
			default:		temp = input; break;
		}
		if (conf.precision_guard && fabs(modf(temp, &trash)) <= DBL_EPSILON) {
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
				memset(lookup+lookuplen, 0, sizeof(double)*(in+1-lookuplen));
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

char * output_string (unsigned int o)
{
	switch (o) {
		case HEXADECIMAL_FORMAT:
			return "hexadecimal format (0xf)";
		case OCTAL_FORMAT:
			return "octal format (08)";
		case BINARY_FORMAT:
			return "binary format (0b1)";
		case DECIMAL_FORMAT:
			return "decimal format (9)";
		default:
			return "error, unknown format";
	}
}

#if !STAT_MACROS_BROKEN
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

int kbw_int_rand (void)
{
	struct stat ex;
	int fd;
	
	if (! stat(RAND_FILE,&ex) && ex.st_ino) { // if could stat it and it has an inode
		fd = open(RAND_FILE, O_RDONLY);
		if (fd < 0) { // could not open it
			return seed_random && random();
		} else {
			int retval;
			int sizeread = read(fd,&retval,sizeof(int));
			close(fd); // I should check a return value here - but I wouldn't do much with it :-)
			if (sizeread != sizeof(int))
				return seed_random && random();
			else {
				return retval;
			}
		}
	} else {
		return seed_random && random();
	}
}
#endif

