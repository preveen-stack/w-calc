#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* System Headers */
#include <string.h>

/* Internal Headers */
#include "iscmd.h"

static const char *commands[] = {
    "b", "bin", "binary", "store", "q", "d", "dec", "decimal",
    "delim",
    "dsep", "e", "eng", "engineering", "cons", "conservative",
    "h", "hex", "hexadecimal", "x", "help", "hlimit", "ints",
    "li", "list", "listvars", "o", "oct", "octal", "open", "p",
    "pre", "prefix", "prefixes", "pref", "prefs", "preferences",
    "r", "rad", "radians", "rou", "round", "rounding", "re",
    "remember", "remember_errors", "bits", "save", "tsep", "c",
    "convert", "conv", "base", "verbose", "explain", "cmod",
    0
};

const char *qcommands[] = {
    "\\b", "\\bin", "\\binary", "\\store", "\\q", "\\d", "\\dec", "\\decimal",
    "\\delim",
    "\\dsep", "\\e", "\\eng", "\\engineering", "\\cons", "\\conservative",
    "\\h", "\\hex", "\\hexadecimal", "\\x", "\\help", "\\hlimit", "\\ints",
    "\\li", "\\list", "\\listvars", "\\o", "\\oct", "\\octal", "\\open", "\\p",
    "\\pre", "\\prefix", "\\prefixes", "\\pref", "\\prefs", "\\preferences",
    "\\r", "\\rad", "\\radians", "\\rou", "\\round", "\\rounding", "\\re",
    "\\remember", "\\remember_errors", "\\bits", "\\save", "\\tsep", "\\c",
    "\\convert", "\\conv", "\\base", "\\verbose", "\\explain", "\\cmod",
    0
};

int iscmd(const char *str)
{   /*{{{*/
    size_t i;

    for (i = 0; commands[i]; i++) {
        if (strcmp(commands[i], str) == 0) {
            return 1;
        }
    }
    return 0;
} /*}}}*/

/* vim:set expandtab: */
