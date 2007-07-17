#include <string.h>
#include "config.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif

const char *funcs[] = { "sin", "cos", "tan", "cot",
    "asin", "acos", "atan", "acot",
    "arcsin", "arccos", "arctan", "arccot",
    "sinh", "cosh", "tanh", "coth",
    "asinh", "acosh", "atanh", "acoth",
    "areasinh", "areacosh", "areatanh", "areacoth",
    "log", "logtwo", "ln",
    "round", "abs", "floor", "ceil", "ceiling",
    "sqrt", "cbrt",
    "exp", "fact", "comp",
#ifdef HAVE_MPFR_22
    "eint","Gamma","gamma","lngamma","lnGamma",
#endif
    "rand", "irand",
    "zeta","sinc",
    0
};

int isfunc(const char *str)
{
    size_t i;

    for (i = 0; funcs[i]; i++) {
	if (!strcmp(funcs[i], str)) {
	    return 1;
	}
    }
    return 0;
}
