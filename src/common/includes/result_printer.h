//
// Created by Kyle Wheeler on 4/26/18.
//

#ifndef W_CALC_GIT_RESULT_PRINTER_H
#define W_CALC_GIT_RESULT_PRINTER_H

/* This file is used by both calculator.c and parser.y */

#include "number.h"

/**
 * Initialize the resultprinter system.
 * @param sa function for displaying results (clean separation for GUI vs CLI)
 * @param la pointer to the location of the last-generated answer ("last_answer")
 */
void init_resultprinter(void (*sa)(char*, int, char*), Number *la);

/**
 * Destroy and clean up the resultprinter system.
 */
void term_resultprinter(void);

/**
 * Fetch a reference to the last-printed answer string (pre-formatted).
 */
const char *get_last_answer_str(void);

/**
 * Re-generate the (formatted) last-printed answer string. Useful for reformatting the string (if
 * preferences have changed) or generating a new one (if a new answer has been calculated).
 */
void reset_last_answer_str(void);

/**
 * Used by calculator.c to display answers. Uses the configured callback function, if it exists.
 * @param err Any error message text
 * @param uncertain zero if the answer is inexact, non-zero otherwise
 * @param answer The answer string to display
 */
void show_answer(char *err, int  uncertain, char *answer);

#endif //W_CALC_GIT_RESULT_PRINTER_H