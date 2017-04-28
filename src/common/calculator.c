#ifdef HAVE_CONFIG_H
# include "config.h"
#else
# define HAVE_MPFR_22
#endif
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>                      /* for HUGE_VAL */
#include <float.h>                     /* for DBL_EPSILON */
#include <ctype.h>                     /* for isalpha() */
#include <assert.h>

#include "output.h"

#ifndef isnan
# define isnan(x)                                   \
    (sizeof(x) == sizeof(long double) ? isnan_ld(x) \
     : sizeof(x) == sizeof(double) ? isnan_d(x)     \
     : isnan_f(x))
static inline int
isnan_f(float x)
{   /*{{{*/
    return x != x;
} /*}}}*/

static inline int
isnan_d(double x)
{   /*{{{*/
    return x != x;
} /*}}}*/

static inline int
isnan_ld(long double x)
{   /*{{{*/
    return x != x;
} /*}}}*/
#endif /* ifndef isnan */

#ifndef isinf
# define isinf(x)                                   \
    (sizeof(x) == sizeof(long double) ? isinf_ld(x) \
     : sizeof(x) == sizeof(double) ? isinf_d(x)     \
     : isinf_f(x))
static inline int
isinf_f(float x)
{   /*{{{*/
    return isnan(x - x);
} /*}}}*/

static inline int
isinf_d(double x)
{   /*{{{*/
    return isnan(x - x);
} /*}}}*/

static inline int
isinf_ld(long double x)
{   /*{{{*/
    return isnan(x - x);
} /*}}}*/
#endif /* ifndef isinf */

#if !defined(HAVE_CONFIG_H) || HAVE_STRING_H
# include <string.h>                   /* for memset() */
#else
# if !HAVE_STRCHR
#  define strchr  index
#  define strrchr rindex
# endif
char *strchr(), *strrchr();
#endif

#if !defined(HAVE_CONFIG_H) || TIME_WITH_SYS_TIME       /* for time() */
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include "number.h"

#include "uint32_max.h"
#include "calculator.h"
#include "variables.h"
#include "string_manip.h"
#include "files.h"
#include "number_formatting.h"
#include "add_commas.h"
#include "list.h"
#include "extract_vars.h"

/* variables everyone needs to get to */
Number last_answer;
char  *pretty_answer = NULL;

/* communication with the parser */
char         compute  = 1;
unsigned int sig_figs = UINT32_MAX;

/* communication with the frontend */
char         standard_output   = 1;
char         not_all_displayed = 0;
char        *pa                = NULL;
char        *last_input        = NULL;

struct _conf conf;

/*
 * These are declared here because they're not in any header files.
 * yyparse() is declared with an empty argument list so that it is
 * compatible with the generated C code from yacc/bison.
 * These two lines are taken from http://www.bgw.org/tutorials/programming/c/lex_yacc/main.c
 */
struct yy_buffer_state;
extern int   yyparse(void);
extern void *yy_scan_string(const char *);
extern void  yy_delete_buffer(struct yy_buffer_state *);

static int   recursion(char *str);
static int   find_recursion(char *);
static int   find_recursion_core(List);
static char *flatten(char *str);

void
parseme(const char *pthis)
{                                      /*{{{ */
    extern int   synerrors;
    short        numbers = 0;
    char        *sanitized;
    extern char *open_file;

    synerrors = 0;
    compute   = 1;
    sig_figs  = UINT32_MAX;

    Dprintf("parsing: %s\n", pthis);
    sanitized = (char *)strdup(pthis);

    /* Save a copy of the input */
    {
        extern int show_line_numbers;

        if (show_line_numbers) {
            if (last_input) {
                free(last_input);
            }
            last_input = strdup(pthis);
        }
    }
    /* Convert to standard notation (american comma and period) if there are
     * numbers */
    // are there numbers?
    {
        unsigned int i;

        for (i = 0; i < strlen(sanitized); ++i) {
            if (isdigit((int)(sanitized[i]))) {
                numbers = 1;
                break;
            }
        }
    }
    if (numbers) {
        unsigned int i;

        for (i = 0; i < strlen(sanitized); ++i) {
            if (((conf.thou_delimiter != '.') && (conf.dec_delimiter != '.') &&
                 (conf.in_thou_delimiter != '.') && (conf.in_dec_delimiter != '.') &&
                 (sanitized[i] == '.')) || ((conf.thou_delimiter != ',') &&
                                            (conf.in_thou_delimiter != ',') &&
                                            (conf.dec_delimiter != ',') &&
                                            (conf.in_dec_delimiter != ',') &&
                                            (sanitized[i] == ','))) {
                // throw an error
                report_error("Improperly formatted numbers! (%c,%c)\n",
                             conf.thou_delimiter, conf.dec_delimiter);
                synerrors = 1;
                break;
            } else if ((conf.in_thou_delimiter != 0) && (sanitized[i] == conf.in_thou_delimiter)) {
                sanitized[i] = ',';
            } else if ((conf.in_thou_delimiter == 0) && (sanitized[i] == conf.thou_delimiter) &&
                    (conf.thou_delimiter != ' ')) {
                sanitized[i] = ',';
            } else if ((conf.in_dec_delimiter != 0) && (sanitized[i] == conf.in_dec_delimiter)) {
                sanitized[i] = '.';
            } else if ((conf.in_dec_delimiter == 0) && (sanitized[i] == conf.dec_delimiter)) {
                sanitized[i] = '.';
            }
        }
    }

    /* Now, check for recursion */
    if (recursion(sanitized)) {
        goto exiting;
    }

    /* now resolve the variables */
    sanitized = flatten(sanitized);
    Dprintf("flattened: '%s'\n", sanitized);

    /* Sanitize the input (add a newline) */
    {
        char        *temp;
        unsigned int len = strlen(sanitized) + 3;
        temp = calloc(sizeof(char), len);
        if (!temp) {
            perror("resizing buffer");
            goto exiting;
        }
        snprintf(temp, len, "%s\n", sanitized);
        free(sanitized);
        sanitized = temp;
    }
    /* reset the position tracker */
    {
        extern int column;

        column = 0;
    }
    /* Evaluate the Expression
     */
    {
        struct yy_buffer_state *yy = yy_scan_string(sanitized);

        yyparse();
        yy_delete_buffer(yy);
    }

    if (open_file) {
        char *filename = open_file;
        int   retval;

        open_file = NULL;
        Dprintf("open_file\n");
        retval    = loadState(filename, 1);
        if (retval) {
            report_error("Could not load file (%s).",
                         (char *)strerror(retval));
        }
    }
exiting:
    /* exiting */
    free(sanitized);
}                                      /*}}} */

static size_t
find_alpha(const char *str)
{
    const size_t len = strlen(str);
    size_t       i   = 0;

    while ((i < len) && str[i] && !isalpha((int)str[i])) {
        switch (str[i]) {
            case '\\':
                do {
                    i++;
                } while ((i < len) && str[i] && isalpha((int)str[i]));
                break;
            case '\'':
            {
                char *tailquote = strchr(str + i + 1, '\'');
                if (tailquote == NULL) { return len; } else {
                    i = (tailquote) - (str);
                }
                break;
            }
            case '0':
                switch (str[i + 1]) {
                    case 'x': // hex numbers can contain letters; skip the whole thing
                        i += 2; // skip the '0x'
                        while ((i < len) && str[i] && isxdigit(str[i])) i++;
                        break;
                    case 'b': i += 2; break; // skip the '0b' binary prefix
                    default: i++; break;
                }
                break;
            default: i++; break;
        }
    }
    return i;
}

static char *
evaluate_var(const char    *varname,
             struct answer *aptr)
{
    struct answer a;
    char         *varvalue = NULL;

    // if it's a variable, evaluate it
    a = getvar_full(varname);
    if (!a.err) {                  // it is a var
        Number f;

        num_init(f);
        if (a.exp) {               // it is an expression
            parseme(a.exp);
            num_set(f, last_answer);
        } else {                   // it is a value
            num_set(f, a.val);
            num_free(a.val);
        }
        // get the number
        {
            char junk;

            // This value must fully reproduce the contents of f (thus, the -2 in arg 4)
            varvalue = num_to_str_complex(f, 10, 0, -2, 1, &junk);
        }
        num_free(f);
    } else {                       // not a known var: itza literal (e.g. cos)
        varvalue = (char *)strdup(varname);
    }
    *aptr = a;
    assert(varvalue != NULL);
    return varvalue;
}

static char *
extract_var(char   *str,
            size_t *len)
{
    const size_t max = strlen(str);
    size_t       i   = 0;
    char        *var;

    while (i < max &&
           (isalpha((int)str[i]) || str[i] == '_' ||
            str[i] == ':' || isdigit((int)str[i]))) {
        i++;
    }

    if (i == 0) {
        return NULL;
    }
    var    = malloc((i + 1) * sizeof(char));
    memcpy(var, str, i);
    var[i] = 0;
    if (len) { *len = i; }
    return var;
}

/* this function should probably stop flattening if it sees a comment, but
 * that's so rare (and hardly processor intensive) that it's not worth digging
 * at the moment */
static char *
flatten(char *str)
{                                      /*{{{ */
    char         *curs = str, *eov, *nstr;
    char         *varname, *varvalue;
    size_t        changedlen;
    struct answer a;
    char          standard_output_save = standard_output;

    standard_output = 0;

    if (*str == '\\') {
        standard_output = standard_output_save;
        return str;
    }
    curs = strchr(str, '=');
    if (!curs || !*curs || (*(curs + 1) == '=')) {
        curs = str;
    }

    while (curs && *curs) {
        // search for the first letter of a possible variable
        size_t max   = strlen(curs);
        size_t alpha = find_alpha(curs);
        if (alpha == max) {
            break;
        }
        curs = curs + alpha;
        // pull out that variable
        {
            size_t varlen = 0;

            varname = extract_var(curs, &varlen);
            eov     = curs + varlen;
        }

        varvalue = evaluate_var(varname, &a);
        assert(varvalue);
        {
            size_t nlen = strlen(varvalue);

            // now, put it back in the string
            // it is a var, and needs parenthesis
            changedlen = strlen(str) + nlen - strlen(varname) + 1;
            free(varname);
        }

        if (!a.err) {
            changedlen += 2;           // space for parens if it's a variable
        }
        nstr = malloc(changedlen);
        if (!nstr) {                   // not enough memory
            perror("flatten: ");
            exit(EXIT_FAILURE);
        }
        {
            char *fromstring = str;
            char *tostring   = nstr;

            // nstr is the new string, str is the input string
            while (fromstring != curs) {        // copy up to the curs (the beginning of the var name)
                *tostring = *fromstring;
                ++fromstring;
                ++tostring;
            }
            if (!a.err) {
                *tostring = '(';
                ++tostring;
            }
            fromstring = varvalue;
            while (fromstring && *fromstring) {
                *tostring = *fromstring;
                ++fromstring;
                ++tostring;
            }
            if (!a.err) {
                *tostring = ')';
                ++tostring;
            }
            curs       = tostring;
            fromstring = eov;
            while (fromstring && *fromstring) {
                *tostring = *fromstring;
                ++fromstring;
                ++tostring;
            }
            *tostring = 0;
            free(str);
            str       = nstr;
        }
        free(varvalue);
    }
    standard_output = standard_output_save;
    return str;
}                                      /*}}} */

static int
recursion(char *str)
{                                      /*{{{ */
    List  vlist  = NULL;
    int   retval = 0;
    char *righthand;

    // do not examine commands
    if (*str == '\\') {
        return 0;
    }
    // do not examine the left side of an assignment
    righthand = strchr(str, '=');
    if (!righthand || !*righthand || (*(righthand + 1) == '=')) {
        righthand = str;
    }
    vlist = extract_vars(righthand);
    while (listLen(vlist) > 0) {
        char *varname = (char *)getHeadOfList(vlist);

        if (retval == 0) {
            retval = find_recursion(varname);
        }
        free(varname);
    }
    return retval;
}                                      /*}}} */

static int
find_recursion(char *instring)
{                                      /*{{{ */
    List vl = NULL;
    int  retval;

    addToList(&vl, (char *)strdup(instring));
    retval = find_recursion_core(vl);
    free(getHeadOfList(vl));
    return retval;
}                                      /*}}} */

static int
find_recursion_core(List oldvars)
{                                      /*{{{ */
    List          newvars    = NULL;
    ListIterator  oldvarsIterator;
    int           retval     = 0;
    struct answer a;
    char         *newVarname = NULL;

    a = getvar_full((char *)peekAheadInList(oldvars));
    if (a.err) {
        return 0;
    }
    if (!a.exp) {
        num_free(a.val);
        return 0;
    }

    newvars         = extract_vars(a.exp);
    oldvarsIterator = getListIterator(oldvars);
    // for each variable in that expression (i.e. each entry in newvars)
    // see if we've seen it before (i.e. it's in oldvars)
    while (listLen(newvars) > 0) {
        newVarname = (char *)getHeadOfList(newvars);
        char *oldVarname;
        while ((oldVarname =
                    (char *)nextListElement(oldvarsIterator)) != NULL) {
            if (!strcmp(newVarname, oldVarname)) {
                report_error
                    ("%s was found twice in symbol descent. Recursive variables are not allowed.",
                    newVarname);
                // free the rest of the newvars list
                do {
                    free(newVarname);
                } while ((newVarname =
                              (char *)getHeadOfList(newvars)) != NULL);
                freeListIterator(oldvarsIterator);
                return 1;
            }
        }
        // now see if it has recursion
        addToListHead(&oldvars, newVarname);
        retval = find_recursion_core(oldvars);
        getHeadOfList(oldvars);
        resetListIterator(oldvarsIterator);
        free(newVarname);
        if (retval != 0) {
            break;
        }
    }
    // make sure newvars is empty (so all that memory gets freed)
    while ((newVarname = (char *)getHeadOfList(newvars)) != NULL) {
        free(newVarname);
    }
    freeListIterator(oldvarsIterator);
    return retval;
}                                      /*}}} */

void
report_error(const char *err_fmt,
             ...)
{                                      /*{{{ */
    extern char *errstring;
    extern int   errloc;
    extern int   column;
    extern int   lines;
    extern int   show_line_numbers;
    char        *tempstring;
    unsigned int len;
    va_list      ap;
    char        *this_error;

    va_start(ap, err_fmt);

    this_error = calloc(strlen(err_fmt) + 1000, sizeof(char));
    vsnprintf(this_error, strlen(err_fmt) + 1000, err_fmt, ap);
    len        = strlen(this_error) + 100;

    va_end(ap);

    /* okay, now this_error has the current error text in it */

    if (errstring) {
        len += strlen(errstring);
    }
    tempstring = calloc(len, sizeof(char));
    if (errstring) {
        if (show_line_numbers) {
            snprintf(tempstring, len, "%s\nError on line %i: %s", errstring,
                     lines, this_error);
        } else {
            snprintf(tempstring, len, "%s\n%s", errstring, this_error);
        }
        free(errstring);
    } else {
        if (show_line_numbers) {
            snprintf(tempstring, len, "Error on line %i: %s", lines,
                     this_error);
        } else {
            snprintf(tempstring, len, "%s", this_error);
        }
    }
    errstring = tempstring;
    free(this_error);
    if (errloc == -1) {
        errloc = column;
    }
}                                      /*}}} */

void
set_prettyanswer(const Number num)
{                                      /*{{{ */
    char *temp;

    Dprintf("set_prettyanswer\n");
    if (pretty_answer) {
        free(pretty_answer);
    }
    Dprintf("set_prettyanswer - call print_this_result\n");
    temp = print_this_result(num, standard_output, NULL, NULL);
    Dprintf("set_prettyanswer: %s\n", temp);
    if (temp) {
        pretty_answer = (char *)strdup(temp);
    } else {
        pretty_answer = NULL;
    }
    Dprintf("set_prettyanswer - done\n");
}                                      /*}}} */

static char *
print_this_result_dbl(const double result,
                      int          output,
                      char        *nad,
                      char       **es)
{                                      /*{{{ */
    char         format[10];
    static char *tmp;
    static char  pa_dyn         = 1;
    extern char *errstring;
    unsigned int decimal_places = 0;

    Dprintf("print_this_result_dbl(%f)\n", result);
    /* Build the "format" string, that will be used in an snprintf later */
    switch (conf.output_format) {      /*{{{ */
        case DECIMAL_FORMAT:
            if (pa_dyn) {
                tmp = realloc(pa, sizeof(char) * 310);
            } else {
                tmp    = pa = malloc(sizeof(char) * 310);
                pa_dyn = 1;
            }
            if (!tmp) {
                free(pa);
                pa     = "Not Enough Memory";
                pa_dyn = 0;
                return pa;
            } else {
                pa = tmp;
            }
            if (conf.precision > -1) {
                decimal_places = conf.precision;
                switch (conf.scientific) {
                    case never:
                        snprintf(format, 10, "%%1.%if", conf.precision);
                        break;
                    case always:
                        snprintf(format, 10, "%%1.%ie", conf.precision);
                        break;
                    case automatic:
                        snprintf(format, 10, "%%1.%ig", conf.precision);
                        break;
                }
                Dprintf("precision was specified as %i, format string is \"%s\"\n", conf.precision, format);
            } else {
                switch (conf.scientific) {
                    case never:
                        strncpy(format, "%f", 10);
                        break;
                    case always:
                        strncpy(format, "%e", 10);
                        break;
                    case automatic:
                        strncpy(format, "%g", 10);
                        break;
                }
                Dprintf("precision is automatic, format string is \"%s\"\n", format);
                if (fabs(result) < 10.0) {
                    decimal_places = 6;
                } else if (fabs(result) < 100.0) {
                    decimal_places = 4;
                } else if (fabs(result) < 1000.0) {
                    decimal_places = 3;
                } else if (fabs(result) < 10000.0) {
                    decimal_places = 2;
                } else if (fabs(result) < 100000.0) {
                    decimal_places = 1;
                } else {
                    decimal_places = 0;
                }
            }
            break;
        case OCTAL_FORMAT:
            if (pa_dyn) {
                tmp = realloc(pa, sizeof(char) * 14);
            } else {
                tmp    = pa = malloc(sizeof(char) * 14);
                pa_dyn = 1;
            }
            if (!tmp) {
                free(pa);
                pa     = "Not Enough Memory";
                pa_dyn = 0;
                return pa;
            } else {
                pa = tmp;
            }
            snprintf(format, 10, conf.print_prefixes ? "%%#o" : "%%o");
            break;
        case HEXADECIMAL_FORMAT:
            if (pa_dyn) {
                tmp = realloc(pa, sizeof(char) * 11);
            } else {
                tmp    = pa = malloc(sizeof(char) * 11);
                pa_dyn = 1;
            }
            if (!tmp) {
                free(pa);
                pa     = "Not Enough Memory";
                pa_dyn = 0;
                return pa;
            } else {
                pa = tmp;
            }
            snprintf(format, 10, conf.print_prefixes ? "%%#x" : "%%x");
            break;
        case BINARY_FORMAT:
            // Binary Format can't just use a format string, so
            // we have to handle it later
            if (pa_dyn) {
                free(pa);
            }
            pa     = NULL;
            pa_dyn = 1;
            break;
    }                                  /*}}} */

    if (isinf(result)) {
        // if it is infinity, print "Infinity", regardless of format
        if (pa_dyn) {
            tmp = realloc(pa, sizeof(char) * 11);
        } else {
            tmp    = pa = malloc(sizeof(char) * 11);
            pa_dyn = 1;
        }
        if (!tmp) {
            free(pa);
            pa     = "Not Enough Memory";
            pa_dyn = 0;
            return pa;
        } else {
            pa = tmp;
        }
        snprintf(pa, 11, "Infinity");
        not_all_displayed = 0;
    } else if (isnan(result)) {
        // if it is not a number, print "Not a Number", regardless of format
        if (pa_dyn) {
            tmp = realloc(pa, sizeof(char) * 13);
        } else {
            tmp    = pa = malloc(sizeof(char) * 13);
            pa_dyn = 1;
        }
        if (!tmp) {
            free(pa);
            pa     = "Not Enough Memory";
            pa_dyn = 0;
            return pa;
        } else {
            pa = tmp;
        }
        snprintf(pa, 13, "Not a Number");
        not_all_displayed = 0;
    } else {
        char *curs;

        Dprintf("normal numbers (format: %s)\n", format);
        switch (conf.output_format) {  /*{{{ */
            case DECIMAL_FORMAT:
            {
                double junk;

                Dprintf("fabs = %f, conf.scientific = %i, conf.print_ints = %i\n", fabs(modf(result, &junk)), conf.scientific, conf.print_ints);
                /* This is the big call */
                /* translation: if we don't have to handle the print_ints special case,
                 * then we can just use the existing format. */
                if ((fabs(modf(result, &junk)) != 0.0) ||
                    !conf.print_ints) {
                    snprintf(pa, 310, format, result);
                } else {
                    /* this is the print_ints special case
                     * (note that we strip trailing zeros) */
                    snprintf(pa, 310, "%1.0f", result);
                }
                Dprintf("pa (unlocalized): %s\n", pa);
                /* was it as good for you as it was for me?
                 * now, you must localize it */
                strswap('.', conf.dec_delimiter, pa);

                Dprintf("pa: %s\n", pa);
                switch (conf.rounding_indication) {
                    case SIMPLE_ROUNDING_INDICATION:
                        Dprintf("simple\n");
                        not_all_displayed =
                            (modf(result * pow(10, decimal_places), &junk)) ?
                            1 : 0;
                        break;
                    case SIG_FIG_ROUNDING_INDICATION:
                        Dprintf("sigfig\n");
                        if (sig_figs < UINT32_MAX) {
                            unsigned int t = count_digits(pa);

                            Dprintf("digits in pa: %u (%u)\n", t, sig_figs);
                            if ((pa[0] == '0') && (pa[1] != '\0')) {
                                --t;
                            } else if ((pa[0] == '-') && (pa[1] == '0')) {
                                --t;
                            }
                            not_all_displayed = (t < sig_figs);
                        } else {
                            not_all_displayed = 1;
                        }
                        break;
                    default:
                    case NO_ROUNDING_INDICATION:
                        Dprintf("none\n");
                        not_all_displayed = 0;
                        break;
                }
                break;
            }
            case HEXADECIMAL_FORMAT:
                curs = pa + (conf.print_prefixes ? 2 : 0);
                strswap('.', conf.dec_delimiter, pa);
                goto hexoct_body;
            case OCTAL_FORMAT:
                curs = pa + (conf.print_prefixes ? 1 : 0);
hexoct_body:
                {
                    long int temp = result;

                    snprintf(pa, 310, format, temp);
                    if (conf.rounding_indication ==
                        SIG_FIG_ROUNDING_INDICATION) {
                        if (sig_figs < UINT32_MAX) {
                            unsigned int t = 0;
                            while (curs && *curs) {
                                ++t;
                                ++curs;
                            }
                            not_all_displayed = (t < sig_figs);
                        } else {
                            not_all_displayed = 0;
                        }
                    } else {
                        not_all_displayed = 0;
                    }
                }
                strswap('.', conf.dec_delimiter, pa);
                break;
            case BINARY_FORMAT:
            {
                int i, place = -1;

                // if it is binary, format it, and print it
                // first, find the upper limit
                for (i = 1; place == -1; ++i) {
                    if (result < pow(2.0, i)) {
                        place = i - 1;
                    }
                }
                pa = calloc(sizeof(char),
                            (place + (conf.print_prefixes * 2) + 1));
                if (!pa) {
                    pa     = "Not Enough Memory";
                    pa_dyn = 0;
                    return pa;
                }
                if (conf.print_prefixes) {
                    pa[0] = '0';
                    pa[1] = 'b';
                }
                // print it
                {
                    double temp = result;
                    for (i = conf.print_prefixes * 2; place >= 0; ++i) {
                        double t = pow(2.0, place);

                        if (temp >= t) {
                            pa[i] = '1';
                            temp -= t;
                        } else {
                            pa[i] = '0';
                        }
                        --place;
                    }
                }
                pa[i + 1] = 0;

                if (sig_figs < UINT32_MAX) {
                    if (conf.rounding_indication ==
                        SIG_FIG_ROUNDING_INDICATION) {
                        not_all_displayed =
                            count_digits(pa + (conf.print_prefixes ? 2 : 0)) <
                            sig_figs;
                    } else {
                        not_all_displayed = 0;
                    }
                } else {
                    not_all_displayed = 0;
                }
                strswap('.', conf.dec_delimiter, pa);
            }                          // binary format
        }                              /*}}} */
    }                                  // if

    if (conf.print_commas) {
        char *str = add_commas(pa, conf.output_format);

        if (str) {
            free(pa);
            pa = str;
        }
    }

    if (output) {
        show_answer(errstring, not_all_displayed, pa);
    }
    if (nad) { *nad = not_all_displayed; }
    if (es) { *es = errstring; }

    return pa;
}                                      /*}}} */

char *
print_this_result(const Number result,
                  int          output,
                  char        *nad,
                  char       **es)
{                                      /*{{{ */
    extern char *errstring;
    unsigned int base = 0;

    Dprintf("print_this_result (%f) in format %i\n",
            num_get_d(result), conf.output_format);
    // output in the proper base and format
    switch (conf.output_format) {
        case HEXADECIMAL_FORMAT:
            base = 16;
            break;
        default:
        case DECIMAL_FORMAT:
            // if you want precision_guard and automatic precision,
            // then we have to go with the tried and true "double" method
            // ... unless it's an int and you want ints printed whole

            // I know that DBL_EPSILON can be calculated like so:
            // 2^(mpfr_get_prec(result)-1) HOWEVER, printf magically handles
            // numbers like 5.1 that I don't even wanna begin to think about
            if (conf.precision_guard && (conf.precision < 0)) {
                Dprintf("precision guard and automatic precision\n");
                if (!conf.print_ints || !is_int(result)) {
                    // XXX: this doesn't work for *huge* numbers, like 100!+0.1
                    Dprintf("no print_ints (%i) or it isn't an int (%i)\n", (int)conf.print_ints, (int)is_int(result));
                    // XXX: what is the following if() for?
                    // if (mpfr_get_d(result, GMP_RNDN) !=
                    // mpfr_get_si(result, GMP_RNDN)) {
                    double res = num_get_d(result);

                    if (fabs(res) < DBL_EPSILON) {
                        res = 0.0;
                    }
                    return print_this_result_dbl(res, output, nad, es);
                    // }
                }
            }
            base = 10;
            break;
        case OCTAL_FORMAT:
            base = 8;
            break;
        case BINARY_FORMAT:
            base = 2;
            break;
    }
    if (pa != NULL) {
        free(pa);
    }
    not_all_displayed = 0;
    pa                = num_to_str_complex(result, base, conf.scientific, conf.precision,
                                           conf.print_prefixes, &not_all_displayed);
    Dprintf("not_all_displayed = %i\n", not_all_displayed);

    /* now, decide whether it's been rounded or not */
    if (num_is_inf(result) || num_is_nan(result)) {
        // if it is infinity, it's all there ;)
        not_all_displayed = 0;
    } else if (not_all_displayed == 0) {
        /* rounding guess */
        switch (conf.rounding_indication) {
            case SIMPLE_ROUNDING_INDICATION:
            {
                char *pa2, junk;

                Dprintf("simple full\n");

                pa2 =
                    num_to_str_complex(result, base, conf.scientific, -2,
                                       conf.print_prefixes, &junk);
                not_all_displayed = (strlen(pa) < strlen(pa2));
                free(pa2);
                break;
            }
            case SIG_FIG_ROUNDING_INDICATION:
                /* sig_figs is how many we need to display */
                Dprintf("sigfig full\n");
                if (sig_figs < UINT32_MAX) {
                    unsigned int t = count_digits(pa);

                    Dprintf("digits in pa: %u (%u)\n", t, sig_figs);
                    not_all_displayed = (t < sig_figs);
                } else {
                    not_all_displayed = 0;
                }
                break;
            default:
            case NO_ROUNDING_INDICATION:
                Dprintf("none full\n");
                not_all_displayed = 0;
                break;
        }
    }
    if (conf.rounding_indication == NO_ROUNDING_INDICATION) {
        Dprintf("clearing rounding indication\n");
        not_all_displayed = 0;
    }
    strswap('.', conf.dec_delimiter, pa);
    // add commas
    if (conf.print_commas) {
        char *str = add_commas(pa, conf.output_format);

        if (str) {
            free(pa);
            pa = str;
        }
    }

    if (output) {
        show_answer(errstring, not_all_displayed, pa);
    }
    if (nad) { *nad = not_all_displayed; }
    if (es) { *es = errstring; }

    return pa;
}                                      /*}}} */

void
simple_exp(Number                output,
           const Number          first,
           const enum operations op,
           const Number          second)
{                                      /*{{{ */
    if (compute) {
        Number temp;

        num_init(temp);
        Dprintf("simple_exp: %f %i %f\n", num_get_d(first), op,
                num_get_d(second));

        switch (op) {
            default:
                num_set_d(output, 0.0);
                break;
            case wequal:
                num_set_ui(output, num_is_equal(first, second));
                break;
            case wnequal:
                num_set_ui(output, !num_is_equal(first, second));
                break;
            case wgt:
                num_set_ui(output, num_is_greater(first, second));
                break;
            case wlt:
                num_set_ui(output, num_is_less(first, second));
                break;
            case wgeq:
                num_set_ui(output, num_is_greaterequal(first, second));
                break;
            case wleq:
                num_set_ui(output, num_is_lessequal(first, second));
                break;
            case wplus:
                num_add(output, first, second);
                break;
            case wminus:
                num_sub(output, first, second);
                break;
            case wmult:
                num_mul(output, first, second);
                break;
            case wdiv:
                num_div(output, first, second);
                break;
            case wpow:
                num_pow(output, first, second);
                break;
            case wor:
                num_set_ui(output, (!num_is_zero(first)) ||
                           (!num_is_zero(second)));
                break;
            case wand:
                num_set_ui(output, (!num_is_zero(first)) &&
                           (!num_is_zero(second)));
                break;
            case wbor:
                num_bor(output, first, second);
                break;
            case wband:
                num_band(output, first, second);
                break;
            case wbxor:
                num_bxor(output, first, second);
                break;
            case wlshft:
                num_set_ui(temp, 2);
                num_pow(temp, temp, second);
                num_mul(output, first, temp);
                break;
            case wrshft:
                num_set_ui(temp, 2);
                num_pow(temp, temp, second);
                num_div(output, first, temp);
                if (is_int(first)) {
                    num_trunc(output, output);
                }
                break;
            case wmod:
                if (num_is_zero(second)) {
                    num_set_nan(output);
                } else {
                    /* divide, round to zero, multiply, subtract
                     *
                     * in essence, find the value x in the equation:
                     * first = second * temp + x */
                    num_div(output, first, second);
                    if (conf.c_style_mod) {
                        num_rintz(output, output);      // makes zeros work
                    } else {
                        if (num_sign(first) >= 0) {
                            num_floor(output, output);
                        } else {
                            num_ceil(output, output);
                        }
                    }
                    num_mul(output, output, second);
                    num_sub(output, first, output);
                }
                break;
        }
        Dprintf("returns: %f\n", num_get_d(output));
        num_free(temp);
        return;
    } else {
        num_set_ui(output, 0);
        return;
    }
}                                      /*}}} */

void
uber_function(Number               output,
              const enum functions func,
              Number               input)
{                                      /*{{{ */
    if (compute) {
        Number temp;

        num_init(temp);
        if (!conf.use_radians) {
            switch (func) {
                case wsin:
                case wcos:
                case wtan:
                case wcot:
                case wsec:
                case wcsc:
                    num_const_pi(temp);
                    num_mul(input, input, temp);
                    num_div_ui(input, input, 180);
                    break;
                case wasin:
                case wacos:
                case watan:
                case wacot:
                case wasec:
                case wacsc:
                    num_const_pi(temp);
                    num_pow_si(temp, temp, -1);
                    num_mul_ui(temp, temp, 180);
                    break;
                default:
                    break;
            }
        }
        switch (func) {
            case wsin:
                num_sin(output, input);
                break;
            case wcos:
                num_cos(output, input);
                break;
            case wtan:
                num_tan(output, input);
                break;
            case wcot:
                num_cot(output, input);
                break;
            case wsec:
                num_sec(output, input);
                break;
            case wcsc:
                num_csc(output, input);
                break;
            case wasin:
                num_asin(output, input);
                if (!conf.use_radians) {
                    num_mul(output, output, temp);
                }
                break;
            case wacos:
                num_acos(output, input);
                if (!conf.use_radians) {
                    num_mul(output, output, temp);
                }
                break;
            case watan:
                num_atan(output, input);
                if (!conf.use_radians) {
                    num_mul(output, output, temp);
                }
                break;
            case wacot:
                num_pow_si(output, input, -1);
                num_atan(output, output);
                if (!conf.use_radians) {
                    num_mul(output, output, temp);
                }
                break;
            case wasec:
                num_pow_si(output, input, -1);
                num_acos(output, output);
                if (!conf.use_radians) {
                    num_mul(output, output, temp);
                }
                break;
            case wacsc:
                num_pow_si(output, input, -1);
                num_asin(output, output);
                if (!conf.use_radians) {
                    num_mul(output, output, temp);
                }
                break;
            case wsinh:
                num_sinh(output, input);
                break;
            case wcosh:
                num_cosh(output, input);
                break;
            case wtanh:
                num_tanh(output, input);
                break;
            case wcoth:
                num_coth(output, input);
                break;
            case wsech:
                num_sech(output, input);
                break;
            case wcsch:
                num_csch(output, input);
                break;
            case wasinh:
                num_asinh(output, input);
                break;
            case wacosh:
                num_acosh(output, input);
                break;
            case watanh:
                num_atanh(output, input);
                break;
            case wacoth:
                num_acoth(output, input);
                break;
            case wasech:
                num_asech(output, input);
                break;
            case wacsch:
                num_acsch(output, input);
                break;
            case wlog:
                num_log10(output, input);
                break;
            case wlogtwo:
                num_log2(output, input);
                break;
            case wln:
                num_log(output, input);
                break;
            case wround:
                num_rint(output, input);
                break;
            case wneg:
                num_neg(output, input);
                break;
            case wnot:
                num_set_ui(output, num_is_zero(input));
                break;
            case wabs:
                num_abs(output, input);
                break;
            case wsqrt:
                num_sqrt(output, input);
                break;
            case wfloor:
                num_floor(output, input);
                break;
            case wceil:
                num_ceil(output, input);
                break;
            case wrand:
                while (num_random(output) != 0) ;
                num_mul(output, output, input);
                if (num_cmp_si(input, 0) < 0) {
                    num_mul_si(output, output, -1);
                }
                break;
            case wirand:
                while (num_random(output) != 0) ;
                num_mul(output, output, input);
                if (num_cmp_si(input, 0) < 0) {
                    num_mul_si(output, output, -1);
                }
                num_rint(output, output);
                break;
            case wcbrt:
                num_cbrt(output, input);
                break;
            case wexp:
                num_exp(output, input);
                break;
            case wfact:
                num_factorial(output, num_get_ui(input));
                break;
            case wcomp:
                num_comp(output, input);
                break;
#ifdef HAVE_MPFR_22
            case weint:
                num_eint(output, input);
                break;
#endif
            case wgamma:
                num_gamma(output, input);
                break;
#ifdef HAVE_MPFR_22
            case wlngamma:
                num_lngamma(output, input);
                break;
#endif
            case wzeta:
                num_zeta(output, input);
                break;
            case wsinc:
                num_sinc(output, input);
                break;
            case wbnot:
                num_bnot(output, input);
                break;
            default:
                num_set(output, input);
                break;
        }
        num_free(temp);
        return;
    } else {
        num_set_ui(output, 0);
        return;
    }
}                                      /*}}} */

char *
output_string(const unsigned int o)
{                                      /*{{{ */
    switch (o) {
        case HEXADECIMAL_FORMAT:
            return "hexadecimal format (0xf)";

        case OCTAL_FORMAT:
            return "octal format (08)       ";

        case BINARY_FORMAT:
            return "binary format (0b1)     ";

        case DECIMAL_FORMAT:
            return "decimal format (9)      ";

        default:
            return "error, unknown format   ";
    }
}                                      /*}}} */

/* vim:set expandtab: */
