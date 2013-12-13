/* */
#ifndef WCALC_OUTPUT_H
#define WCALC_OUTPUT_H

/* These functions should be implemented in any frontend. */

#include "variables.h" // for variable_t
#include "list.h"      // for List

void display_prefs(void);
void display_and_clear_errstring(void);
void show_answer(char *err,
                 int   uncertain,
                 char *answer);
void display_output_format(int format);
void display_status(const char *format,
                    ...);
void display_err(const char *format,
                 ...);
void display_interactive_help(void);
void display_command_help(void);
void display_val(const char *name);
void display_var(variable_t *v,
                 unsigned    count,
                 unsigned    digits);
void display_expvar_explanation(const char *str,
                                const char *exp,
                                List        subvars,
                                const char *desc);
void display_valvar_explanation(const char *str,
                                Number     *val,
                                const char *desc);
void display_explanation(const char*exp, ...);
void display_stateline(const char *buf);
void display_consts(void);

#endif // ifndef WCALC_OUTPUT_H
/* vim:set expandtab: */
