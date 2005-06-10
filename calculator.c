#ifndef GUI
#include "config.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <math.h>					   /* for HUGE_VAL */
#include <float.h>					   /* for DBL_EPSILON */
#include <inttypes.h>				   /* for UINT32_MAX */
#include <ctype.h>					   /* for isalpha() */

#if STDC_HEADERS
# include <string.h>				   /* for memset() */
#else
# if !HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
char *strchr(), *strrchr();
#endif

#if TIME_WITH_SYS_TIME				   /* for time() */
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <gmp.h>
#include <mpfr.h>

#include "calculator.h"
#include "variables.h"
#include "string_manip.h"
#include "files.h"

/* variables everyone needs to get to */
mpfr_t last_answer;
char *pretty_answer = NULL;
gmp_randstate_t randstate;

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
extern int yy_scan_string(const char *);

static size_t num_to_str_complex(char *str, mpfr_t num, int base, int engr, int prec,
						  int prefix);
#define num_to_str(x,y) num_to_str_complex(x,y,10,0,0,1)
struct variable_list
{
	char *varname;
	struct variable_list *next;
};

static struct variable_list *extract_vars(char *str);
static int recursion(char *str);
static int find_recursion(struct variable_list *vstack);
static char *flatten(char *str);

void parseme(mpfr_t output, char *pthis)
{
	extern int synerrors;
	short numbers = 0;
	char *sanitized;
	extern char *open_file;

	synerrors = 0;
	compute = 1;
	sig_figs = UINT32_MAX;

	sanitized = strdup(pthis);

	/* Convert to standard notation (american comma and period) if there are
	 * numbers */
	// are there numbers?
	{
		unsigned int i;

		for (i = 0; i < strlen(sanitized); ++i) {
			if (isdigit((int)(sanitized[i]))) {
				numbers = 1;
				break;
			}
		}
	}
	if (numbers) {
		unsigned int i;

		for (i = 0; i < strlen(sanitized); ++i) {
			char errmsg[1000];

			if ((conf.thou_delimiter != '.' && conf.dec_delimiter != '.' &&
				 sanitized[i] == '.') || (conf.thou_delimiter != ',' &&
										  conf.dec_delimiter != ',' &&
										  sanitized[i] == ',')) {
				// throw an error
				sprintf(errmsg, "Improperly formatted numbers! (%c,%c)\n",
						conf.thou_delimiter, conf.dec_delimiter);
				report_error(errmsg);
				synerrors = 1;
				break;
			} else if (sanitized[i] == conf.thou_delimiter)
				sanitized[i] = ',';
			else if (sanitized[i] == conf.dec_delimiter)
				sanitized[i] = '.';
			//      sanitized[i] = conf.charkey[(int)sanitized[i]];
		}
	}

	/* Now, check for recursion */
	if (recursion(sanitized)) {
		goto exiting;
	}

	/* now resolve the variables */
	sanitized = flatten(sanitized);
	Dprintf("flattened: '%s'\n", sanitized);

	/* Sanitize the input (add a newline) */
	{
		char *temp;
		temp = calloc(sizeof(char), strlen(sanitized) + 3);
		if (!temp) {
			perror("resizing buffer");
			goto exiting;
		}
		sprintf(temp, "%s\n", sanitized);
		free(sanitized);
		sanitized = temp;
	}


	/* Hold my Place */
//  stackcur = stacklast + 1;
	/* Evaluate the Expression
	 * These two lines borrowed from:
	 * http://www.bgw.org/tutorials/programming/c/lex_yacc/main.c
	 * and are here strictly for readline suppport
	 */
	Dprintf("scanning string\n");
	yy_scan_string(sanitized);
	Dprintf("yyparse\n");
	yyparse();
	Dprintf("done yyparse\n");

	if (open_file) {
		char *filename = open_file;
		int retval;

		open_file = NULL;
		Dprintf("open_file\n");
		retval = loadState(filename);
		if (retval) {
			report_error("Could not load file.");
			report_error((char *)strerror(retval));
		}
	}
  exiting:
	/* exiting */
	free(sanitized);
	return;
}

/* this function takes a number (mpfr_t) and prints it.
 * This is a blatant ripoff of mpfr's mpfr_out_str(), because it formats things
 * (sorta) the way I want them formatted, though this prints things out to a
 * string, and does all the fancy presentation stuff we've come to expect from
 * wcalc.
 */
size_t num_to_str_complex(char *str, mpfr_t num, int base, int engr, int prec,
						  int prefix)
{
	char *s, *s0, *curs;
	size_t l;
	mp_exp_t e;

	if (mpfr_nan_p(num)) {
		sprintf(str, "@NaN@");
		return 3;
	}
	if (mpfr_inf_p(num)) {
		if (mpfr_sgn(num) > 0) {
			sprintf(str, "@Inf@");
			return 3;
		} else {
			sprintf(str, "-@Inf@");
			return 4;
		}
	}
	if (mpfr_zero_p(num)) {
		if (mpfr_sgn(num) > 0) {
			sprintf(str, "0");
			return 1;
		} else {
			sprintf(str, "-0");
			return 2;
		}
	}

	s = mpfr_get_str(NULL, &e, base, 0, num, GMP_RNDN);
	if (prec > -1) {
		if (engr == 0) {
			s = mpfr_get_str(s, &e, base, e + prec, num, GMP_RNDN);
		} else {
			s = mpfr_get_str(s, &e, base, 1 + prec, num, GMP_RNDN);
		}
	}
	s0 = s;
	/* for num = 3.1416 we have s = "31416" and e = 1 */

	l = strlen(s) + 1;				   /* size of allocated block returned by mpfr_get_str
									    * - may be incorrect, as only an upper bound? */
	curs = str;
	if (*s == '-') {
		sprintf(curs++, "%c", *s++);
	}

	if (prefix) {
		switch (base) {
			case 16:
				sprintf(curs, "0x");
				curs += 2;
				break;
			case 10:
				break;
			case 8:
				sprintf(curs++, "0");
				break;
			case 2:
				sprintf(curs, "0b");
				curs += 2;
				break;
		}
	}

	/* outputs mantissa */
	if (e > 0) {
		sprintf(curs++, "%c", *s++);
		e--;						   /* leading digit */
		if (engr == 0) {
			while (e > 0) {
				sprintf(curs++, "%c", *s++);
				e--;				   /* leading digits */
			}
		}
	} else {
		sprintf(curs++, "0");
	}
	sprintf(curs++, ".");			   /* decimal point */
	while (e < 0) {
		sprintf(curs++, "0");
		e++;
	}
	curs += sprintf(curs, "%s", s);	   /* the rest of the mantissa */
	mpfr_free_str(s0);
	/* strip off trailing 0's */
	if (prec == -1) {
		curs--;
		while ('0' == *curs) {
			*curs-- = '\0';
			l--;
		}
		if ('.' == *curs) {
			*curs = '\0';
			l--;
		}
		curs++;
	}
	if (e > 0) {
		l += sprintf(curs, (base <= 10 ? "e%ld" : "@%ld"), (long)e);
	}

	return l;
}

static struct variable_list *extract_vars(char *str)
{
	char *curs, *eov, save_char;
	struct variable_list *vlist = NULL, *vcurs;
	char *varname;

	if (*str == '\\')
		return NULL;
	curs = strchr(str, '=');
	if (!curs || !*curs)
		curs = str;

	while (curs && *curs) {
		// search for the first letter of a possible variable
		while (curs && *curs && !isalpha((int)(*curs))) {
			if (*curs == '\'')
				return NULL;
			curs++;
			// skip hex numbers
			if ((*curs == 'x' || *curs == 'X') && curs != str &&
				*(curs - 1) == '0') {
				curs++;
				while (curs && *curs &&
					   (isdigit((int)(*curs)) ||
						(*curs >= 'a' && *curs <= 'f') || (*curs >= 'A' &&
														   *curs <= 'F'))) {
					curs++;
				}
			}
			// skip binary (not strictly necessary, since b is reserved, but just in case)
			if (*curs == 'b' && curs != str && *(curs - 1) == '0')
				curs++;
		}

		// if we didn't find any, we're done looking
		if (!*curs)
			break;

		// if we did find something, pull out the variable name
		eov = curs;
		while (eov && *eov &&
			   (isalpha((int)(*eov)) || *eov == '_' || *eov == ':' ||
				isdigit((int)(*eov)))) {
			eov++;
		}
		save_char = *eov;
		*eov = 0;
		varname = strdup(curs);
		*eov = save_char;
		curs = eov;

		// add it to the varlist
		vcurs = malloc(sizeof(struct variable_list));
		vcurs->varname = varname;
		vcurs->next = vlist;
		vlist = vcurs;
	}
	return vlist;
}

static char *flatten(char *str)
{
	struct variable_list *vlist = NULL;
	char *curs = str, *eov, *nstr;
	char varname[500];
	int i, olen, nlen, changedlen;
	struct answer a;
	char standard_output_save = standard_output;

	standard_output = 0;

	if (*str == '\\') {
		standard_output = standard_output_save;
		return str;
	}
	curs = strchr(str, '=');
	if (!curs || !*curs)
		curs = str;

	while (curs && *curs) {
		// search for the fist letter of a possible variable
		while (curs && *curs && !isalpha((int)(*curs))) {
			if (*curs == '\\') {
				curs++;
				while (curs && *curs && isalpha((int)(*curs)))
					curs++;
			}
			if (*curs == '\'') {
				curs++;
				while (curs && *curs && *curs != '\'')
					curs++;
			}
			curs++;
		}
		if (!curs || !*curs)
			break;

		// pull out that variable
		eov = curs;
		i = 0;
		while (eov && *eov &&
			   (isalpha((int)(*eov)) || *eov == '_' || *eov == ':' ||
				isdigit((int)(*eov)))) {
			varname[i++] = *eov;
			eov++;
		}
		if (i == 0)
			break;
		varname[i] = 0;
		olen = strlen(varname);

		// if it's a variable, evaluate it
		a = getvar_full(varname);
		if (!a.err) {				   // it is a var
			if (a.exp) {			   // it is an expression
				mpfr_t f;

				mpfr_init(f);
				parseme(f, a.exp);
				num_to_str(varname, f);
				mpfr_clear(f);
			} else {				   // it is a value
				num_to_str(varname, a.val);
			}
		}
		nlen = strlen(varname);

		// now, put it back in the string
		// it is a var, and needs parenthesis
		changedlen = strlen(str) + nlen - olen + 1;
		if (!a.err)
			changedlen += 2;		   // space for parens if it's a variable

		nstr = malloc(changedlen);
		if (!nstr) {				   // not enough memory
			perror("flatten: ");
			exit(1);
		}
		{
			char *fromstring = str;
			char *tostring = nstr;

			// nstr is the new string, str is the input string
			tostring = nstr;
			while (fromstring != curs) {	// copy up to the curs (the beginning of the var name)
				*tostring = *fromstring;
				++fromstring;
				++tostring;
			}
			if (!a.err) {
				*tostring = '(';
				++tostring;
			}
			fromstring = varname;
			while (fromstring && *fromstring) {
				*tostring = *fromstring;
				++fromstring;
				++tostring;
			}
			if (!a.err) {
				*tostring = ')';
				++tostring;
			}
			curs = tostring;
			fromstring = eov;
			while (fromstring && *fromstring) {
				*tostring = *fromstring;
				++fromstring;
				++tostring;
			}
			*tostring = 0;
			free(str);
			str = nstr;
		}
	}
	// free up the vlist
	while (vlist) {
		struct variable_list *freeme = vlist;

		free(vlist->varname);
		vlist = vlist->next;
		free(freeme);
	}
	standard_output = standard_output_save;
	return str;
}

static int recursion(char *str)
{
	struct variable_list *vlist, *vcurs, vstack_base;
	int retval = 0;

	vlist = extract_vars(str);
	for (vcurs = vlist; vcurs && !retval; vcurs = vcurs->next) {
		vstack_base.varname = strdup(vcurs->varname);
		vstack_base.next = NULL;
		retval = find_recursion(&vstack_base);
		free(vstack_base.varname);
	}
	while (vlist) {
		vcurs = vlist->next;
		free(vlist->varname);
		free(vlist);
		vlist = vcurs;
	}
	return retval;
}

int find_recursion(struct variable_list *vstack)
{
	struct variable_list *vlist = NULL;
	int retval = 0;
	struct answer a;
	struct variable_list *vcurs, *vstackcurs;

	a = getvar_full(vstack->varname);
	if (a.err || !a.exp)
		return 0;

	vlist = extract_vars(a.exp);
	// for each entry in the vlist, see if it's in the vstack
	for (vcurs = vlist; vcurs; vcurs = vcurs->next) {
		for (vstackcurs = vstack; vstackcurs; vstackcurs = vstackcurs->next) {
			if (!strcmp(vcurs->varname, vstackcurs->varname)) {
				char *error =
					malloc(sizeof(char) * (strlen(vcurs->varname) + 73));
				sprintf(error,
						"%s was found twice in symbol descent. Recursive variables are not allowed.",
						vcurs->varname);
				report_error(error);
				free(error);
				return 1;
			}
		}
	}
	// for each entry in the vlist, see if it has recursion
	for (vcurs = vlist; vcurs && !retval; vcurs = vcurs->next) {
		struct variable_list *bookmark = vcurs->next;

		vcurs->next = vstack;
		retval = find_recursion(vcurs);
		vcurs->next = bookmark;
	}
	// free the vlist
	vcurs = vlist;
	while (vcurs) {
		struct variable_list *bookmark = vcurs->next;

		free(vcurs->varname);
		free(vcurs);
		vcurs = bookmark;
	}
	return retval;
}

void report_error(char *err)
{
	extern char *errstring;
	char *tempstring;

	if (errstring) {
		tempstring =
			calloc(strlen(errstring) + 2 + strlen(err), sizeof(char));
		sprintf(tempstring, "%s\n%s", errstring, err);
		free(errstring);
		errstring = tempstring;
	} else {
		errstring = (char *)strdup(err);
	}
}

void set_prettyanswer(mpfr_t num)
{
	char *temp;

	if (pretty_answer) {
		free(pretty_answer);
	}
	temp = print_this_result(num);
	if (temp) {
		pretty_answer = (char *)strdup(temp);
	} else {
		pretty_answer = NULL;
	}
}

char *print_this_result(mpfr_t result)
{
	static char pa[500];
	char pa2[500];
	extern char *errstring;
	unsigned int base = 0;

	// output in the proper base and format
	switch (conf.output_format) {
		case HEXADECIMAL_FORMAT:
			base = 16;
			break;
		default:
		case DECIMAL_FORMAT:
			base = 10;
			break;
		case OCTAL_FORMAT:
			base = 8;
			break;
		case BINARY_FORMAT:
			base = 2;
			break;
	}
	num_to_str_complex(pa, result, base, conf.engineering, conf.precision,
					   conf.print_prefixes);

	/* now, decide whether it's been rounded or not */
	if (mpfr_inf_p(result) || mpfr_nan_p(result)) {
		// if it is infinity, it's all there ;)
		not_all_displayed = 0;
	} else {
		/* rounding guess */
		switch (conf.rounding_indication) {
			case SIMPLE_ROUNDING_INDICATION:
				num_to_str_complex(pa2, result, base, conf.engineering, -1,
								   conf.print_prefixes);
				not_all_displayed = (strlen(pa) < strlen(pa2));
				break;
			case SIG_FIG_ROUNDING_INDICATION:
				/* sig_figs is how many we need to display */
				if (sig_figs < UINT32_MAX) {
					/* do something */
#warning SIG_FIG_ROUNDING_INDICATION not implemented
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

	Dprintf("standard_output? -> ");
	if (standard_output) {
		Dprintf("yes\n");
		if (errstring && strlen(errstring)) {
			extern int scanerror;

			fprintf(stderr, "%s\n", errstring);
			free(errstring);
			errstring = NULL;
			scanerror = 0;
		}
		printf("%s%s\n",
			   conf.print_equal ? (not_all_displayed ? " ~= " : " = ")
			   : (not_all_displayed ? "~" : ""), pa);
	}
	Dprintf("no\n");

	return pa;
}

void simple_exp(mpfr_t output, mpfr_t first, enum operations op,
				mpfr_t second)
{
	if (compute) {
		mpfr_t temp;

		mpfr_init(temp);

		switch (op) {
			default:
				mpfr_set_d(output, 0.0, GMP_RNDN);
				break;
			case wequal:
				mpfr_set_ui(output, mpfr_equal_p(first, second), GMP_RNDN);
				break;
			case wnequal:
				mpfr_set_ui(output, !mpfr_equal_p(first, second), GMP_RNDN);
				break;
			case wgt:
				mpfr_set_ui(output, mpfr_greater_p(first, second), GMP_RNDN);
				break;
			case wlt:
				mpfr_set_ui(output, mpfr_less_p(first, second), GMP_RNDN);
				break;
			case wgeq:
				mpfr_set_ui(output, mpfr_greaterequal_p(first, second),
							GMP_RNDN);
				break;
			case wleq:
				mpfr_set_ui(output, mpfr_lessequal_p(first, second),
							GMP_RNDN);
				break;
			case wplus:
				mpfr_add(output, first, second, GMP_RNDN);
				break;
			case wminus:
				mpfr_sub(output, first, second, GMP_RNDN);
				break;
			case wmult:
				mpfr_mul(output, first, second, GMP_RNDN);
				break;
			case wdiv:
				mpfr_div(output, first, second, GMP_RNDN);
				break;
			case wpow:
				mpfr_pow(output, first, second, GMP_RNDN);
				break;
			case wor:
				mpfr_set_ui(output, (!mpfr_zero_p(first)) ||
							(!mpfr_zero_p(second)), GMP_RNDN);
				break;
			case wand:
				mpfr_set_ui(output, (!mpfr_zero_p(first)) &&
							(!mpfr_zero_p(second)), GMP_RNDN);
				break;
			case wbor:
#warning Binary OR isn't implemented
				break;
			case wband:
#warning Binary AND isn't implemented
				break;
			case wlshft:
				mpfr_pow_ui(temp, second, 2, GMP_RNDN);
				mpfr_mul(output, first, temp, GMP_RNDN);
				break;
			case wrshft:
				mpfr_pow_ui(temp, second, 2, GMP_RNDN);
				mpfr_div(output, first, temp, GMP_RNDN);
				break;
			case wmod:
				if (mpfr_zero_p(second)) {
					mpfr_set_nan(output);
				} else {
					/* find the biggest integer (temp) for which
					 * first-temp*second is positive, then return the value
					 * first-temp*second */
					mpfr_set_ui(temp, 0, GMP_RNDN);
					while (mpfr_cmp_ui(output, 0) > 0) {
						mpfr_add_ui(temp, temp, 1, GMP_RNDN);	// temp++
						mpfr_mul(output, temp, second, GMP_RNDN);	// temp*second
						mpfr_sub(output, first, output, GMP_RNDN);
					}
					mpfr_sub_ui(temp, temp, 1, GMP_RNDN);
					mpfr_mul(output, temp, second, GMP_RNDN);
					mpfr_sub(output, first, output, GMP_RNDN);
				}
				break;
		}
		mpfr_clear(temp);
		return;
	} else {
		mpfr_set_ui(output, 0, GMP_RNDN);
		return;
	}
}

void uber_function(mpfr_t output, enum functions func, mpfr_t input)
{
	if (compute) {
		mpfr_t temp;

		mpfr_init(temp);
		if (!conf.use_radians) {
			switch (func) {
				case wsin:
				case wcos:
				case wtan:
				case wcot:
					mpfr_const_pi(temp, GMP_RNDN);
					mpfr_mul(input, input, temp, GMP_RNDN);
					mpfr_div_ui(input, input, 180, GMP_RNDN);
					break;
				case wasin:
				case wacos:
				case watan:
				case wacot:
					mpfr_const_pi(temp, GMP_RNDN);
					mpfr_pow_si(temp, temp, -1, GMP_RNDN);
					mpfr_mul_ui(temp, temp, 180, GMP_RNDN);
					break;
				default:
					break;
			}
		}
		switch (func) {
			case wsin:
				mpfr_sin(output, input, GMP_RNDN);
				break;
			case wcos:
				mpfr_cos(output, input, GMP_RNDN);
				break;
			case wtan:
				mpfr_tan(output, input, GMP_RNDN);
				break;
			case wcot:
				mpfr_tan(output, input, GMP_RNDN);
				mpfr_pow_si(output, output, -1, GMP_RNDN);
				break;
			case wasin:
				mpfr_asin(output, input, GMP_RNDN);
				if (!conf.use_radians) {
					mpfr_mul(output, output, temp, GMP_RNDN);
				}
				break;
			case wacos:
				mpfr_acos(output, input, GMP_RNDN);
				if (!conf.use_radians) {
					mpfr_mul(output, output, temp, GMP_RNDN);
				}
				break;
			case watan:
				mpfr_atan(output, input, GMP_RNDN);
				if (!conf.use_radians) {
					mpfr_mul(output, output, temp, GMP_RNDN);
				}
				break;
			case wacot:
				mpfr_pow_si(output, input, -1, GMP_RNDN);
				mpfr_atan(output, output, GMP_RNDN);
				if (!conf.use_radians) {
					mpfr_mul(output, output, temp, GMP_RNDN);
				}
				break;
			case wsinh:
				mpfr_sinh(output, input, GMP_RNDN);
				break;
			case wcosh:
				mpfr_cosh(output, input, GMP_RNDN);
				break;
			case wtanh:
				mpfr_tanh(output, input, GMP_RNDN);
				break;
			case wcoth:
				mpfr_tanh(output, input, GMP_RNDN);
				mpfr_pow_si(output, output, -1, GMP_RNDN);
				break;
			case wasinh:
				mpfr_asinh(output, input, GMP_RNDN);
				break;
			case wacosh:
				mpfr_acosh(output, input, GMP_RNDN);
				break;
			case watanh:
				mpfr_atanh(output, input, GMP_RNDN);
				break;
			case wacoth:
				mpfr_pow_si(input, input, -1, GMP_RNDN);
				mpfr_atanh(output, input, GMP_RNDN);
				break;
			case wlog:
				mpfr_log10(output, input, GMP_RNDN);
				break;
			case wlogtwo:
				mpfr_log2(output, input, GMP_RNDN);
				break;
			case wln:
				mpfr_log(output, input, GMP_RNDN);
				break;
			case wround:
				mpfr_rint(output, input, GMP_RNDN);
				break;
			case wneg:
				mpfr_mul_si(output, input, -1, GMP_RNDN);
				break;
			case wnot:
				mpfr_set_ui(output, mpfr_zero_p(input), GMP_RNDN);
				break;
			case wabs:
				mpfr_abs(output, input, GMP_RNDN);
				break;
			case wsqrt:
				mpfr_sqrt(output, input, GMP_RNDN);
				break;
			case wfloor:
				mpfr_floor(output, input);
				break;
			case wceil:
				mpfr_ceil(output, input);
				break;
			case wrand:
#warning rand(range) hasn't been tested
				seed_random();
				while (mpfr_urandomb(output,randstate) != 0) ;
				mpfr_mul(output,output,input,GMP_RNDN);
				if (mpfr_cmp_si(input,0) < 0) {
					mpfr_mul_si(output,output,-1,GMP_RNDN);
				}
				break;
			case wirand:
#warning irand(range) hasn't been tested
				seed_random();
				while (mpfr_urandomb(output,randstate) != 0) ;
				mpfr_mul(output,output,input,GMP_RNDN);
				if (mpfr_cmp_si(input,0) < 0) {
					mpfr_mul_si(output,output,-1,GMP_RNDN);
				}
				mpfr_rint(output,output,GMP_RNDN);
				break;
			case wcbrt:
				mpfr_cbrt(output, input, GMP_RNDN);
				break;
			case wexp:
				mpfr_exp(output, input, GMP_RNDN);
				break;
			case wfact:
				mpfr_fac_ui(output, mpfr_get_ui(input, GMP_RNDN), GMP_RNDN);
				break;
			default:
				mpfr_set(output, input, GMP_RNDN);
				break;
		}
		mpfr_clear(temp);
		return;
	} else {
		mpfr_set_ui(output, 0, GMP_RNDN);
		return;
	}
}

int seed_random(void)
{
	static char seeded = 0;

	if (!seeded) {
		gmp_randinit_default(randstate);
		gmp_randseed_ui(randstate, time(NULL));
		//srandom(time(NULL));
		seeded = 1;
	}
	return 1;
}

char *output_string(unsigned int o)
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
