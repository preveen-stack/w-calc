/* */
#ifndef WCALC_OUTPUT_H
#define WCALC_OUTPUT_H

void init_parser(void (*display_prefs)(void),
                 void (*display_output_format)(int),
                 void (*display_status)(const char*, ...),
                 void (*display_interactive_help)(void),
                 void (*display_val)(const char*));
#endif // ifndef WCALC_OUTPUT_H
/* vim:set expandtab: */
