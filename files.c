/*
 *  files.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Sat Mar 22 2003.
 *  Copyright (c) 2003 Kyle Wheeler. All rights reserved.
 *
 */

#include <fcntl.h>     /* for open() */
#include <unistd.h>    /* for close() */
#include <errno.h>     /* for errno */
#include <sys/types.h> /* for open flags */
#include <sys/stat.h>  /* for open flags */
#include <string.h>    /* for strlen() */
#include <stdlib.h>     /* for free() */

#include "historyManager.h"
#include "string_manip.h"
#include "calculator.h"
#include "variables.h"

#include "files.h"

/* this is for communicating with the scanner */
char * open_file = NULL;

int saveState(char* filename)
{
	int fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	int return_error = 0;
	free(filename);
	if (fd >= 0) { // success
		int hindex;
		int retval;
		extern int contents;
		/* save variables */
		for (hindex=0;hindex<contents;hindex++) {
			struct variable *keyval = getrealnvar(hindex);
			char value[100], *cptr;
			if (! keyval) continue;
			if (!strcmp(keyval->key,"a")) continue;
			retval = write(fd,keyval->key,strlen(keyval->key));
			if (retval < (int) strlen(keyval->key)) {
				return_error = errno;
				break;
			}
			retval = write(fd,"=",1);
			if (retval < 1) {
				return_error = errno;
				break;
			}
			if (keyval->exp) {
				cptr = keyval->expression;
			} else {
				sprintf(value,"%g",keyval->value);
				cptr = value;
			}
			retval = write(fd,cptr,strlen(cptr));
			if (retval < (int) strlen(cptr)) {
				return_error = errno;
				break;
			}
			retval = write(fd,"\n",1);
			if (retval < 1) {
				return_error = errno;
				break;
			}
		}
	    /* save history */
		for (hindex=0;hindex<historyLength();hindex++) {
			char * history_entry = historynum(hindex,1);
			retval = write(fd,history_entry,strlen(history_entry));
			if (retval < (int) strlen(history_entry)) {
				return_error = errno;
				break;
			}
			retval = write(fd," # = ",5);
			if (retval < 5) {
				return_error = errno;
				break;
			}
			history_entry = historynum(hindex,2);
			retval = write(fd,history_entry,strlen(history_entry));
			if (retval < (int) strlen(history_entry)) {
				return_error = errno;
				break;
			}
			retval = write(fd,"\n",1);
			if (retval < 1) {
				return_error = errno;
				break;
			}
		}
		if (close(fd) != 0) {
			return_error = errno;
		}
	} else { // failure
		return_error = errno;
	}
	return return_error;
}

int loadState (char* filename)
{
	int fd, return_error = 0;
	fd = open(filename, O_RDONLY);
	free(filename);
	if (fd >= 0) { // success
		char * linebuf;
		int retval;
		double val;
		unsigned int linelen = 0, maxlinelen = 100;

		linebuf = calloc(sizeof(char),100);
		retval = read(fd,linebuf+linelen,1);
		while (retval == 1) {
			while (retval == 1 && linebuf[linelen] != '\n') {
				linelen++;
				if (linelen == maxlinelen) {
					char * newlinebuf = realloc( linebuf, sizeof(maxlinelen+100));
					if (newlinebuf) {
						maxlinelen += 100;
						linebuf = newlinebuf;
					} else {
						return_error = errno;
						retval = -1;
						break;
					}
				}
				retval = read(fd,linebuf+linelen,1);
			}
			linebuf[linelen] = 0;
			strstrip(' ',linebuf);
			stripComments(linebuf);
			if (strlen(linebuf)) {
				extern char * errstring;
				char *safe;
				safe = strdup(linebuf);
				val = parseme(safe);
				putval("a",val);
				if (!errstring || (errstring && !strlen(errstring)) || conf.remember_errors) {
					addToHistory(linebuf, val);
				}
			}
			linelen = 0;
			linebuf[linelen] = 0;
			if (retval == 1)
				retval = read(fd,linebuf+linelen,1);
		}

		if (close(fd) != 0) {
			return_error = errno;
		}
	}
	return return_error;
}
		