/*
 *  historyManager.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Mon Jan 21 2002.
 *  Copyright (c) 2001 Kyle Wheeler. All rights reserved.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>					   /* might make readline happy */

#include <gmp.h>
#include <mpfr.h>

#ifndef GUI
#include "config.h"
#endif
#include "historyManager.h"
#include "calculator.h"

#ifdef HAVE_READLINE_HISTORY
# if defined(HAVE_READLINE_HISTORY_H)
#  include <readline/history.h>
# elif defined(HAVE_HISTORY_H)
#  include <history.h>
# else /* ! defined(HAVE_HISTORY_H) */
extern void add_history();
extern int write_history();
extern int read_history();
extern int history_truncate_file(char *, int);
# endif	/* defined(HAVE_READLINE_HISTORY_H) */
/* no history */
#endif /* HAVE_READLINE_HISTORY */

/* Configuration Variables */
short allow_duplicates = 0;
short recalculate = 0;

/* Data Types */
struct entry
{
	char *exp;
	mpfr_t ans;
	unsigned int calc:1;
};

/* Private Variables */
static struct entry *history = NULL;
static unsigned long histlen = 0;

/* Private Functions */
static int all_calculated(void);
static void clear_calculated(void);

void clearHistory()
{
	if (histlen) {
		free(history);
		histlen = 0;
	}
}

void addToHistory(char *expression, mpfr_t answer)
{
#ifdef HAVE_READLINE_HISTORY
	add_history(expression);
#endif
	if (!histlen) {
		/* this is the beginning of a new history record */
		if (!conf.history_limit || conf.history_limit_len > 0) {
			history = malloc(sizeof(struct entry));
			if (!history)
				return;
			history->exp = strdup(expression);
			if (!history->exp) {
				free(history);
				return;
			}
			mpfr_init_set(history->ans, answer, GMP_RNDN);
			histlen = 1;
		}
	} else {						   /* a history exists */
		// eliminate duplicates
		if (allow_duplicates || strcmp(history[histlen - 1].exp, expression)) {
			if (!conf.history_limit || histlen < conf.history_limit_len) {
				/* history not too long, just add a new entry */
				struct entry *temp =
					realloc(history, sizeof(struct entry) * (histlen + 1));

				if (!temp) {
					// if it couldn't be realloced, try malloc and memcpy
					temp = malloc(sizeof(struct entry) * (histlen + 1));
					if (!temp)
						return;
					memcpy(temp, history, sizeof(struct entry) * histlen);
					free(history);
				}
				history = temp;
				if (expression)
					history[histlen].exp = strdup(expression);
				else
					history[histlen].exp = NULL;
				mpfr_init_set(history[histlen].ans, answer, GMP_RNDN);
				/* why is this if statement here? */
				if (!temp[histlen].exp)
					return;
				++histlen;
			} else {
				/* history too long */
				unsigned long i;

				if (history[0].exp)
					free(history[0].exp);
				for (i = 0; i < histlen - 1; ++i) {
					history[i].exp = history[i + 1].exp;
					mpfr_set(history[i].ans, history[i + 1].ans, GMP_RNDN);
					history[i].calc = history[i + 1].calc;
				}
				if (expression)
					history[histlen - 1].exp = strdup(expression);
				else
					history[histlen - 1].exp = NULL;
				mpfr_set(history[histlen - 1].ans, answer, GMP_RNDN);
				return;
			}
		}
	}
}

char *historynum(int step, int col)
{
	if (col == 1)
		return history[step].exp;
	else {
		static char *temp;

		if (recalculate) {
			parseme(history[step].ans, history[step].exp);
			history[step].calc = 1;
			if (all_calculated()) {
				recalculate = 0;
				clear_calculated();
			}
		}
		temp = print_this_result(history[step].ans);
		return (temp ? temp : "Not Enough Memory");
	}
}

static int all_calculated(void)
{
	unsigned long i, ret = 0;

	for (i = 0; i < histlen; ++i) {
		if (history[i].calc)
			++ret;
	}
	return (ret == histlen);
}

static void clear_calculated(void)
{
	unsigned long i;

	for (i = 0; i < histlen; ++i) {
		history[i].calc = 0;
	}
}

int historyLength(void)
{
	return histlen;
}
