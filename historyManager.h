/*
 *  historyManager.h
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Mon Jan 21 2002.
 *  Copyright (c) 2001 Kyle Wheeler. All rights reserved.
 *
 */

#ifndef WCALC_HISTORY_MANAGER

#include <gmp.h>
#include <mpfr.h>

void clearHistory(void);
void addToHistory(char *, mpfr_t);
char *historynum(int, int);
int historyLength(void);

extern short allow_duplicates;
extern short recalculate;
#endif
