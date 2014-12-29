#ifndef WCALC_NUMBER_FORMATTING_H
#define WCALC_NUMBER_FORMATTING_H

#include "number.h"

char *num_to_str_complex(const Number                 num,
                         const int                    base,
                         const enum engineering_modes engr,
                         const int                    prec,
                         const int                    prefix,
                         char                        *truncated_flag);
#endif // ifndef WCALC_NUMBER_FORMATTING_H
/* vim:set expandtab: */
