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

void init_resultprinter(void (*sa)(char*,int,char*));
void parseme(const char *);
void report_error(const char *fmt,
                  ...);
void  set_prettyanswer(const Number num);
char *print_this_result(const Number result,
                        int          output,
                        char        *nad,
                        char       **es);
void uber_function(Number               output,
                   const enum functions func,
                   Number               input);
void simple_exp(Number                output,
                const Number          first,
                const enum operations op,
                const Number          second);
int   seed_random(void);
char *output_string(const unsigned int);

/* results */
extern Number last_answer;
extern char  *pretty_answer;

/* communication with parser */
extern char         compute;
extern unsigned int sig_figs;

/* communication with the frontend */
extern char standard_output;
extern char not_all_displayed;

#endif // ifndef WCALC_CALCULATOR_H
/* vim:set expandtab: */
