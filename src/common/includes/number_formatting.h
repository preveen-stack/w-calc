#ifndef WCALC_NUMBER_FORMATTING_H
#define WCALC_NUMBER_FORMATTING_H

#include <stdbool.h>
#include "number.h"
#include "conf.h" /* for enum scientific_modes */

char *num_to_str_complex(const Number          num,
                         int                   base,
                         enum scientific_modes engr,
                         int                   prec,
                         int                   prefix,
                         bool                 *truncated_flag);
#endif // ifndef WCALC_NUMBER_FORMATTING_H
/* vim:set expandtab: */
