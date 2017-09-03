#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* System Headers */
#include <string.h>  /* for strlen() */
#include <strings.h> /* for strncasecmp(), per POSIX 2001 */

/* Internal Headers */
#include "isfunc.h"

const struct name_with_exp funcs[] = {
    { { "sin", NULL },
      "A trigonometric function." },
    { { "cos", NULL },
      "A trigonometric function." },
    { { "tan", NULL },
      "A trigonometric function." },
    { { "cot", NULL },
      "A trigonometric function." },
    { { "asin", "arcsin", NULL },
      "The inverse of the standard trigonometric function." },
    { { "acos", "arccos", NULL },
      "The inverse of the standard trigonometric function." },
    { { "atan", "arctan", NULL },
      "The inverse of the standard trigonometric function." },
    { { "acot", "arctan", NULL },
      "The inverse of the standard trigonometric function." },
    { { "sinh", NULL },
      "A standard hyperbolic trigonometric function." },
    { { "cosh", NULL },
      "A standard hyperbolic trigonometric function." },
    { { "tanh", NULL },
      "A standard hyperbolic trigonometric function." },
    { { "coth", NULL },
      "A standard hyperbolic trigonometric function." },
    { { "asinh", "areasinh", NULL },
      "The inverse of the standard hyperbolic trigonometric function." },
    { { "acosh", "areacosh", NULL },
      "The inverse of the standard hyperbolic trigonometric function." },
    { { "atanh", "areatanh", NULL },
      "The inverse of the standard hyperbolic trigonometric function." },
    { { "acoth", "areacoth", NULL },
      "The inverse of the standard hyperbolic trigonometric function." },
    { { "log", NULL },
      "A logarithm (base 10)." },
    { { "logtwo", NULL },
      "A logarithm (base 2)." },
    { { "ln", NULL },
      "A logarithm (base e). Also known as the \"natural\" log." },
    { { "round", NULL },
      "Returns the closest integer to the input number." },
    { { "abs", NULL },
      "The absolute value (the distance of the number from zero)." },
    { { "floor", NULL },
      "Returns the biggest integer that is not bigger than the input number." },
    { { "ceil", "ceiling", NULL },
      "Returns the smallest integer that is not smaller than the input number." },
    { { "sqrt", NULL },
      "The square root function." },
    { { "cbrt", NULL },
      "The cube root function." },
    { { "exp", NULL },
      "Returns the value of e to the given power. Equivalent to: e^" },
    { { "fact", NULL },
      "Returns the factorial of the input number." },
    { { "comp", NULL },
      "Returns the one's complement of the input number." },
#ifdef HAVE_MPFR_22
    { { "eint", NULL },
      "The exponential integral function." },
    { { "gamma", NULL },
      "Returns the Gamma function of the input number. The Gamma function extends the factorial function to complex and non-natural numbers." },
    { { "lngamma", NULL },
      "Returns the natural log of the Gamma function of the input number." },
#endif
    { { "rand", NULL },
      "Returns a random number between 0 and the input number." },
    { { "irand", NULL },
      "Returns a random integer between 0 and the input number." },
    { { "zeta", NULL },
      "Returns the Riemann zeta function of the input number. This is used primarily in number theory, but also has applications in physics, probability theory, and applied statistics." },
    { { "sinc", NULL },
      "Sinus cardinalis, also known as the interpolation function, filtering function, or the first spherical Bessel function, is the product of a sine function and a monotonically decreasing function." },
    { { 0 }, NULL }
};

const struct name_with_exp *getFuncs()
{
    return funcs;
}

int
isfunc(const char *str)
{   /*{{{*/
    unsigned i, j;

    if (!str) return 0;
    for (i = 0; funcs[i].explanation; i++) {
        for (j = 0; funcs[i].names[j]; j++) {
            if (strncasecmp(funcs[i].names[j], str, strlen(funcs[i].names[j]) + 1) == 0) {
                return 1;
            }
        }
    }
    return 0;
} /*}}}*/

/* vim:set expandtab: */
