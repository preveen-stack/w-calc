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

void init_file_loader(void (*show_stateline)(const char*));
int openDotFile(const char *dotFileName,
                int         flags);
int saveState(char *filename); // filename is freed by saveState
int loadState(const char *filename,
              const int   into_history);
int loadStateFD(int       fd,
                const int into_history);         // same as loadState, but takes an FD
int storeVar(const char *variable);
#endif // ifndef WCALC_FILES
/* vim:set expandtab: */
