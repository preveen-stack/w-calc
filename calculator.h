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

/* configuration variables */
extern double last_answer;
extern char *pretty_answer;
extern short standard_output;
extern short compute;

extern int precision;
extern short engineering;
extern short picky_variables;
extern short use_radians;
extern short output_format;
#define DECIMAL_FORMAT 0
#define OCTAL_FORMAT 1
#define HEXADECIMAL_FORMAT 2
#define BINARY_FORMAT 3
extern short print_prefixes;
extern short use_commas;

#endif
