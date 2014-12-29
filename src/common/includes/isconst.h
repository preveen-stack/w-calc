#ifndef WCALC_ISCONST
#define WCALC_ISCONST

#include "explain.h"

extern const struct name_with_exp consts[];

int  isconst(const char *str);
void printconsts(void);
#endif // ifndef WCALC_ISCONST
/* vim:set expandtab: */
