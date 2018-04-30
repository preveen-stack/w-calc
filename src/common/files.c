/*
 *  files.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Sat Mar 22 2003.
 *  Copyright (c) 2003 Kyle Wheeler. All rights reserved.
 *
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* System Headers */
#include <fcntl.h>                     /* for open() */
#include <unistd.h>                    /* for close() */
#include <errno.h>                     /* for errno */
#include <sys/types.h>                 /* for open flags */
#include <sys/stat.h>                  /* for open flags */
#include <string.h>                    /* for strlen() */
#include <stdlib.h>                    /* for free() */
#include <assert.h>                    /* for assert() */

#ifdef HAVE_PATH_MAX
# ifdef HAVE_LIMITS_H
#  include <limits.h>                  /* for PATH_MAX */
# endif
#endif

/* Internal Headers */
#include "historyManager.h"
#include "string_manip.h"
#include "conf.h"
#include "calculator.h"
#include "result_printer.h"
#include "variables.h"
#include "output.h"

#include "files.h"

/* this is for communicating with the scanner */
char *open_file = NULL;

/* Injected Dependencies (output functions) */
static void (*display_stateline)(const char *buf);

static inline void tristrncat(char       *dest,
                              size_t      len,
                              const char *one,
                              const char *two,
                              const char *three)
{
    size_t i;
    size_t written = 0;

    strncpy(dest, one, len);
    for (i = 0; i < len; i++) {
        if (dest[i] == 0) { break; }
    }
    written = i;
    strncat(dest, two, len - written);
    for (i = written; i < len; i++) {
        if (dest[i] == 0) { break; }
    }
    written = i;
    strncat(dest, three, len - written);
}

void init_file_loader(void (*show_stateline)(const char*))
{
    display_stateline = show_stateline;
}

int openDotFile(const char *dotFileName,
                int         flags)
{   /*{{{*/
    char       *filename = NULL;
    const char *home     = getenv("HOME");
    int         namelen;
    int         fd;

    if (!home) { return -1; }
    assert(dotFileName);
    namelen = strlen(home) + strlen(dotFileName) + 3;
#ifdef HAVE_PATH_MAX
    if (namelen > PATH_MAX) {
        return -2;
    }
#endif

    filename = malloc(sizeof(char) * namelen + 1);
    tristrncat(filename, namelen, home, "/.", dotFileName);
    fd       = open(filename, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    free(filename);
    if (fd < 0) {
        return -3;
    } else {
        return fd;
    }
} /*}}}*/

int saveState(char *filename)
{                                      /*{{{ */
    int fd           = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_EXCL,
                            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    int return_error = 0;

    free(filename);
    if (fd >= 0) {
        // success
        size_t hindex;
        int    retval;
        size_t num_vars = numvars();

        /* save variables */
        for (hindex = 0; hindex < num_vars; hindex++) {
            struct variable *keyval = getrealnvar(hindex);
            char            *cptr;

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
                const size_t len = strlen(keyval->expression) + 3;
                cptr = malloc(len);
                tristrncat(cptr, len, "'", keyval->expression, "'");
            } else {
                cptr = strdup(print_this_result(keyval->value, standard_output, sig_figs, NULL,
                                                NULL));
            }
            retval = write(fd, cptr, strlen(cptr));
            if (retval < (int)strlen(cptr)) {
                return_error = errno;
                free(cptr);
                break;
            }
            free(cptr);
            if (keyval->description) {
                const size_t len = strlen(keyval->expression) + 4;
                cptr   = malloc(len);
                tristrncat(cptr, len, " '", keyval->description, "'");
                retval = write(fd, cptr, strlen(cptr));
                if (retval < (int)strlen(cptr)) {
                    return_error = errno;
                    free(cptr);
                    break;
                }
                free(cptr);
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
            retval        = write(fd, history_entry, strlen(history_entry));
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
        // failure
        return_error = errno;
    }
    return return_error;
}                                      /*}}} */

int loadStateFD(int       fd,
                const int into_history)
{   /*{{{*/
    assert(fd >= 0);

    int return_error         = 0;
    int standard_output_save = standard_output;
    standard_output = false;

    char         *linebuf;
    int           retval;
    unsigned int  linelen = 0, maxlinelen = 99;
    const conf_t *conf = getConf();

    linebuf = calloc(sizeof(char), 100);
    retval  = read(fd, linebuf + linelen, 1);
    while (retval == 1) {
        while (retval == 1 && linebuf[linelen] != '\n') {
            linelen++;
            if (linelen == maxlinelen) {
                char *newlinebuf = realloc(linebuf,
                                           sizeof(char) * (maxlinelen +
                                                           100));
                if (newlinebuf) {
                    maxlinelen += 100;
                    linebuf     = newlinebuf;
                } else {
                    return_error = errno;
                    retval       = -1;
                    break;
                }
            }
            retval = read(fd, linebuf + linelen, 1);
        }
        linebuf[linelen] = 0;
        if (conf->verbose) {
            display_stateline(linebuf);
        }
        stripComments(linebuf);
        while (linebuf[strlen(linebuf) - 1] == ' ') {
            linebuf[strlen(linebuf) - 1] = 0;
        }
        if (strlen(linebuf)) {
            extern char *errstring;
            char        *safe;

            safe = strdup(linebuf);
            parseme(safe);
            putval("a", *get_last_answer(), "previous answer");
            if ((!errstring || (errstring && !strlen(errstring)) ||
                 conf->remember_errors) && into_history) {
                addToHistory(linebuf, *get_last_answer());
            }
            free(safe);
        }
        linelen = 0;
        memset(linebuf, 0, maxlinelen);
        if (retval == 1) {
            retval = read(fd, linebuf + linelen, 1);
        }
    }

    free(linebuf);
    standard_output = standard_output_save;

    return return_error;
} /*}}}*/

int loadState(const char *filename,
              const int   into_history)
{                                      /*{{{ */
    int fd, return_error = 0;

    fd = open(filename, O_RDONLY);
    if (fd < 0) { return_error = errno; }
    {
        int tmp = loadStateFD(fd, into_history);
        if (tmp != 0) {
            return_error = tmp;
        }
    }
    if (close(fd) != 0) {
        return_error = errno;
    }

    return return_error;
}                                      /*}}} */

int storeVar(const char *variable)
{                                      /*{{{ */
    int fd, retval = 0, return_error = 0;

    if (!varexists(variable)) {
        report_error("Variable is not defined.");
        return -1;
    }
    fd = openDotFile("wcalc_preload", O_WRONLY | O_CREAT | O_APPEND);
    switch (fd) {
        case -1:
            report_error("HOME environment variable unset.");
            break;
        case -2:
            report_error("HOME environment variable is too long.");
            break;
        default:
        {
            // success
            struct answer keyval = getvar_full(variable);
            char         *cptr;

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
                const size_t len = strlen(keyval.exp) + 4;
                cptr = malloc(len);
                tristrncat(cptr, len, "'", keyval.exp, "' ");
            } else {
                cptr = strdup(print_this_result(keyval.val, standard_output, sig_figs, NULL, NULL));
            }
            retval = write(fd, cptr, strlen(cptr));
            if (retval < (int)strlen(cptr)) {
                return_error = errno;
                free(cptr);
                goto exit_storeVar;
            }
            free(cptr);
            if (keyval.desc) {
                const size_t len = strlen(keyval.desc) + 3;
                cptr   = malloc(len);
                tristrncat(cptr, len, "'", keyval.desc, "'");
                retval = write(fd, cptr, strlen(cptr));
                if (retval < (int)strlen(cptr)) {
                    return_error = errno;
                    free(cptr);
                    goto exit_storeVar;
                }
                free(cptr);
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
    }
exit_storeVar:
    return return_error;
}                                      /*}}} */

/* vim:set expandtab: */
