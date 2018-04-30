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

#include <stdbool.h>

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

void init_calculator(void (*sa)(char*, bool, char*));
void term_calculator(void);

Number *get_last_answer(void);
void set_last_answer(Number value);
void parseme(const char *);
void report_error(const char *fmt,
                  ...);
void uber_function(Number         output,
                   enum functions func,
                   Number         input);
void simple_exp(Number          output,
                const Number    first,
                enum operations op,
                const Number    second);

/* communication with parser */
extern char         compute;
extern unsigned int sig_figs;

/* communication with the frontend */
extern bool standard_output;
extern bool not_all_displayed;

#endif // ifndef WCALC_CALCULATOR_H
/* vim:set expandtab: */
