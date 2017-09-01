/*
 * stdin
 * Copyright (c) 2013 Micron Technology, Inc.
 *
 */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* System Headers */
#include <string.h>
#include <wctype.h>                     /* for iswspace() */

/* Internal Headers */
#include "number.h"
#include "uint32_max.h"
#include "variables.h"
#include "calculator.h"
#include "list.h"
#include "extract_vars.h"
#include "evalvar.h"
#include "iscmd.h"
#include "isconst.h"
#include "isfunc.h"
#include "explain.h"
#include "output.h"

static int explain_command(const char *,
                           int);
static void explain_variable(const char *);
static void explain_constant(const char *);
static void explain_function(const char *);

/* Injected Dependencies (callback functions for output) */
static void (*display_consts)(const struct name_with_exp *);
static void (*display_explanation)(const char *, ...);
static void (*display_expvar_explanation)(const char *,
                                          const char *,
                                          List,
                                          const char *);
static void (*display_valvar_explanation)(const char *,
                                          Number *,
                                          const char *);

void
init_explanation(void (*show_consts)(const struct name_with_exp *),
                 void (*show_explanation)(const char *, ...),
                 void (*show_expvar_exp)(const char *, const char *, List, const char *),
                 void (*show_valvar_exp)(const char *, Number *, const char *))
{
    display_consts = show_consts;
    display_explanation = show_explanation;
    display_expvar_explanation = show_expvar_exp;
    display_valvar_explanation = show_valvar_exp;
}

void explain(const char *str)
{                                      /*{{{ */
    size_t curs;
    char  *mystr;


    if ((str == NULL) || (str[0] == 0)) {
        str = "\\explain";
    }
    mystr = strdup(str);
    /* for sanity's sake, remove any trailing whitespace */
    curs = strlen(mystr) - 1;
    while (iswspace(mystr[curs]) && curs > 0) {
        mystr[curs] = 0;
        curs--;
    }
    if (!strcmp(mystr, "q")) {
        display_explanation("Quits the program.");
    } else if (!strcmp(mystr, "constants")) {
        display_explanation("Wcalc understands many standard physical constants.\n"
                            "Here is the list of the ones currently understood;\n"
                            "each can be explained in detail individually (some\n"
                            "mean the same thing).\n");
        display_consts(getConsts());
    } else if (*mystr == '\\') {       // it's a command
        explain_command(mystr, 0);
    } else if (isconst(mystr)) {       // it's a constant
        explain_constant(mystr);
    } else if (isfunc(mystr)) {        // it's a function
        explain_function(mystr);
    } else if (varexists(mystr)) {     // it's a variable
        explain_variable(mystr);
    } else {                           // it's a call for help
        int   len         = strlen(mystr) + 2;
        char *add_a_slash = calloc(len, sizeof(char));

        add_a_slash[0] = '\\';
        add_a_slash[1] = 0;
        strncat(add_a_slash, mystr, len - 2);
        if (explain_command(add_a_slash, 1) == -1) {
            report_error("%s is neither a command, constant, function, or variable.\n", mystr);
        }
        free(add_a_slash);
    }
    free(mystr);
}                                      /*}}} */

static inline int scan_exps(const char                 *str,
                            const struct name_with_exp *exps)
{/*{{{*/
    const struct name_with_exp *curs;

    for (curs = exps; curs->explanation; curs++) {
        const char * const * const names = curs->names;
        for (unsigned i = 0; names[i]; i++) {
            if (!strcmp(str, names[i])) {
                display_explanation(curs->explanation);
                return 0;
            }
        }
    }
    return -1;
}/*}}}*/

static int explain_command(const char *str,
                           int         test)
{                                      /*{{{ */
    str++;
    if (!strcmp(str, "bits")) {
        display_explanation("Sets the number of bits used internally to represent numbers. Used like this: \\bitsX where X is a number that must be above %li and below %li.",
                            (long int)NUM_PREC_MIN, (long int)NUM_PREC_MAX);
        return 0;
    } else {
        if (scan_exps(str, commands) == 0) {
            return 0;
        }
    }
    if (test == 0) {
        report_error("Undefined command.");
    }
    return -1;
}                                      /*}}} */

static void explain_variable(const char *str)
{                                      /*{{{ */
    struct answer var;

    var = getvar_full(str);
    if (var.err) {
        report_error("An error was encountered!\n");
        return;
    }
    if (var.exp) {                     // it's an expression (i.e. a function)
        List strings;

        strings = extract_vars(var.exp);
        display_expvar_explanation(str, var.exp, strings, var.desc);
        freeList(&strings);
    } else {
        display_valvar_explanation(str, &var.val, var.desc);
    }
}                                      /*}}} */

static void explain_constant(const char *str)
{                                      /*{{{ */
    (void)scan_exps(str, getConsts());
}                                      /*}}} */

static void explain_function(const char *str)
{                                      /*{{{ */
    (void)scan_exps(str, funcs);
}                                      /*}}} */

/* vim:set expandtab: */
