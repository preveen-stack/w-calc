/*
 *  historyManager.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Mon Jan 21 2002.
 *  Copyright (c) 2001 Kyle Wheeler. All rights reserved.
 *
 */

#include "historyManager.h"
#include "calculator.h"

/* Configuration Variables */
short allow_duplicates = 0;

/* Data Types */
struct entry {
	char * exp;
	double ans;
};

/* Private Variables */
static struct entry *history = NULL;
static int histlen = 0;

void addToHistory(char * expression, double answer)
{
	if (! histlen) {
		history = malloc(sizeof(struct entry));
		if (! history) return;
		history->exp = strdup(expression);
		history->ans = answer;
		if (! history->exp) {
			free(history);
			return;
		}
		histlen = 1;
	} else {
		// eliminate duplicates
		if (allow_duplicates || strcmp(history[histlen-1].exp,expression)) {
			struct entry *temp = realloc(history, sizeof(struct entry) * (histlen+1));
			if (! temp) {
				// if it couldn't be realloced, try malloc and memcpy
				temp = malloc(sizeof(struct entry)*(histlen+1));
				if (! temp) return;
				memcpy(temp,history,sizeof(struct entry)*histlen);
				free(history);
			}
			history = temp;
			history[histlen].exp = strdup(expression);
			history[histlen].ans = answer;
			if (! temp[histlen].exp) return;
			++histlen;
		}
	}
}

char * historynum (int step, int col)
{
	if (col == 1)
		return history[step].exp;
	else {
		return print_this_result(history[step].ans);
	}
}

int history_length (void)
{
	return histlen;
}