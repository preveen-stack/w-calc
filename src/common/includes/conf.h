#ifndef WCALC_CONF_H
#define WCALC_CONF_H

#ifdef EBUG
#warning "DEBUG OUTPUT ENABLED"
# include <stdio.h>
# include "number.h"
# define Dprintf(fmt, ...)                                               \
    num_fprintf(stderr, "[%s:%d] " fmt, __FILE__, __LINE__, ## __VA_ARGS__); \
    fflush(stderr);
#else
# define Dprintf(...) ;
#endif

enum scientific_modes {always, never, automatic};

enum output_formats {
    DECIMAL_FORMAT,
    OCTAL_FORMAT,
    HEXADECIMAL_FORMAT,
    BINARY_FORMAT,
    FORMAT_COUNT
};

enum rounding_style {
    NO_ROUNDING_INDICATION,
    SIMPLE_ROUNDING_INDICATION,
    SIG_FIG_ROUNDING_INDICATION,
    ROUNDING_INDICATION_COUNT
};


typedef struct _conf {
    unsigned long         history_limit_len;
    int                   precision;
    char                  thou_delimiter;
    char                  dec_delimiter;
    char                  in_thou_delimiter;
    char                  in_dec_delimiter;
    enum scientific_modes scientific          : 2;
    unsigned int          picky_variables     : 1;
    unsigned int          use_radians         : 1;
    enum output_formats   output_format       : 4;
    unsigned int          print_prefixes      : 1;
    enum rounding_style   rounding_indication : 4;
    unsigned int          remember_errors     : 1;
    unsigned int          precision_guard     : 1;
    unsigned int          history_limit       : 1;
    unsigned int          print_equal         : 1;
    unsigned int          print_ints          : 1;
    unsigned int          simple_calc         : 1; // GUI-only
    unsigned int          verbose             : 1;
    unsigned int          print_commas        : 1;
    unsigned int          live_precision      : 1; // GUI-only
    unsigned int          c_style_mod         : 1;
    unsigned int          color_ui            : 1;
    unsigned int          print_greeting      : 1;
} conf_t;

conf_t * getConf(void);

#endif
