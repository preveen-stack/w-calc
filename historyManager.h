/*
 *  historyManager.h
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Mon Jan 21 2002.
 *  Copyright (c) 2001 __MyCompanyName__. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>

void addToHistory(char * expression);
char * historynum(int);
int history_length(void);

extern short allow_duplicates;