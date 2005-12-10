/*
 *  files.h
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Sat Mar 22 2003.
 *  Copyright (c) 2003 Kyle Wheeler. All rights reserved.
 *
 */

#ifndef WCALC_FILES
#define WCALC_FILES

int saveState(char *filename);
int loadState(char *filename, int into_history);
int storeVar(char *variable);

#endif
