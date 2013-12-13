/* */
#ifndef WCALC_OUTPUT_H
#define WCALC_OUTPUT_H

/* These functions should be implemented in any frontend. */

void display_prefs(void);
void display_and_clear_errstring(void);
void show_answer(char *err,
                 int   uncertain,
                 char *answer);
void display_output_format(int format);
void display_status(const char *format, ...);
void display_err(const char *format, ...);
void display_interactive_help(void);
void display_command_help(void);
void display_val(const char *name);

#endif
/* vim:set expandtab: */
