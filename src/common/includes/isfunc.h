#ifndef WCALC_ISFUNC
#define WCALC_ISFUNC

#include "explain.h"

struct name_with_exp;

int isfunc(const char *str);
const struct name_with_exp *getFuncs(void);
#endif
/* vim:set expandtab: */
