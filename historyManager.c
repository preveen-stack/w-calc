/*
 *  historyManager.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Mon Jan 21 2002.
 *  Copyright (c) 2001 Kyle Wheeler. All rights reserved.
 *
 */

#include "historyManager.h"

/* Configuration Variables */
short allow_duplicates = 0;

/* Private Variables */
static char **history = NULL;
static int histlen = 0;

void addToHistory(char * expression)
{
	if (! histlen) {
		history = malloc(sizeof(char *));
		if (! history) return;
		*history = strdup(expression);
		if (! *history) return;
		histlen = 1;
	} else {
		// eliminate duplicates
		if (allow_duplicates || strcmp(history[histlen-1],expression)) {
			char **temp = realloc(history, sizeof(char *) * (histlen+1));
			if (! temp) {
				// if it couldn't be realloced, try malloc and memcpy
				temp = malloc(sizeof(char*)*(histlen+1));
				if (! temp) return;
				memcpy(temp,history,sizeof(history));
				free(history);
			}
			history = temp;
			history[histlen] = strdup(expression);
			if (! temp[histlen]) return;
			++histlen;
		}
	}
}

char * historynum (int step)
{
	return history[step];
}

int history_length (void)
{
	return histlen;
}