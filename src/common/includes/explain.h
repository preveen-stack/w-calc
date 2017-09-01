#ifndef WCALC_EXPLAIN
#define WCALC_EXPLAIN

#include "number.h" /* for Number */
#include "list.h"   /* for List */

struct name_with_exp {
    const char *names[10];
    const char *explanation;
};

void init_explanation(void (*show_consts)(const struct name_with_exp *),
                      void (*show_explanation)(const char *, ...),
                      void (*show_expvar_exp)(const char *str, const char *exp, List subv, const char *desc),
                      void (*show_valvar_exp)(const char *str, Number *val, const char *desc));
void explain(const char *str);
#endif
/* vim:set expandtab: */
