#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* System Headers */
#include <string.h>

/* Internal Headers */
#include "iscmd.h"

const struct name_with_exp commands[] = {
    { { "b", "bin", "binary", NULL},
        "This displays the output in binary."},
    { { "store", NULL},
        "Saves the variable specified in the preload file, ~/.wcalc_preload. Use like so: \\store variablename"},
    { { "q", NULL},
        "Exits the program."},
    { { "d", "dec", "decimal", NULL },
        "This displays the output in decimal."},
    { { "delim", NULL },
        "This toggles the formatting of output such that delimiters are inserted in integers."},
    { { "dsep", NULL },
        "Used like so: \\dsepX This sets the decimal separator to X. By default, it is a period (.)."},
    { { "s", "sci", "scientific", NULL },
        "This toggles the formatting of output between decimal and scientific notation."},
    { { "color", NULL},
        "Toggles the use of color in the commandline output."},
    { { "cons", "conservative", NULL},
        "Toggles precision guards."},
    { { "h", "hex", "hexadecimal", "x", NULL},
        "This displays the output in hexadecimal."},
    { { "help", NULL },
        "This displays a generic help message."},
    { { "hlimit", NULL },
        "This places (or removes) a limit on the number of things stored in the history. Use like this: \\hlimitX where X is a number. 0 removes the limit."},
    { { "ints", NULL },
        "Toggles whether long integers will be abbreviated or not (conflicts with scientific notation for large numbers, but not for decimals)."},
    { { "li", "list", "listvars", NULL },
        "Prints out the currently defined variables."},
    { { "o", "oct", "octal", NULL},
        "This displays the output in octal."},
    { { "open", NULL},
        "Loads a saved file. Used like this: \\openXXXXX where XXXXX is the name of the file to load."},
    { { "p", NULL },
        "Sets the precision. Use it like so: \\pX where X is the desired precision. Precision here is in digits. This setting only affects display. -1 means \"auto\"."},
    { { "pre", "prefix", "prefixes", NULL},
        "Toggles whether prefixes are shown with the output."},
    { { "pref", "prefs", "preferences", NULL},
        "Prints out the current preference settings."},
    { { "r", "rad", "radians", NULL},
        "Toggles radian mode for trigonometric functions."},
    { { "rou", "round", "rounding", NULL},
        "Sets the rounding indication. The possible arguments to this preference are \"none\", \"simple\", and \"sig_fig\". Use like this: \\round none"},
    { { "re", "remember", "remember_errors", NULL },
        "Toggles whether errors are remembered in history."},
    { { "assert", NULL },
        "Internal debugging command, used for testing."},
    { { "bits", NULL},
        ""},
    { { "save", NULL},
        "Saves the history and variable list to a file. Used like this: \\saveXXXXX where XXXXX is the name of the file to save."},
    { { "tsep", NULL},
        "Used like this: \\tsepX Sets the thousands-place separator character to X. The default is a comma (,)."},
    { { "c", "conv", "convert", NULL},
        "Used either like this: \"\\convert unit1 to unit2\" or like this: \"\\convert unit1 unit2\". Converts the previous answer from the first unit to the second."},
    { { "base", NULL},
        "Prints out the previous answer in any base from 2 to 36."},
    { { "verbose", NULL},
        "Prints out the lines to be evaluated before evaluating them."},
    { { "explain", NULL},
        "Gives you information about commands, variables, constants and functions."},
    { { "cmod", NULL},
        "Changes how the modulus operator (%%) behaves with negative numbers. The default is to behave like the C programming language modulus, the other is slightly more flexible. For example, with the default setting:\t-340 %% 60 == -40; 340 %% -60 == 40; -340 %% -60 == -40When this setting is toggled, it behaves like this:\t-340 %% 60 == -40; 340 %% -60 == -20; -340 %% -60 == 20"},
    { { 0 }, NULL }
};

int iscmd(const char *str)
{   /*{{{*/
    size_t i;

    for (i = 0; commands[i].explanation; i++) {
        for (unsigned j=0; commands[i].names[j]; j++) {
            if (strcmp(commands[i].names[j], str) == 0) {
                return 1;
            }
        }
    }
    return 0;
} /*}}}*/

/* vim:set expandtab: */
