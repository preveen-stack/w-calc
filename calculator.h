#ifndef WCALC_H
#define WCALC_H

#include "config.h"
#include "definitions.h"

enum functions {wnot,
	wsin,
	wcos,
	wtan,
	wasin,
	wacos,
	watan,
	wsinh,
	wcosh,
	wtanh,
	wasinh,
	wacosh,
	watanh,
	wlog,
	wlogtwo,
	wln,
	wround,
	wneg,
	wabs,
	wsqrt,
	wfloor,
	wceil,
	wcbrt};
enum operations {wplus,
	wminus,
	wmult,
	wdiv,
	wmod,
	wfact,
	wexp,
	wor,
	wand,
	wequal,
	wnequal,
	wgt,
	wlt,
	wrshft,
	wlshft,
	wgeq,
	wleq,
	wnone};
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
	unsigned int engineering:1;
	unsigned int picky_variables:1;
	unsigned int strict_syntax:1;
	unsigned int use_radians:1;
	unsigned int output_format:4;
	unsigned int print_prefixes:1;
	unsigned int rounding_indication:4;
	unsigned int remember_errors:1;
	unsigned char charkey[256];
	unsigned char charunkey[256];
	unsigned char thou_delimiter;
	unsigned char dec_delimiter;
	unsigned int precision_guard:1;
	unsigned int history_limit:1;
	unsigned int history_limit_len;
};

/* configuration */
extern struct _conf conf;

/* results */
extern double last_answer;
extern char *pretty_answer;

/* communication with parser */
extern char compute;
extern unsigned int sig_figs;

/* communication with the frontend */
extern char standard_output;
extern char not_all_displayed;

#define DECIMAL_FORMAT 0
#define OCTAL_FORMAT 1
#define HEXADECIMAL_FORMAT 2
#define BINARY_FORMAT 3

#define NO_ROUNDING_INDICATION 0
#define SIMPLE_ROUNDING_INDICATION 1
#define SIG_FIG_ROUNDING_INDICATION 2

#endif
