#ifndef WCALC_H
#define WCALC_H

#include "definitions.h"

enum functions {wnot,wsin,wcos,wtan,wasin,wacos,watan,wsinh,wcosh,wtanh,wasinh,wacosh,watanh,wlog,wln,wround,wneg,wabs,wsqrt,wfloor,wceil};
enum operations {wplus,wminus,wmult,wdiv,wmod,wfact,wexp,wor,wand,wequal,wnequal,wgt,wlt,wgeq,wleq,wnone};
enum commands {redisplay,nothing};

double parseme (char *);
void print_results (void);
void push_value (double);
void report_error (char *);
void print_result (void);
char *print_this_result (double result);
double uber_function (enum functions func, double input);
double simple_exp (double first, enum operations op, double second);
double kbw_rand ();


double fact (int);

struct _conf {
	int precision;
	int engineering:1;
	int picky_variables:1;
	int strict_syntax:1;
	int use_radians:1;
	int output_format:4;
	int print_prefixes:1;
	int use_commas:1;
	int rounding_indication:1;
};

/* configuration */
extern struct _conf conf;

/* results */
extern double last_answer;
extern char *pretty_answer;

/* communication with parser */
extern char compute;

/* communication with the frontend */
extern char standard_output;
extern char not_all_displayed;

/*extern char not_all_displayed;

extern int precision;
extern char engineering;
extern char picky_variables;
extern char strict_syntax;
extern char use_radians;
extern char output_format;*/
#define DECIMAL_FORMAT 0
#define OCTAL_FORMAT 1
#define HEXADECIMAL_FORMAT 2
#define BINARY_FORMAT 3
//extern char print_prefixes;
//extern char use_commas;

#endif
