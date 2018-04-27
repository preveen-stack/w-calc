//
// Created by Kyle Wheeler on 4/26/18.
//

/* My header */
#include "result_printer.h"

/* Internal Headers */
#include "number.h"
#include <conf.h>
#include <calculator.h>

/* System Libraries */
#include <stdlib.h>
#include <string.h>

/* Injected Dependencies */
static void
(*show_answer_internal)(char *err, int  uncertain, char *answer);
static Number *last_answer_ref = NULL;

/* *************
 * Local state *
 * ************* */
/** The most recently calculated answer ('a') */
static char  *pretty_answer = NULL;

void
init_resultprinter(void (*sa)(char*,int,char*), Number *la)
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

void show_answer(char *err, int  uncertain, char *answer)
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
        temp = print_this_result(*last_answer_ref, standard_output, NULL, NULL);
    }
    Dprintf("reset_last_answer_str: %s\n", temp);
    if (temp) {
        pretty_answer = strdup(temp);
    } else {
        pretty_answer = NULL;
    }
    Dprintf("reset_last_answer_str - done\n");
}

