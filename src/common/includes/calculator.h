/*
 *  calculator.h
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Thu Feb 07 2002.
 *  Copyright (c) 2001 Kyle Wheeler. All rights reserved.
 *
 */

#ifndef WCALC_CALCULATOR_H
#define WCALC_CALCULATOR_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef EBUG
# include <stdio.h>
# define Dprintf(fmt, ...)                                               \
    fprintf(stderr, "[%s:%d] " fmt, __FILE__, __LINE__, ## __VA_ARGS__); \
    fflush(stderr);
#else
# define Dprintf(...) ;
#endif

#include "definitions.h"
#include "number.h"

enum functions {
    wnot,
    wbnot,
    wsin,
    wcos,
    wtan,
    wcot,
    wsec,
    wcsc,
    wasin,
    wacos,
    watan,
    wacot,
    wasec,
    wacsc,
    wsinh,
    wcosh,
    wtanh,
    wcoth,
    wsech,
    wcsch,
    wasinh,
    wacosh,
    watanh,
    wacoth,
    wasech,
    wacsch,
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
    wexp,
    wfact,
    wcomp,
    weint,
    wgamma,
    wlngamma,
    wzeta,
    wsinc,
    wcbrt
};
enum operations {
    wplus,
    wminus,
    wmult,
    wdiv,
    wmod,
    wpow,
    wor,
    wbor,
    wbxor,
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
    wnone
};

enum commands {redisplay, nothing};

enum engineering_modes {always, never, automatic};

void parseme(const char *);
void report_error(const char *fmt,
                  ...);
void  display_and_clear_errstring(void);
void  set_prettyanswer(const Number num);
char *print_this_result(const Number result);
void  uber_function(Number               output,
                    const enum functions func,
                    Number               input);
void simple_exp(Number                output,
                const Number          first,
                const enum operations op,
                const Number          second);
int   seed_random(void);
char *output_string(const unsigned int);

struct _conf {
    unsigned long          history_limit_len;
    int                    precision;
    char                   thou_delimiter;
    char                   dec_delimiter;
    char                   in_thou_delimiter;
    char                   in_dec_delimiter;
    enum engineering_modes engineering         : 2;
    unsigned int           picky_variables     : 1;
    unsigned int           use_radians         : 1;
    unsigned int           output_format       : 4;
    unsigned int           print_prefixes      : 1;
    unsigned int           rounding_indication : 4;
    unsigned int           remember_errors     : 1;
    unsigned int           precision_guard     : 1;
    unsigned int           history_limit       : 1;
    unsigned int           print_equal         : 1;
    unsigned int           print_ints          : 1;
    unsigned int           simple_calc         : 1;
    unsigned int           verbose             : 1;
    unsigned int           print_commas        : 1;
    unsigned int           live_precision      : 1;
    unsigned int           c_style_mod         : 1;
};

/* configuration */
extern struct _conf conf;

/* results */
extern Number last_answer;
extern char  *pretty_answer;

/* communication with parser */
extern char         compute;
extern unsigned int sig_figs;

/* communication with the frontend */
extern char standard_output;
extern char not_all_displayed;

#define DECIMAL_FORMAT     0
#define OCTAL_FORMAT       1
#define HEXADECIMAL_FORMAT 2
#define BINARY_FORMAT      3

#define NO_ROUNDING_INDICATION      0
#define SIMPLE_ROUNDING_INDICATION  1
#define SIG_FIG_ROUNDING_INDICATION 2

#endif // ifndef WCALC_CALCULATOR_H
/* vim:set expandtab: */
