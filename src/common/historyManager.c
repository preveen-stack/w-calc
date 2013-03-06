/*
 *  historyManager.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Mon Jan 21 2002.
 *  Copyright (c) 2001 Kyle Wheeler. All rights reserved.
 *
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* System Headers */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>                     /* might make readline happy */

/* Internal Headers */
#include "number.h"
#include "historyManager.h"
#include "calculator.h"

#ifdef HAVE_READLINE_HISTORY
# if defined(HAVE_READLINE_HISTORY_H)
#  include <readline/history.h>
# elif defined(HAVE_HISTORY_H)
#  include <history.h>
# else /* ! defined(HAVE_HISTORY_H) */
extern void add_history();
extern int  write_history();
extern int  read_history();
# endif /* defined(HAVE_READLINE_HISTORY_H) */
/* no history */
#endif /* HAVE_READLINE_HISTORY */
#ifdef MEMWATCH
# include "memwatch.h"
#endif

/* Configuration Variables */
short allow_duplicates = 0;
short recalculate      = 0;

/* Data Types */
struct entry {
    char        *exp;
    Number       ans;
    unsigned int calc : 1;
};

/* Private Variables */
static struct entry *history = NULL;
static size_t        histlen = 0;

/* Private Functions */
static int  all_calculated(void);
static void clear_calculated(void);

void clearHistory()
{   /*{{{*/
    unsigned int i;

    for (i = 0; i < histlen; i++) {
        free(history[i].exp);
        num_free(history[i].ans);
    }
    if (histlen) {
        free(history);
        histlen = 0;
    }
} /*}}}*/

void addToHistory(char  *expression,
                  Number answer)
{   /*{{{*/
#if defined(HAVE_READLINE_HISTORY)
    add_history(expression);
#endif
    if (!histlen) {
        /* this is the beginning of a new history record */
        if (!conf.history_limit || (conf.history_limit_len > 0)) {
            history = malloc(sizeof(struct entry));
            if (!history) {
                return;
            }
            history->exp = strdup(expression);
            if (!history->exp) {
                free(history);
                return;
            }
            num_init_set(history->ans, answer);
            histlen = 1;
        }
    } else {                           /* a history exists */
        // eliminate duplicates
        if (allow_duplicates || strcmp(history[histlen - 1].exp, expression)) {
            if (!conf.history_limit || (histlen < conf.history_limit_len)) {
                /* history not too long, just add a new entry */
                struct entry *temp =
                    realloc(history, sizeof(struct entry) * (histlen + 1));

                if (!temp) {
                    // if it couldn't be realloced, try malloc and memcpy
                    temp = malloc(sizeof(struct entry) * (histlen + 1));
                    if (!temp) {
                        return;
                    }
                    memcpy(temp, history, sizeof(struct entry) * histlen);
                    free(history);
                }
                history = temp;
                if (expression) {
                    history[histlen].exp = strdup(expression);
                } else {
                    history[histlen].exp = NULL;
                }
                num_init_set(history[histlen].ans, answer);
                /* why is this if statement here? */
                if (!temp[histlen].exp) {
                    return;
                }
                ++histlen;
            } else {
                /* history too long */
                unsigned long i;

                if (history[0].exp) {
                    free(history[0].exp);
                }
                for (i = 0; i < histlen - 1; ++i) {
                    history[i].exp = history[i + 1].exp;
                    num_set(history[i].ans, history[i + 1].ans);
                    history[i].calc = history[i + 1].calc;
                }
                if (expression) {
                    history[histlen - 1].exp = strdup(expression);
                } else {
                    history[histlen - 1].exp = NULL;
                }
                num_set(history[histlen - 1].ans, answer);
                return;
            }
        }
    }
} /*}}}*/

char *historynum(int step,
                 int col)
{   /*{{{*/
    if (col == 1) {
        return history[step].exp;
    } else {
        static char *temp;

        if (recalculate) {
            parseme(history[step].exp);
            num_set(history[step].ans, last_answer);
            history[step].calc = 1;
            if (all_calculated()) {
                recalculate = 0;
                clear_calculated();
            }
        }
        temp = print_this_result(history[step].ans);
        return (temp ? temp : "Not Enough Memory");
    }
} /*}}}*/

static int all_calculated(void)
{   /*{{{*/
    unsigned long i, ret = 0;

    for (i = 0; i < histlen; ++i) {
        if (history[i].calc) {
            ++ret;
        }
    }
    return (ret == histlen);
} /*}}}*/

static void clear_calculated(void)
{   /*{{{*/
    unsigned long i;

    for (i = 0; i < histlen; ++i) {
        history[i].calc = 0;
    }
} /*}}}*/

size_t historyLength(void)
{   /*{{{*/
    return histlen;
} /*}}}*/

/* vim:set expandtab: */
