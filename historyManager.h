/*
 *  historyManager.h
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Mon Jan 21 2002.
 *  Copyright (c) 2001 __MyCompanyName__. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>

void clearHistory(void);
void addToHistory(char *, double);
char * historynum(int,int);
int history_length(void);

extern short allow_duplicates;
extern short recalculate;