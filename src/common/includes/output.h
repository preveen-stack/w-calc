/* */
#ifndef WCALC_OUTPUT_H
#define WCALC_OUTPUT_H

/* These functions should be implemented in any frontend. */

void display_and_clear_errstring(void);
void show_answer(char *err,
                 int   uncertain,
                 char *answer);

#endif
/* vim:set expandtab: */
