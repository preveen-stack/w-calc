#ifndef WCALC_H
#define WCALC_H

#ifndef GUI
#include "config.h"
#endif
#include "definitions.h"

#ifdef EBUG
#include <stdio.h>
#define Dprintf(fmt, ...) \
	fprintf(stderr, "[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
	fflush(stderr);
#else
#define Dprintf(...) ;
#endif

enum functions {wnot,
	wbnot,
	wsin,
	wcos,
	wtan,
	wcot,
	wasin,
	wacos,
	watan,
	wacot,
	wsinh,
	wcosh,
	wtanh,
	wcoth,
	wasinh,
	wacosh,
	watanh,
	wacoth,
	wlog,
	wlogtwo,
	wln,
	wround,
	wneg,
	wabs,
	wsqrt,
	wfloor,
	wceil,
	wrand,
	wirand,
	wcbrt};
enum operations {wplus,
	wminus,
	wmult,
	wdiv,
	wmod,
	wfact,
	wexp,
	wor,
	wbor,
	wand,
	wband,
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
void set_prettyanswer(double num);
char *print_this_result (double result);
double uber_function (enum functions func, double input);
double simple_exp (double first, enum operations op, double second);
double kbw_rand ();
int kbw_int_rand ();
char *output_string(unsigned int);

double fact (int);

struct _conf {
	int precision;
	unsigned int engineering:1;
	unsigned int picky_variables:1;
	unsigned int use_radians:1;
	unsigned int output_format:4;
	unsigned int print_prefixes:1;
	unsigned int rounding_indication:4;
	unsigned int remember_errors:1;
	unsigned char thou_delimiter;
	unsigned char dec_delimiter;
	unsigned int precision_guard:1;
	unsigned int history_limit:1;
	unsigned long history_limit_len;
	unsigned int print_equal:1;
	unsigned int print_ints:1;
	unsigned int simple_calc:1;
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
