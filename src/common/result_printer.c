//
// Created by Kyle Wheeler on 4/26/18.
//
#if HAVE_CONFIG_H
# include "config.h"
#endif

/* System Libraries */
#include <math.h> /* for pow() */
#include <stdbool.h>
#include <stdint.h> /* for UINT32_MAX */
#include <stdio.h> /* for snprintf() */
#include <stdlib.h>
#include <string.h>

/* My header */
// Included after system libraries so that mpfr.h properly defines mpfr_fprintf
#include "result_printer.h"

/* Internal Headers */
#include "number.h"
#include "conf.h"
#include "calculator.h"
#include "number_formatting.h"
#include "add_commas.h"
#include "string_manip.h"

/* Injected Dependencies */
static void
(*show_answer_internal)(char *err, bool uncertain, char *answer);
static Number *last_answer_ref = NULL;

/* *************
 * Local state *
 * ************* */
/** The most recently calculated answer ('a') */
static char  *pretty_answer        = NULL;
static char  *formatted_number     = NULL;
static bool   formatted_number_dyn = true;

void
init_resultprinter(void (*sa)(char*,bool,char*), Number *la)
{
    show_answer_internal = sa;
    last_answer_ref = la;
    pretty_answer = NULL;
}

void
term_resultprinter(void)
{
    if (pretty_answer) {
        free(pretty_answer);
        pretty_answer = NULL;
    }
    last_answer_ref = NULL;
}

void show_answer(char *err, bool uncertain, char *answer)
{
    if (show_answer_internal) {
        show_answer_internal(err, uncertain, answer);
    }
}

const char *get_last_answer_str(void)
{
    return (const char *)pretty_answer;
}

void reset_last_answer_str(void)
{
    char *temp = NULL;

    Dprintf("reset_last_answer_str\n");
    if (pretty_answer) {
        free(pretty_answer);
    }
    Dprintf("reset_last_answer_str - call print_this_result\n");
    if (last_answer_ref != NULL) {
        temp = print_this_result(*last_answer_ref, standard_output, sig_figs, NULL, NULL);
    }
    Dprintf("reset_last_answer_str: %s\n", temp);
    if (temp) {
        pretty_answer = strdup(temp);
    } else {
        pretty_answer = NULL;
    }
    Dprintf("reset_last_answer_str - done\n");
}

#define MAGNITUDE_UNDER(n, x) ((num_cmp_d((n), (x)) < 0) || (num_cmp_d((n), -(x)) > 0))

static char *
print_this_result_printf(const Number result,
                         bool         output,
                         unsigned int sig_figs,
                         bool        *nad,
                         char       **es)
{                                      /*{{{ */
    char          format[10];
    const conf_t *conf = getConf();
    static char  *tmp;
    extern char  *errstring;
    unsigned int  decimal_places = 0;

    if (conf->output_format != DECIMAL_FORMAT) {
        Dprintf("attempted print_this_result_printf for a non-decimal format (%i)",
                conf->output_format);
    }
    /* Build the "format" string, that will be used in an snprintf later */
    if (formatted_number_dyn) {
        tmp = realloc(formatted_number, sizeof(char) * 310);
    } else {
        tmp    = formatted_number = malloc(sizeof(char) * 310);
        formatted_number_dyn = true;
    }
    if (!tmp) {
        free(formatted_number);
        formatted_number     = "Not Enough Memory";
        formatted_number_dyn = false;
        return formatted_number;
    } else {
        formatted_number = tmp;
    }
    if (conf->precision > -1) {
        decimal_places = (unsigned)(conf->precision);
        switch (conf->scientific) {
            case never:
                snprintf(format, 10, "%%1.%iRNf", conf->precision);
                break;
            case always:
                snprintf(format, 10, "%%1.%iRNe", conf->precision);
                break;
            case automatic:
                snprintf(format, 10, "%%1.%iRNg", conf->precision);
                break;
        }
        Dprintf("precision was specified as %i, format string is \"%s\"\n", conf->precision, format);
    } else {
        switch (conf->scientific) {
            case never:
                strncpy(format, "%RNf", 10);
                break;
            case always:
                strncpy(format, "%RNe", 10);
                break;
            case automatic:
                strncpy(format, "%RNg", 10);
                break;
        }
        Dprintf("precision is automatic, format string is \"%s\"\n", format);
        if (MAGNITUDE_UNDER(result, 10.0)) {
            decimal_places = 6;
        } else if (MAGNITUDE_UNDER(result, 100.0)) {
            decimal_places = 4;
        } else if (MAGNITUDE_UNDER(result, 1000.0)) {
            decimal_places = 3;
        } else if (MAGNITUDE_UNDER(result, 10000.0)) {
            decimal_places = 2;
        } else if (MAGNITUDE_UNDER(result, 100000.0)) {
            decimal_places = 1;
        } else {
            decimal_places = 0;
        }
    }

    if (num_is_inf(result)) {
        // if it is infinity, print "@Inf@", regardless of format
        if (formatted_number_dyn) {
            tmp = realloc(formatted_number, sizeof(char) * 11);
        } else {
            tmp    = formatted_number = malloc(sizeof(char) * 11);
            formatted_number_dyn = true;
        }
        if (!tmp) {
            free(formatted_number);
            formatted_number     = "Not Enough Memory";
            formatted_number_dyn = false;
            return formatted_number;
        } else {
            formatted_number = tmp;
        }
        if (num_sign(result) > 0) {
            snprintf(formatted_number, 11, INF_STRING);
        } else {
            snprintf(formatted_number, 11, "-" INF_STRING);
        }
        not_all_displayed = false;
    } else if (num_is_nan(result)) {
        // if it is not a number, print "@NaN@", regardless of format
        if (formatted_number_dyn) {
            tmp = realloc(formatted_number, sizeof(char) * 13);
        } else {
            tmp    = formatted_number = malloc(sizeof(char) * 13);
            formatted_number_dyn = true;
        }
        if (!tmp) {
            free(formatted_number);
            formatted_number     = "Not Enough Memory";
            formatted_number_dyn = false;
            return formatted_number;
        } else {
            formatted_number = tmp;
        }
        snprintf(formatted_number, 13, NAN_STRING);
        not_all_displayed = false;
    } else {
        Dprintf("normal numbers (format: %s)\n", format);

        NUM_DECL_INIT(absfracpart);
        NUM_DECL_INIT(fracpart);
        num_modf(absfracpart /*this part will be discarded in the next line */,
                 fracpart, result);
        num_abs(absfracpart, fracpart);

        Dprintf("fabs = %Rf, conf->scientific = %i, conf->print_ints = %i\n", absfracpart, conf->scientific, conf->print_ints);
        /* This is the big call */
        /* translation: if we don't have to handle the print_ints special case,
         * then we can just use the existing format. */
        Dprintf("%s (%RNf)... absfracpart:%RNf (%i) pi:%i\n", format, result, absfracpart,
                num_is_zero(absfracpart), !conf->print_ints);
        if (!num_is_zero(absfracpart) || !conf->print_ints) {
            Dprintf("general case\n");
            num_snprintf(formatted_number, 310, format, result);
        } else {
            Dprintf("Print_ints special case!\n");
            /* this is the print_ints special case
             * (note that we strip trailing zeros) */
            num_snprintf(formatted_number, 310, "%1.0RNf", result);
        }
        Dprintf("formatted_number (unlocalized): %s\n", formatted_number);
        /* was it as good for you as it was for me?
         * now, you must localize it */
        strswap('.', conf->dec_delimiter, formatted_number);

        Dprintf("formatted_number: %s\n", formatted_number);
        switch (conf->rounding_indication) {
            case SIMPLE_ROUNDING_INDICATION:
                Dprintf("simple\n");
                num_mul_d(fracpart, result, pow(10, decimal_places));
                num_modf(/* ignored */ absfracpart, fracpart, fracpart);
                not_all_displayed = !(bool)num_is_zero(fracpart);
                Dprintf("not_all_displayed = %i because %Rg is (not?) zero\n", not_all_displayed,
                        fracpart);
                break;
            case SIG_FIG_ROUNDING_INDICATION:
                Dprintf("sigfig\n");
                if (sig_figs < UINT32_MAX) {
                    unsigned int t = count_digits(formatted_number);

                    Dprintf("digits in formatted_number: %u (%u)\n", t, sig_figs);
                    if ((formatted_number[0] == '0') && (formatted_number[1] != '\0')) {
                        --t;
                    } else if ((formatted_number[0] == '-') && (formatted_number[1] == '0')) {
                        --t;
                    }
                    not_all_displayed = (t < sig_figs);
                } else {
                    not_all_displayed = true;
                }
                break;
            default:
            case NO_ROUNDING_INDICATION:
                Dprintf("none\n");
                not_all_displayed = false;
                break;
        }
    }                                  // if

    if (conf->print_commas) {
        char *str = add_commas(formatted_number, conf->output_format);

        if (str) {
            free(formatted_number);
            formatted_number = str;
        }
    }

    if (output) {
        show_answer(errstring, not_all_displayed, formatted_number);
    }
    if (nad) { *nad = not_all_displayed; }
    if (es) { *es = errstring; }

    return formatted_number;
}                                      /*}}} */

static void
roundToZeroIfLessThanEpsilon(Number *guardedResult, const Number result)
{
    NUM_DECL_INIT(epsilon);
    NUM_DECL_INIT(absresult);

    // Estimating epsilon for current precision, per Wikipedia
    // https://en.wikipedia.org/wiki/Machine_epsilon
    // 2^-(precision-1)
    long prec = num_get_default_prec() - 1;
    prec *= -1;
    num_set_ui(epsilon, 2);
    num_pow_si(epsilon, epsilon, prec);

    // Updating epsilon for the expression
    // XXX: this *should* be done via error estimation, but that's complicated, and should be
    // done as part of parsing. So... we inflate epsilon somewhat here to hopefully be "close
    // enough". Value chosen to be big enough to get all current tests to pass (and no larger).
    // Yes, this is a bit of a hack.
    num_mul_ui(epsilon, epsilon, 8);

    num_abs(absresult, result);
    Dprintf("  mpfr epsilon: %RNg\n", epsilon);
    Dprintf("    abs result: %RNg\n", absresult);

    if (num_is_lessequal(absresult, epsilon)) {
        Dprintf("abs result (%RNg) less than epsilon (%RNg)! "
                "Coercing to zero...\n", absresult, epsilon);
        num_set_ui(*guardedResult, 0);
    } else {
        num_set(*guardedResult, result);
    }
}

char *
print_this_result(const Number result,
                  bool         output,
                  unsigned int sig_figs,
                  bool        *nad,
                  char       **es)
{                                      /*{{{ */
    extern char  *errstring;
    unsigned int  base = 0;
    const conf_t *conf = getConf();

    Dprintf("print_this_result (%Rf) in format %i (sig_figs:%u)\n",
            result, conf->output_format, sig_figs);
    // output in the proper base and format
    switch (conf->output_format) {
        case HEXADECIMAL_FORMAT:
            base = 16;
            break;
        default:
        case DECIMAL_FORMAT:
            // if you want precision_guard and automatic precision,
            // then we have to go with the tried and true "printf" method
            // ... unless it's an int and you want ints printed whole
            if (conf->precision_guard && (conf->precision < 0)) {
                Dprintf("precision guard and automatic precision\n");
                if (!conf->print_ints || !is_int(result)) {
                    Dprintf("no print_ints (%i) or it isn't an int (%i)\n", (int)conf->print_ints, (int)is_int(result));
                    NUM_DECL_INIT(guardedResult);
                    roundToZeroIfLessThanEpsilon(&guardedResult, result);
                    return print_this_result_printf(guardedResult, output, sig_figs, nad, es);
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
    if (formatted_number != NULL) {
        free(formatted_number);
    }
    not_all_displayed = false;
    formatted_number  = num_to_str_complex(result, base, conf->scientific, conf->precision,
                                           conf->print_prefixes, &not_all_displayed);
    Dprintf("not_all_displayed = %i\n", not_all_displayed);

    /* now, decide whether it's been rounded or not */
    if (num_is_inf(result) || num_is_nan(result)) {
        // if it is infinity, it's all there ;)
        not_all_displayed = false;
    } else if (not_all_displayed == false) {
        /* rounding guess */
        switch (conf->rounding_indication) {
            case SIMPLE_ROUNDING_INDICATION:
            {
                char *pa2;

                Dprintf("simple full\n");
                pa2 = num_to_str_complex(result, base, conf->scientific, -2,
                                         conf->print_prefixes, NULL);
                not_all_displayed = (strlen(formatted_number) < strlen(pa2));
                free(pa2);
                break;
            }
            case SIG_FIG_ROUNDING_INDICATION:
                /* sig_figs is how many we need to display */
                Dprintf("sigfig full (%u)\n", sig_figs);
                if (sig_figs < UINT32_MAX) {
                    unsigned int t = count_digits(formatted_number);

                    Dprintf("digits in formatted_number: %u (%u)\n", t, sig_figs);
                    not_all_displayed = (t < sig_figs);
                } else {
                    not_all_displayed = false;
                }
                break;
            default:
            case NO_ROUNDING_INDICATION:
                Dprintf("none full\n");
                not_all_displayed = false;
                break;
        }
    }
    if (conf->rounding_indication == NO_ROUNDING_INDICATION) {
        Dprintf("clearing rounding indication\n");
        not_all_displayed = false;
    }
    strswap('.', conf->dec_delimiter, formatted_number);
    // add commas
    if (conf->print_commas) {
        char *str = add_commas(formatted_number, conf->output_format);

        if (str) {
            free(formatted_number);
            formatted_number = str;
        }
    }

    if (output) { show_answer(errstring, not_all_displayed, formatted_number); }
    if (nad) { *nad = not_all_displayed; }
    if (es) { *es = errstring; }

    return formatted_number;
}                                      /*}}} */
