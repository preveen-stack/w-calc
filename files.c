/*
 *  files.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Sat Mar 22 2003.
 *  Copyright (c) 2003 Kyle Wheeler. All rights reserved.
 *
 */

#include <fcntl.h>					   /* for open() */
#include <unistd.h>					   /* for close() */
#include <errno.h>					   /* for errno */
#include <sys/types.h>				   /* for open flags */
#include <sys/stat.h>				   /* for open flags */
#include <string.h>					   /* for strlen() */
#include <stdlib.h>					   /* for free() */
#include <limits.h>					   /* for PATH_MAX */

#include <gmp.h>
#include <mpfr.h>

#include "historyManager.h"
#include "string_manip.h"
#include "calculator.h"
#include "variables.h"

#include "files.h"

/* this is for communicating with the scanner */
char *open_file = NULL;

int saveState(char *filename)
{									   /*{{{ */
	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC,
				  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	int return_error = 0;

	free(filename);
	if (fd >= 0) {
		//success
		int hindex;
		int retval;
		extern int contents;

		/* save variables */
		for (hindex = 0; hindex < contents; hindex++) {
			struct variable *keyval = getrealnvar(hindex);
			char *cptr;

			if (!keyval) {
				continue;
			}
			if (!strcmp(keyval->key, "a")) {
				continue;
			}
			retval = write(fd, keyval->key, strlen(keyval->key));
			if (retval < (int)strlen(keyval->key)) {
				return_error = errno;
				break;
			}
			retval = write(fd, "=", 1);
			if (retval < 1) {
				return_error = errno;
				break;
			}
			if (keyval->exp) {
				cptr = malloc(strlen(keyval->expression) + 3);
				snprintf(cptr, strlen(keyval->expression) + 3, "'%s'",
						 keyval->expression);
			} else {
				cptr = strdup(print_this_result(keyval->value));
			}
			retval = write(fd, cptr, strlen(cptr));
			free(cptr);
			if (retval < (int)strlen(cptr)) {
				return_error = errno;
				break;
			}
			retval = write(fd, "\n", 1);
			if (retval < 1) {
				return_error = errno;
				break;
			}
		}
		/* save history */
		for (hindex = 0; hindex < historyLength() && return_error == 0;
			 hindex++) {
			char *history_entry = historynum(hindex, 1);

			retval = write(fd, history_entry, strlen(history_entry));
			if (retval < (int)strlen(history_entry)) {
				return_error = errno;
				break;
			}
			retval = write(fd, " # = ", 5);
			if (retval < 5) {
				return_error = errno;
				break;
			}
			history_entry = historynum(hindex, 2);
			retval = write(fd, history_entry, strlen(history_entry));
			if (retval < (int)strlen(history_entry)) {
				return_error = errno;
				break;
			}
			retval = write(fd, "\n", 1);
			if (retval < 1) {
				return_error = errno;
				break;
			}
		}
		if (close(fd) != 0) {
			return_error = errno;
		}
	} else {
		//failure
		return_error = errno;
	}
	return return_error;
}									   /*}}} */

int loadState(char *filename, int into_history)
{									   /*{{{ */
	int fd, return_error = 0;
	int standard_output_save = standard_output;

	standard_output = 0;
	fd = open(filename, O_RDONLY);
	if (fd >= 0) {
		//success
		char *linebuf;
		int retval;
		unsigned int linelen = 0, maxlinelen = 99;

		linebuf = calloc(sizeof(char), 100);
		retval = read(fd, linebuf + linelen, 1);
		while (retval == 1) {
			while (retval == 1 && linebuf[linelen] != '\n') {
				linelen++;
				if (linelen == maxlinelen) {
					char *newlinebuf = realloc(linebuf,
											   sizeof(char) * (maxlinelen +
															   100));
					if (newlinebuf) {
						maxlinelen += 100;
						linebuf = newlinebuf;
					} else {
						return_error = errno;
						retval = -1;
						break;
					}
				}
				retval = read(fd, linebuf + linelen, 1);
			}
			linebuf[linelen] = 0;
			if (conf.verbose) {
				printf("-> %s\n", linebuf);
			}
			stripComments(linebuf);
			while (linebuf[strlen(linebuf) - 1] == ' ') {
				linebuf[strlen(linebuf) - 1] = 0;
			}
			if (strlen(linebuf)) {
				extern char *errstring;
				char *safe;

				safe = strdup(linebuf);
				parseme(safe);
				putval("a", last_answer);
				if ((!errstring || (errstring && !strlen(errstring)) ||
					 conf.remember_errors) && into_history) {
					addToHistory(linebuf, last_answer);
				}
			}
			linelen = 0;
			memset(linebuf, 0, maxlinelen);
			if (retval == 1) {
				retval = read(fd, linebuf + linelen, 1);
			}
		}

		if (close(fd) != 0) {
			return_error = errno;
		}
	} else {
		return_error = errno;
	}
	standard_output = standard_output_save;
	return return_error;
}									   /*}}} */

int storeVar(char *variable)
{									   /*{{{ */
	int fd, retval = 0, return_error = 0;
	char filename[PATH_MAX];

	if (!varexists(variable)) {
		report_error("Variable is not defined.");
		return -1;
	}
	snprintf(filename, PATH_MAX, "%s/.wcalc_preload", getenv("HOME"));
	fd = open(filename, O_WRONLY | O_CREAT | O_APPEND,
			  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd >= 0) {
		// success
		struct answer keyval = getvar_full(variable);
		char *cptr;

		retval = write(fd, variable, strlen(variable));
		if (retval < (int)strlen(variable)) {
			return_error = errno;
			goto exit_storeVar;
		}
		retval = write(fd, "=", 1);
		if (retval < 1) {
			return_error = errno;
			goto exit_storeVar;
		}
		if (keyval.exp) {
			cptr = malloc(strlen(keyval.exp) + 3);
			snprintf(cptr, strlen(keyval.exp) + 3, "'%s'", keyval.exp);
		} else {
			cptr = strdup(print_this_result(keyval.val));
		}
		retval = write(fd, cptr, strlen(cptr));
		free(cptr);
		if (retval < strlen(cptr)) {
			return_error = errno;
			goto exit_storeVar;
		}
		retval = write(fd, "\n", 1);
		if (retval < 1) {
			return_error = errno;
			goto exit_storeVar;
		}
		retval = close(fd);
		if (retval == -1) {
			return_error = errno;
			goto exit_storeVar;
		}
	}
  exit_storeVar:
	return return_error;
}									   /*}}} */
