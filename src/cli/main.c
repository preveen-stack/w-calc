/* Based (very loosely) on Ostermann's headstart for the shell project
 * Modified by Kyle Wheeler
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h> /* for strncasecmp(), per POSIX 2001 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>                  /* for stat() */
#include <wctype.h>                    /* for iswspace() */
#include <errno.h>
#include <fcntl.h>                     /* for open() */
#include <limits.h>                    /* for stroul() */
#include <sys/mman.h>                  /* for mmap() */
#include <assert.h>                    /* for assert() */
#include <stdarg.h>                    /* for va_start() */
#include "number.h"

#ifdef HAVE_EDITLINE
#elif defined(HAVE_LIBREADLINE)
# if defined(HAVE_READLINE_READLINE_H)
#  include <readline/readline.h>
# elif defined(HAVE_READLINE_H)
#  include <readline.h>
# else /* !defined(HAVE_READLINE_H) */
extern char *readline();
# endif /* !defined(HAVE_READLINE_H) */
/*@null@*/
char *cmdline = NULL;
#else /* !defined(HAVE_READLINE_READLINE_H) */
/* no readline */
#endif /* HAVE_LIBREADLINE */

#ifdef HAVE_READLINE_HISTORY
# if defined(HAVE_READLINE_HISTORY_H)
#  include <readline/history.h>
# elif defined(HAVE_HISTORY_H)
#  include <history.h>
# elif defined(HAVE_EDITLINE)
#  include "editline/readline.h" /* for using_history() */
# else /* ! defined(HAVE_HISTORY_H) */
extern void add_history();
extern int  write_history();
extern int  read_history();
extern int  history_truncate_file(char *,
                                  int);
# endif /* defined(HAVE_READLINE_HISTORY_H) */
/* no history */
#endif /* HAVE_READLINE_HISTORY */

#include "calculator.h"
#include "conversion.h"
#ifdef HAVE_CONFIG_H /* auto-tool build */
# include "parser.h"
#else
# include "y.tab.h"
#endif
#include "variables.h"
#include "files.h"
#include "historyManager.h"
#include "list.h"
#include "iscmd.h"
#include "isconst.h"
#include "isfunc.h"
#include "output.h"
#include "evalvar.h"

#define TRUEFALSE  (!strcmp(value, "yes") || !strcmp(value, "true") || !strcmp(value, "1"))
#define BIG_STRING 4096
#define VERSION    PACKAGE_VERSION
#define EXIT_EARLY(code) do {  \
        clearHistory();        \
        cleanupvar();          \
        num_free(last_answer); \
        lists_cleanup();       \
        fflush(NULL);          \
        exit(code);            \
} while (0)

static int  read_prefs(void);
static int  read_preload(void);
static void display_and_clear_errstring(void);
static int  set_pref(const char *key, const char *value);
static void config_error(const char *format, ...);

/*
 * These are declared here because they're not in any header files.
 * yyparse() is declared with an empty argument list so that it is
 * compatible with the generated C code from yacc/bison.
 * This part is taken from http://www.bgw.org/tutorials/programming/c/lex_yacc/main.c
 */
extern int yyparse(void);
extern int yy_scan_string(const char *);

static int exit_on_err = 0;
static char *config_type = "";

#if defined(HAVE_LIBREADLINE)
/*@null@*/
static List tc_options = NULL;

/*@null@*/
static char *
tc_generator(const char *text,
             int         state)
{                                      /*{{{ */
    char *ret = getHeadOfList(tc_options);

    if (ret) {
        return ret;
    } else {
        return NULL;
    }
}                                      /*}}} */

/*@null@*/
static char *
tc_rounding(const char *text,
            int         state)
{                                      /*{{{ */
    static unsigned int i          = 0;
    char               *rounding[] = { "none", "simple", "sig_fig", 0 };

    if (state == 0) {
        i = 0;
    }
    while (rounding[i] != NULL) {
        if (strncmp(text, rounding[i], strlen(text)) == 0) {
            return strdup(rounding[i++]);
        }
        i++;
    }
    return NULL;
}                                      /*}}} */

/*@null@*/
static char *
tc_scientific(const char *text,
               int         state)
{                                      /*{{{ */
    static unsigned int i            = 0;
    char               *scientific[] = { "always", "never", "auto", "automatic", 0 };

    if (state == 0) {
        i = 0;
    }
    while (scientific[i] != NULL) {
        if (strncmp(text, scientific[i], strlen(text)) == 0) {
            return strdup(scientific[i++]);
        }
        i++;
    }
    return NULL;
}                                      /*}}} */

char **qcommands = NULL;

# define COMPLETE(strs)  do {                                                 \
        int compareword = 0;                                                  \
        int comparechar = 0;                                                  \
        int textcurs    = 0;                                                  \
        while (strs[compareword] != NULL) {                                   \
            if (text[textcurs] == 0) { /* add to the list of possibilities */ \
                addToList(&tc_options, strdup(strs[compareword]));            \
                textcurs    = 0;                                              \
                comparechar = 0;                                              \
                compareword++;                                                \
            } else if (text[textcurs] == strs[compareword][comparechar]) {    \
                textcurs++;                                                   \
                comparechar++;                                                \
            } else { /* not a possibility: next! */                           \
                textcurs    = 0;                                              \
                compareword++;                                                \
                comparechar = 0;                                              \
            }                                                                 \
        }                                                                     \
} while (0)
# define COMPLETE2(strs) do {                                       \
        const unsigned textlen = strlen(text);                      \
        for (unsigned _c_i = 0; strs[_c_i].explanation; _c_i++) {   \
            const char *const *const _c_names = strs[_c_i].names;   \
            for (unsigned _c_j = 0; _c_names[_c_j]; _c_j++) {       \
                if (!strncmp(text, _c_names[_c_j], textlen)) {      \
                    /* add to the list of possibilities */          \
                    addToList(&tc_options, strdup(_c_names[_c_j])); \
                }                                                   \
            }                                                       \
        }                                                           \
} while (0)

static void
build_qcommands(void)
{
    unsigned c_count = 0;

    for (unsigned c = 0; commands[c].explanation; c++) {
        for (unsigned n = 0; commands[c].names[n]; n++) {
            c_count++;
        }
    }
    c_count++;
    qcommands = malloc(sizeof(char *) * c_count);
    c_count   = 0;
    for (unsigned c = 0; commands[c].explanation; c++) {
        for (unsigned n = 0; commands[c].names[n]; n++) {
            qcommands[c_count] = malloc(strlen(commands[c].names[n]) + 2);
            sprintf(qcommands[c_count], "\\%s", commands[c].names[n]);
            c_count++;
        }
    }
    qcommands[c_count] = NULL;
}

static char **
wcalc_completion(const char *text,
                 int         start,
                 int         end)
{                                      /*{{{ */
    /*extern const char *commands[];*/
    char **variables;
    char **retvals = NULL;

    // printf("\ncompleting: %s\n", text);
    if ('\\' == rl_line_buffer[0]) {
        if ((NULL == strchr(rl_line_buffer, ' ')) &&
            (NULL == strchr(rl_line_buffer, '\t'))) {
            COMPLETE(qcommands);
        } else if (strncmp("\\explain", rl_line_buffer, 8) == 0) {
            int i = 8;

            while (iswspace(rl_line_buffer[i])) ++i;
            if (i == start) {
                COMPLETE(qcommands);
                COMPLETE2(consts);
                COMPLETE2(funcs);
                variables = listvarnames();
                COMPLETE(variables);
                free(variables);
            }
        } else if ((strncmp("\\open", rl_line_buffer, 5) == 0) ||
                   (strncmp("\\save", rl_line_buffer, 5) == 0)) {
            int i = 5;

            while (iswspace(rl_line_buffer[i])) ++i;
            if (i == start) {
                retvals =
                    rl_completion_matches(text,
                                          rl_filename_completion_function);
                return retvals;
            }
        } else if (((strncmp("\\rou", rl_line_buffer, 4) == 0) &&
                    iswspace(rl_line_buffer[4])) ||
                   ((strncmp("\\round", rl_line_buffer, 6) == 0) &&
                    iswspace(rl_line_buffer[6])) ||
                   ((strncmp("\\rounding", rl_line_buffer, 9) == 0) &&
                    iswspace(rl_line_buffer[9]))) {
            int i = 4;

            while (!iswspace(rl_line_buffer[i])) ++i;
            while (iswspace(rl_line_buffer[i])) ++i;
            if (i == start) {
                retvals = rl_completion_matches(text, tc_rounding);
                return retvals;
            }
        } else if (((strncmp("\\s", rl_line_buffer, 2) == 0) &&
                    iswspace(rl_line_buffer[2])) ||
                   ((strncmp("\\sci", rl_line_buffer, 4) == 0) &&
                    iswspace(rl_line_buffer[4])) ||
                   ((strncmp("\\scientific", rl_line_buffer, 12) == 0) &&
                    iswspace(rl_line_buffer[12]))) {
            int i = 2;

            while (!iswspace(rl_line_buffer[i])) ++i;
            while (iswspace(rl_line_buffer[i])) ++i;
            if (i == start) {
                retvals = rl_completion_matches(text, tc_scientific);
                return retvals;
            }
        } else if (((strncmp("\\c", rl_line_buffer, 2) == 0) &&
                    iswspace(rl_line_buffer[2])) ||
                   ((strncmp("\\conv", rl_line_buffer, 5) == 0) &&
                    iswspace(rl_line_buffer[5])) ||
                   ((strncmp("\\convert", rl_line_buffer, 8) == 0) &&
                    iswspace(rl_line_buffer[8]))) {
            int i = 2;
            /*extern const struct conversion lengths[], areas[], volumes[],
             *  masses[], speeds[], powers[], forces[], accelerations[];*/
            extern const struct conversion *conversions[];

            while (!iswspace(rl_line_buffer[i])) ++i;
            while (iswspace(rl_line_buffer[i])) ++i;
            if (i == start) {
                /* complete on ALL units */
                size_t unit, conversion;

                for (conversion = 0; conversions[conversion]; conversion++) {
                    for (unit = 0; conversions[conversion][unit].name; unit++) {
                        COMPLETE(conversions[conversion][unit].aka);
                    }
                }
            } else {
                /* seek past the previous unit... */
                char   *unit1 = rl_line_buffer + i;
                char    saved_char;
                ssize_t unit_cat;

                while (!iswspace(rl_line_buffer[i])) ++i;
                saved_char        = rl_line_buffer[i];
                rl_line_buffer[i] = 0;
                unit_cat          = identify_unit(unit1);
                rl_line_buffer[i] = saved_char;
                if (unit_cat != -1) {
                    while (iswspace(rl_line_buffer[i])) ++i;
                    if (i == start) {
                        size_t unit;

                        /* complete on COMPATABLE units */
                        for (unit = 0; conversions[unit_cat][unit].name;
                             unit++) {
                            COMPLETE(conversions[unit_cat][unit].aka);
                        }
                        addToList(&tc_options, strdup("to"));
                    } else if ((strncmp(rl_line_buffer + i, "to", 2) == 0) &&
                               iswspace(rl_line_buffer[i + 2])) {
                        i += 2;
                        while (iswspace(rl_line_buffer[i])) ++i;
                        if (i == start) {
                            size_t unit;

                            /* complete on COMPATABLE units */
                            for (unit = 0; conversions[unit_cat][unit].name;
                                 unit++) {
                                COMPLETE(conversions[unit_cat][unit].aka);
                            }
                        }
                    }
                }
            }
        }
    } else {
        COMPLETE(qcommands);
        COMPLETE2(consts);
        COMPLETE2(funcs);
        variables = listvarnames();
        COMPLETE(variables);
        free(variables);
    }
    rl_attempted_completion_over = 1;  // do not use standard file completion
    rl_completion_entry_function = (Function*)tc_generator;
    retvals                      = rl_completion_matches(text, tc_generator);
    return retvals;
}                                      /*}}} */
#endif /* ifdef HAVE_LIBREADLINE */

enum ui_colors {
    NONE,
    RESET,
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BOLDBLACK,
    BOLDRED,
    BOLDGREEN,
    BOLDYELLOW,
    BOLDBLUE,
    BOLDMAGENTA,
    BOLDCYAN,
    BOLDWHITE,
};
const char *const colors[] = {
    "",        // NONE
    "\033[0m", // RESET
    "\033[30m",
    "\033[31m",
    "\033[32m",
    "\033[33m",
    "\033[34m",
    "\033[35m",
    "\033[36m",
    "\033[37m",
    "\033[1m\033[30m",
    "\033[1m\033[31m",
    "\033[1m\033[32m",
    "\033[1m\033[33m",
    "\033[1m\033[34m",
    "\033[1m\033[35m",
    "\033[1m\033[36m",
    "\033[1m\033[37m",
};

enum ui_pieces {
    PROMPT,
    CONV_CAT,
    CONV_UNIT,
    APPROX_ANSWER,
    EXACT_ANSWER,
    ERR_LOCATION,
    ERR_TEXT,
    PREF_NAME,
    PREF_VAL,
    PREF_CMD,
    STATUS,
    VAR_NAME,
    VAR_DESC,
    SUBVAR_NAME,
    EXPLANATION,
    UNCOLOR,
};
int  black_and_white_ui[] = {
    NONE, // PROMPT
    NONE, // CONV_CAT
    NONE, // CONV_UNIT
    NONE, // APPROX_ANSWER
    NONE, // EXACT_ANSWER
    NONE, // ERR_LOCATION
    NONE, // ERR_TEXT
    NONE, // PREF_NAME
    NONE, // PREF_VAL
    NONE, // PREF_CMD
    NONE, // STATUS
    NONE, // VAR_NAME
    NONE, // VAR_DESC
    NONE, // SUBVAR_NAME
    NONE, // EXPLANATION
    NONE, // UNCOLOR
};
int  color_ui[] = {
    BLUE,        // PROMPT
    BOLDYELLOW,  // CONV_CAT
    CYAN,        // CONV_UNIT
    YELLOW,      // APPROX_ANSWER
    GREEN,       // EXACT_ANSWER
    BOLDMAGENTA, // ERR_LOCATION
    BOLDRED,     // ERR_TEXT
    YELLOW,      // PREF_NAME
    CYAN,        // PREF_VAL
    BLUE,        // PREF_CMD
    BOLDGREEN,   // STATUS
    BOLDCYAN,    // VAR_NAME
    CYAN,        // VAR_DESC
    CYAN,        // SUBVAR_NAME
    NONE,        // EXPLANATION
    RESET,       // UNCOLOR
};
int *uiselect = black_and_white_ui;

static void
PrintConversionUnitCategory(int nCategory)
{
    printf("\n%s%s%s\n", colors[uiselect[CONV_CAT]], conversion_names[nCategory], colors[uiselect[UNCOLOR]]);
    size_t unit, nAka;

    for (unit = 0; conversions[nCategory][unit].name; ++unit) {
        printf("\n%s%s%s: ", colors[uiselect[CONV_UNIT]], conversions[nCategory][unit].name, colors[uiselect[UNCOLOR]]);
        for (nAka = 0; nAka < 9; ++nAka) {
            if (conversions[nCategory][unit].aka[nAka] != NULL) {
                if (nAka > 0) { printf(", "); }
                printf("%s", conversions[nCategory][unit].aka[nAka]);
            }
        }
    }
    printf("\n\n");
}

void
show_answer(char *err,
            int   uncertain,
            char *answer)
{   /*{{{*/
    if (err && strlen(err)) {
        display_and_clear_errstring();
    }
    if (uncertain) {
        printf("%s%s %s%s\n", colors[uiselect[APPROX_ANSWER]], conf.print_equal ? "~=" : "  ", colors[uiselect[UNCOLOR]], answer);
    } else {
        printf("%s%s %s%s\n", colors[uiselect[EXACT_ANSWER]], conf.print_equal ? " =" : "  ", colors[uiselect[UNCOLOR]], answer);
    }
} /*}}}*/

static void
display_and_clear_errstring(void)
{                                      /*{{{ */
    extern int   scanerror;
    extern char *errstring;
    extern int   errloc;

    if (errstring && errstring[0]) {
        if (errloc != -1) {
            int          i;
            extern int   show_line_numbers;
            extern char *last_input;

            if (show_line_numbers && last_input) {
                fprintf(stderr, "-> %s\n", last_input);
            }
            fprintf(stderr, "   ");
            for (i = 0; i < errloc; i++) {
                fprintf(stderr, " ");
            }
            fprintf(stderr, "%s^%s\n", colors[uiselect[ERR_LOCATION]], colors[uiselect[UNCOLOR]]);
            errloc = -1;
        }
        fprintf(stderr, "%s%s%s", colors[uiselect[ERR_TEXT]], errstring, colors[uiselect[UNCOLOR]]);
        if (errstring[strlen(errstring) - 1] != '\n') {
            fprintf(stderr, "\n");
        }
        free(errstring);
        errstring = NULL;
        scanerror = 0;
        if (exit_on_err) {
            EXIT_EARLY(EXIT_FAILURE);
        }
    }
}                                      /*}}} */

int
main(int   argc,
     char *argv[])
{                                      /*{{{ */
    extern int yydebug;
    extern int lines;

#ifdef HAVE_LIBREADLINE
    char *readme = NULL;
#else
    char  readme[BIG_STRING];
#endif
#ifdef HAVE_READLINE_HISTORY
    char *historyfile = "/.wcalc_history";
#endif
    int   tty, i;
    short cmdline_input = 0;

    // yydebug = 1;                       /* turn on ugly YACC debugging */
    yydebug = 0;                       /* turn off ugly YACC debugging */

    initvar();
    lists_init();

    /* initialize the preferences */
    memset(&conf, 0, sizeof(struct _conf));
    conf.precision           = -1;
    conf.scientific          = automatic;
    standard_output          = 1;
    conf.picky_variables     = 1;
    conf.print_prefixes      = 1;
    conf.precision_guard     = 1;
    conf.thou_delimiter      = ',';
    conf.dec_delimiter       = '.';
    conf.print_equal         = 1;
    conf.c_style_mod         = 1;
    conf.print_greeting      = 1;
    conf.rounding_indication = SIG_FIG_ROUNDING_INDICATION;

    init_numbers();
    num_init_set_ui(last_answer, 0);

    /* load the preferences */
    {
        int foundflag = 0;

        /* quick check the commandline for a --defaults argument */
        for (i = 1; i < argc; ++i) {
            if (!strcmp(argv[i], "--defaults")) {
                foundflag = 1;
                break;
            }
        }

        if (foundflag == 0) {
            if (read_prefs()) {
                read_preload();
            } else {
                perror("Error reading preferences file (~/.wcalcrc)");
            }
        }
    }

    /* Parse commandline options */
    config_type = "Arg: ";
    for (i = 1; i < argc; ++i) {
        if (!strncmp(argv[i], "-P", 2)) {
            char *valstr;

            if ((strlen(&argv[i][2]) == 0) && (i + 1 < argc)) {
                // pull the arg from i+1
                valstr = argv[++i];
            } else {
                valstr = &argv[i][2];
            }
            if (!set_pref("precision", valstr)) {
                EXIT_EARLY(EXIT_FAILURE);
            }
        } else if (!strcmp(argv[i], "-s") ||
                   !strcmp(argv[i], "--scientific")) {
            conf.scientific = always;
        } else if (!strcmp(argv[i], "-EE")) {
            conf.scientific = never;
        } else if (!strcmp(argv[i], "-H") || !strcmp(argv[i], "--help")) {
            display_command_help();
            EXIT_EARLY(EXIT_SUCCESS);
        } else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
            printf("wcalc "VERSION "\n");
            EXIT_EARLY(EXIT_SUCCESS);
        } else if (!strcmp(argv[i], "-u") || !strcmp(argv[i], "--units")) {
            if ((i + 1 >= argc) || !strcasecmp(argv[i + 1], "all")) {
                int nCategory;
                for (nCategory = 0; nCategory <= MAX_TYPE; ++nCategory) {
                    PrintConversionUnitCategory(nCategory);
                }
            } else if (strlen(argv[i + 1]) == 1) {
                fprintf(stderr, "The %s option's (optional) argument must be two or more letters of one\nof the following (case insensitive):\n", argv[i]);
                int nCategory;
                for (nCategory = 0; nCategory <= MAX_TYPE; ++nCategory) {
                    printf("\t%s\n", conversion_names[nCategory]);
                }
                EXIT_EARLY(EXIT_FAILURE);
            } else {
                const size_t len     = strlen(argv[i + 1]);
                int          printed = 0;
                int          nCategory;
                for (nCategory = 0; nCategory <= MAX_TYPE; ++nCategory) {
                    if (!strncasecmp(argv[i + 1], conversion_names[nCategory], len)) {
                        printed = 1;
                        PrintConversionUnitCategory(nCategory);
                        break;
                    }
                }
                if (printed == 0) {
                    fprintf(stderr, "The %s option's (optional) argument must be two or more letters of one\nof the following (case insensitive):\n", argv[i]);
                    for (nCategory = 0; nCategory <= MAX_TYPE; ++nCategory) {
                        printf("\t%s\n", conversion_names[nCategory]);
                    }
                    EXIT_EARLY(EXIT_FAILURE);
                }
            }
            EXIT_EARLY(EXIT_SUCCESS);
        } else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--decimal") ||
                   !strcmp(argv[i], "-dec") || !strcmp(argv[i], "--dec")) {
            conf.output_format = DECIMAL_FORMAT;
        } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--hexadecimal") ||
                   !strcmp(argv[i], "-hex") || !strcmp(argv[i], "--hex")) {
            conf.output_format = HEXADECIMAL_FORMAT;
        } else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--octal") ||
                   !strcmp(argv[i], "-oct") || !strcmp(argv[i], "--oct")) {
            conf.output_format = OCTAL_FORMAT;
        } else if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--binary") ||
                   !strcmp(argv[i], "-bin") || !strcmp(argv[i], "--bin")) {
            conf.output_format = BINARY_FORMAT;
        } else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--prefixes")) {
            conf.print_prefixes = !conf.print_prefixes;
            /*} else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--lenient")) {
             * conf.picky_variables = 0; */
        } else if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--radians")) {
            conf.use_radians = !conf.use_radians;
        } else if (!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")) {
            conf.print_equal    = !conf.print_equal;
            conf.print_greeting = !conf.print_greeting;
        } else if (!strcmp(argv[i], "-c") ||
                   !strcmp(argv[i], "--conservative")) {
            conf.precision_guard = !conf.precision_guard;
        } else if (!strcmp(argv[i], "-R") || !strcmp(argv[i], "--remember")) {
            conf.remember_errors = !conf.remember_errors;
        } else if (!strncmp(argv[i], "--round=", 8)) {
            if (!set_pref("rounding_indication", &(argv[i][8]))) {
                EXIT_EARLY(EXIT_FAILURE);
            }
        } else if (!strncmp(argv[i], "--idsep=", 7)) {
            if (!set_pref("input_decimal_delimiter", &(argv[i][7]))) {
                EXIT_EARLY(EXIT_FAILURE);
            }
        } else if (!strncmp(argv[i], "--dsep=", 7)) {
            if (!set_pref("decimal_delimiter", &(argv[i][7]))) {
                EXIT_EARLY(EXIT_FAILURE);
            }
        } else if (!strncmp(argv[i], "--itsep=", 7)) {
            if (!set_pref("input_thousands_delimiter", &(argv[i][7]))) {
                EXIT_EARLY(EXIT_FAILURE);
            }
        } else if (!strncmp(argv[i], "--tsep=", 7)) {
            if (!set_pref("thousands_delimiter", &(argv[i][7]))) {
                EXIT_EARLY(EXIT_FAILURE);
            }
        } else if (!strncmp(argv[i], "--bits", 6)) {
            unsigned long int argnum;
            char             *endptr;

            argnum = strtoul(&(argv[i][6]), &endptr, 0);
            if ((endptr != NULL) && (strlen(endptr) > 0)) {
                config_error("--bits option requires a valid integer without spaces.\n");
                EXIT_EARLY(EXIT_FAILURE);
            } else {
                if (argnum < NUM_PREC_MIN) {
                    config_error("Minimum precision is %lu.\n", (unsigned long)NUM_PREC_MIN);
                } else if (argnum > NUM_PREC_MAX) {
                    config_error("Maximum precision is %lu.\n", (unsigned long)NUM_PREC_MAX);
                } else {
                    num_set_default_prec(argnum);
                }
            }
        } else if (!strcmp(argv[i], "--ints")) {
            conf.print_ints = !conf.print_ints;
        } else if (!strcmp(argv[i], "--delim")) {
            conf.print_commas = !conf.print_commas;
        } else if (!strcmp(argv[i], "--verbose")) {
            conf.verbose = !conf.verbose;
        } else if (!strcmp(argv[i], "--yydebug")) {
            yydebug = 1;
        } else if (!strcmp(argv[i], "-n")) {
            conf.print_equal = 0;
        } else if (!strcmp(argv[i], "-C") || !strcmp(argv[i], "--color")) {
            conf.color_ui = !conf.color_ui;
            if (conf.color_ui) {
                uiselect = color_ui;
            } else {
                uiselect = black_and_white_ui;
            }
        } else if (!strcmp(argv[i], "--defaults")) {
            /* ignore this argument */
        } else {
            extern char *errstring;

            if (isatty(1) == 0) { /* Find out where stdout is going */
                uiselect = black_and_white_ui;
            }

            if (!cmdline_input) {
#ifdef HAVE_READLINE_HISTORY
                char             *filename;
                const char *const home = getenv("HOME");

                using_history();
                filename = malloc(strlen(home) + strlen(historyfile) + 2);
                snprintf(filename,
                         strlen(home) + strlen(historyfile) + 1,
                         "%s%s",
                         home, historyfile);
                if (read_history(filename)) {
                    if (errno != ENOENT) {
                        perror("Reading History");
                    }
                }
                free(filename);
#endif          /* ifdef HAVE_READLINE_HISTORY */
                cmdline_input = 1;
            }
            if (conf.verbose) {
                printf("-> %s\n", argv[i]);
            }
            parseme(argv[i]);
            if (!errstring || (errstring && !strlen(errstring)) ||
                conf.remember_errors) {
                addToHistory(argv[i], last_answer);
            }
            if (errstring && strlen(errstring)) {
                fprintf(stderr, "%s", errstring);
                if (errstring[strlen(errstring) - 1] != '\n') {
                    fprintf(stderr, "\n");
                }
                free(errstring);
                errstring = NULL;
            }
            if (putval("a", last_answer, "previous answer") != 0) {
                fprintf(stderr, "error storing last answer\n");
            }
        }
    }
    config_type = "";

    if (!cmdline_input) {
        extern char *errstring;
        char        *envinput = getenv("wcalc_input");
        if (envinput) {
#ifdef HAVE_READLINE_HISTORY
            char             *filename;
            const char *const home = getenv("HOME");

            using_history();
            filename = malloc(strlen(home) + strlen(historyfile) + 2);
            strcpy(filename, home);
            strcat(filename, historyfile);
            if (read_history(filename)) {
                if (errno != ENOENT) {
                    perror("Reading History");
                }
            }
            free(filename);
#endif      /* ifdef HAVE_READLINE_HISTORY */
            cmdline_input = 1;
            if (conf.verbose) {
                printf("-> %s\n", envinput);
            }
            parseme(envinput);
            if (!errstring || (errstring && !strlen(errstring)) ||
                conf.remember_errors) {
                addToHistory(envinput, last_answer);
            }
            if (errstring && strlen(errstring)) {
                fprintf(stderr, "%s", errstring);
                if (errstring[strlen(errstring) - 1] != '\n') {
                    fprintf(stderr, "\n");
                }
                free(errstring);
                errstring = NULL;
            }
            if (putval("a", last_answer, "previous answer") != 0) {
                fprintf(stderr, "error storing last answer\n");
            }
        }
    }

    if (cmdline_input) {
#ifdef HAVE_READLINE_HISTORY
        char *filename;
        char *home = getenv("HOME");

        filename = malloc(strlen(home) + strlen(historyfile) + 2);
        snprintf(filename,
                 strlen(home) + strlen(historyfile) + 1,
                 "%s%s",
                 home, historyfile);
        if (write_history(filename)) {
            perror("Saving History 1");
        }
        if (conf.history_limit) {
            if (history_truncate_file(filename, conf.history_limit_len)) {
                perror("Truncating History");
            }
        }
        free(filename);
#endif  /* ifdef HAVE_READLINE_HISTORY */
        EXIT_EARLY(EXIT_SUCCESS);
    }

    tty = isatty(0);                   /* Find out where stdin is coming from... */
    if (tty > 0) {
        /* if stdin is a keyboard or terminal, then use readline and prompts */
#ifdef HAVE_READLINE_HISTORY
        const char *const home = getenv("HOME");
        char             *filename;

        using_history();
        filename = malloc(strlen(home) + strlen(historyfile) + 2);
        snprintf(filename,
                 strlen(home) + strlen(historyfile) + 1,
                 "%s%s",
                 home, historyfile);
        if (read_history(filename)) {
            if (errno != ENOENT) {
                perror("Reading History");
            }
        }
#endif  /* ifdef HAVE_READLINE_HISTORY */
#ifdef HAVE_LIBREADLINE
        build_qcommands();
        rl_attempted_completion_function = wcalc_completion;
        rl_basic_word_break_characters   = " \t\n\"\'+-*/[{()}]=<>!|~&^%";
#endif
        exit_on_err                      = 0;
        if (conf.print_greeting) {
            printf("Enter an expression to evaluate, q to quit, or ? for help:\n");
        }
        while (1) {
            lines = 1;
            fflush(NULL);
#ifdef HAVE_LIBREADLINE
            {
#if defined(HAVE_EDITLINE) && ! defined(HAVE_READLINE_H)
                const char *prompt = "-> ";
#else
                char prompt[30] = "";
                snprintf(prompt, 30, "\1%s\2->\1%s\2 ", colors[uiselect[PROMPT]], colors[uiselect[UNCOLOR]]);
#endif
                readme = readline(prompt);
            }
            if (!readme) {
                /* from the readline manpage:
                 *      readline returns the text of the line read. A blank
                 *      line returns the empty string. If EOF is encountered
                 *      while reading a line, and the line is empty, NULL is
                 *      returned. If an eof is read with a non-empty line, it
                 *      is treated as a newline.
                 * This means: readme == NULL is a perfectly reasonable
                 * response from readline(), AND it means something
                 * significant. DO NOT DO errno PARSING HERE!!!!
                 */
                printf("\n");
                break;
            }
#else       /* ifdef HAVE_LIBREADLINE */
            {
                char         c;
                unsigned int i = 0;

                memset(readme, 0, BIG_STRING);
                printf("%s->%s ", colors[uiselect[PROMPT]], colors[uiselect[UNCOLOR]]);
                fflush(stdout);
                c = fgetc(stdin);
                while (c != '\n' && i < BIG_STRING && !feof(stdin) &&
                       !ferror(stdin)) {
                    readme[i] = c;
                    c         = fgetc(stdin);
                    ++i;
                }
                if (feof(stdin) || ferror(stdin)) {
                    printf("\n");
                    break;
                }
            }
#endif      /* ifdef HAVE_LIBREADLINE */
            /* if there is a line */
            if (strlen(readme)) {
                if (!strcmp(readme, "q") || !strcmp(readme, "quit") ||
                    !strcmp(readme, "\\q")) {
                    break;
                } else if (!strncmp(readme, "\\yydebug", 8)) {
                    // addToHistory(readme, 0);
                    yydebug = !yydebug;
                    printf("Debug Mode %s\n", yydebug ? "On" : "Off");
                } else if (!strncmp(readme, "\\color", 6)) {
                    conf.color_ui = !conf.color_ui;
                    if (conf.color_ui) {
                        uiselect = color_ui;
                    } else {
                        uiselect = black_and_white_ui;
                    }
                } else {
                    if (conf.verbose) {
                        printf("-> %s\n", readme);
                    }
                    parseme(readme);
                    {
                        extern char *errstring;

                        if (!errstring || (errstring && !strlen(errstring)) ||
                            conf.remember_errors) {
                            addToHistory(readme, last_answer);
                        }
                    }
                }
                if (putval("a", last_answer, "previous answer") != 0) {
                    fprintf(stderr, "error storing last answer\n");
                }

                {
                    extern char *errstring;

                    if (errstring && strlen(errstring)) {
                        display_and_clear_errstring();
                        /*rl_stuff_char('f');*/
                    }
                }
            }
#ifdef HAVE_LIBREADLINE
            free(readme);
#endif
        }
#ifdef HAVE_READLINE_HISTORY
        if (write_history(filename)) {
            fprintf(stderr, "Cannot save history to %s: %s\n", filename, strerror(errno));
            fflush(stderr);
        }
        if (conf.history_limit) {
            if (history_truncate_file(filename, conf.history_limit_len)) {
                perror("Truncating History");
            }
        }
        free(filename);
#endif  /* ifdef HAVE_READLINE_HISTORY */
    } else if (tty < 0) {
        fprintf(stderr, "Could not determine terminal type.\n");
    } else {
        /* if stdin is ANYTHING ELSE (a pipe, a file, etc), don't prompt */
        unsigned int linelen = 0, maxlinelen = BIG_STRING;
        extern int   show_line_numbers;

        exit_on_err       = 1;
        show_line_numbers = 1;
        while (1) {
            char *line   = calloc(maxlinelen, sizeof(char));
            char  gotten = fgetc(stdin);
            while (gotten != '\n' && !feof(stdin) && !ferror(stdin)) {
                line[linelen] = gotten;
                gotten        = fgetc(stdin);
                linelen++;
                if (linelen > maxlinelen) {
                    char *temp;
                    temp = realloc(line, (maxlinelen + BIG_STRING) * sizeof(char));
                    if (!temp) {
                        free(line);
                        fprintf(stderr,
                                "Ran out of memory. Line too long.\n");
                        EXIT_EARLY(EXIT_FAILURE);
                    }
                    memset(temp + maxlinelen, 0, BIG_STRING);
                    maxlinelen += BIG_STRING;
                    line        = temp;
                }
            }
            if (ferror(stdin) || (feof(stdin) && (linelen == 0))) {
                free(line);
                break;
            }
            if (conf.verbose) {
                printf("-> %s\n", line);
            }
            parseme(line);
            if (putval("a", last_answer, "previous answer") != 0) {
                fprintf(stderr, "error storing last answer\n");
            }
            {
                extern int errloc;

                if (errloc != -1) {
                    display_and_clear_errstring();
                    /*fprintf(stderr, "%s", errstring);
                     * if (errstring[strlen(errstring) - 1] != '\n')
                     *  fprintf(stderr, "\n");
                     * free(errstring);
                     * errstring = NULL;*/
                }
            }
            free(line);
            linelen = 0;
        }
    }

    if (pretty_answer) {
        extern char *pa;

        free(pretty_answer);
        if (pa) {
            free(pa);
        }
    }
    EXIT_EARLY(EXIT_SUCCESS);
}                                      /*}}} */

static int
read_preload(void)
{                                      /*{{{ */
    int fd = openDotFile("wcalc_preload", O_RDONLY);

    switch (fd) {
        case -1:
            fprintf(stderr, "HOME environment variable unset. Cannot read preload file.\n");
            return 0;

        case -2:
            fprintf(stderr, "HOME environment variable is too long. Cannot read preload file.\n");
            return 0;
    }
    if (fd < 0) {
        if (errno == ENOENT) {
            /* The prefs file does not exist. This is okay, just means we can't read it. */
            return 1;
        }
        perror("Could not open preload file (~/.wcalc_preload)");
        return 0;
    }
    if (loadStateFD(fd, 0) < 0) {
        perror("Error reading preload file (~/.wcalc_preload)");
    }
    if (close(fd) < 0) {
        perror("Could not close preload file.");
    }
    return 1;
}                                      /*}}} */

#define CHECK_COLOR(x) else if (!strcasecmp(str, # x)) { return x; }

static enum ui_colors
str2color(const char *str)
{
    if (!strcasecmp(str, "NONE")) {
        return NONE;
    }
    CHECK_COLOR(BLACK)
    CHECK_COLOR(RED)
    CHECK_COLOR(GREEN)
    CHECK_COLOR(YELLOW)
    CHECK_COLOR(BLUE)
    CHECK_COLOR(MAGENTA)
    CHECK_COLOR(CYAN)
    CHECK_COLOR(WHITE)
    CHECK_COLOR(BOLDBLACK)
    CHECK_COLOR(BOLDRED)
    CHECK_COLOR(BOLDGREEN)
    CHECK_COLOR(BOLDYELLOW)
    CHECK_COLOR(BOLDBLUE)
    CHECK_COLOR(BOLDMAGENTA)
    CHECK_COLOR(BOLDCYAN)
    CHECK_COLOR(BOLDWHITE)
    else {
        fprintf(stderr, "Unrecognized color: '%s'\n", str);
        return RESET;
    }
}

#define HANDLECOLOR(x) else if (!strcasecmp(key, # x "]")) { uiselect[x] = str2color(value); }
static int
assign_color_prefs(const char *key,
                   const char *value)
{
    assert(key);
    assert(value);
    if (uiselect != color_ui) {
        fprintf(stderr, "Colors not enabled!\n");
        return 0;
    }
    if (!strcasecmp(key, "conversion_category]")) {
        uiselect[CONV_CAT] = str2color(value);
    } else if (!strcasecmp(key, "conversion_unit]")) {
        uiselect[CONV_UNIT] = str2color(value);
    }
#if defined(HAVE_EDITLINE) && ! defined(HAVE_READLINE_H)
    else if (!strcasecmp(key, "PROMPT]")) {
        fprintf(stderr, "Libeditline does not support prompt colors!\n");
    }
#else
    HANDLECOLOR(PROMPT)
#endif
    HANDLECOLOR(APPROX_ANSWER)
    HANDLECOLOR(EXACT_ANSWER)
    HANDLECOLOR(ERR_LOCATION)
    HANDLECOLOR(ERR_TEXT)
    HANDLECOLOR(PREF_NAME)
    HANDLECOLOR(PREF_VAL)
    HANDLECOLOR(PREF_CMD)
    HANDLECOLOR(STATUS)
    HANDLECOLOR(VAR_NAME)
    HANDLECOLOR(VAR_DESC)
    HANDLECOLOR(SUBVAR_NAME)
    HANDLECOLOR(EXPLANATION)
    else {
        char *res = strdup(key);
        *strchr(res, ']') = 0;
        fprintf(stderr, "Unrecognized colorable resource: '%s'\n", res);
        free(res);
        return 0;
    }
    return 1;
}

static int
set_pref(const char *key,
         const char *value)
{
    int ok = 1;
    if (!strcmp(key, "precision")) {
        char *endptr;
        long int tmp = strtol(value, &endptr, 0);
        if (('\0' != *endptr) || (value == endptr)) {
            if (value[0]) {
                config_error("Precision requires a valid integer (found '%s' instead).", value);
            } else {
                config_error("Precision requires a valid integer.");
            }
            ok = 0;
        } else {
            conf.precision = (int)tmp;
        }
    } else if (!strcmp(key, "show_equals")) {
        conf.print_equal = TRUEFALSE;
    } else if (!strcmp(key, "print_greeting")) {
        conf.print_greeting = TRUEFALSE;
    } else if (!strcmp(key, "flag_undeclared")) {
        conf.picky_variables = TRUEFALSE;
    } else if (!strcmp(key, "use_radians")) {
        conf.use_radians = TRUEFALSE;
    } else if (!strcmp(key, "print_prefixes")) {
        conf.print_prefixes = TRUEFALSE;
    } else if (!strcmp(key, "save_errors")) {
        conf.remember_errors = TRUEFALSE;
    } else if (!strcmp(key, "remember_errors")) {
        conf.remember_errors = TRUEFALSE;
    } else if (!strcmp(key, "precision_guard")) {
        conf.precision_guard = TRUEFALSE;
    } else if (!strcmp(key, "print_integers")) {
        conf.print_ints = TRUEFALSE;
    } else if (!strcmp(key, "print_delimiters")) {
        conf.print_commas = TRUEFALSE;
    } else if (!strcmp(key, "input_thousands_delimiter")) {
        if ((NULL == value) || ('\0' == value[0])) {
            config_error("Input thousands separator requires a single character value.");
            ok = 0;
        }
        if ((strlen(value) > 1)) {
            config_error("Input thousands separator requires a single character value (found '%s').", value);
            ok = 0;
        }
        if (value[0] == ' ') {
            config_error("'%c' cannot be the input thousands separator; input would be too confusing.", value[0]);
            ok = 0;
        }
        if ((conf.in_dec_delimiter != 0) && (conf.in_dec_delimiter == value[0])) {
            config_error("'%c' cannot be the input thousands separator; it is the input decimal separator.", value[0]);
            ok = 0;
        }
        if ((conf.in_dec_delimiter == 0) && (conf.dec_delimiter == value[0])) {
            config_error("'%c' cannot be the input thousands separator; it is the decimal separator.", value[0]);
            ok = 0;
        }
        if (ok) {
            conf.in_thou_delimiter = value[0];
        }
    } else if (!strcmp(key, "input_decimal_delimiter")) {
        if ((NULL == value) || ('\0' == value[0])) {
            config_error("Input decimal separator requires a single character value.");
            ok = 0;
        }
        if ((strlen(value) > 1)) {
            config_error("Input decimal separator requires a single character value (found '%s').", value);
            ok = 0;
        }
        if (value[0] == ' ') {
            config_error("'%c' cannot be the input decimal separator; input would be too confusing.", value[0]);
            ok = 0;
        }
        if ((conf.in_thou_delimiter != 0) && (conf.in_thou_delimiter == value[0])) {
            config_error("'%c' cannot be the input decimal separator; it is the input thousands separator.", value[0]);
            ok = 0;
        }
        if ((conf.in_thou_delimiter == 0) && (conf.thou_delimiter == value[0])) {
            config_error("'%c' cannot be the input decimal separator; it is the thousands separator.", value[0]);
            ok = 0;
        }
        if (ok) {
            conf.in_dec_delimiter = value[0];
        }
    } else if (!strcmp(key, "thousands_delimiter")) {
        if ((NULL == value) || ('\0' == value[0])) {
            config_error("Thousands separator requires a single character value.");
            ok = 0;
        }
        if ((strlen(value) > 1)) {
            config_error("Thousands separator requires a single character value (found '%s').", value);
            ok = 0;
        }
        if (conf.dec_delimiter == value[0]) {
            config_error("'%c' cannot be the thousands separator; it is the decimal separator.", value[0]);
            ok = 0;
        }
        if (ok) {
            conf.thou_delimiter = value[0];
        }
    } else if (!strcmp(key, "decimal_delimiter")) {
        if ((NULL == value) || ('\0' == value[0])) {
            config_error("Decimal separator requires a single character value.");
            ok = 0;
        }
        if ((strlen(value) > 1)) {
            config_error("Decimal separator requires a single character value (found '%s').", value);
            ok = 0;
        }
        if (conf.thou_delimiter == value[0]) {
            config_error("'%c' cannot be the decimal separator; it is the thousands separator.", value[0]);
            ok = 0;
        }
        if (ok) {
            conf.dec_delimiter = value[0];
        }
    } else if (!strcmp(key, "history_limit")) {
        if (!strcmp(value, "no")) {
            conf.history_limit = conf.history_limit_len = 0;
        } else {
            conf.history_limit     = 1;
            conf.history_limit_len = strtoul(value, NULL, 0);
        }
    } else if (!strcmp(key, "output_format")) {
        if ((NULL == value) || ('\0' == value[0])) {
            config_error("Output format requires a type. Supported types are: decimal, octal, binary, hex.");
            ok = 0;
        } else if (!strcmp(value, "decimal")) {
            conf.output_format = DECIMAL_FORMAT;
        } else if (!strcmp(value, "octal")) {
            conf.output_format = OCTAL_FORMAT;
        } else if (!strcmp(value, "binary")) {
            conf.output_format = BINARY_FORMAT;
        } else if (!strcmp(value, "hex") || !strcmp(value, "hexadecimal")) {
            conf.output_format = HEXADECIMAL_FORMAT;
        } else {
            config_error("Unrecognized output format. Supported formats are: decimal, octal, binary, hex.");
            ok = 0;
        }
    } else if (!strcmp(key, "rounding_indication")) {
        if ((NULL == value) || ('\0' == value[0])) {
            config_error("Rounding indication requires a type. Supported types are: none, simple, sig_fig.");
            ok = 0;
        } else if (!strcmp(value, "no") || !strcmp(value, "none")) {
            conf.rounding_indication = NO_ROUNDING_INDICATION;
        } else if (!strcmp(value, "simple")) {
            conf.rounding_indication = SIMPLE_ROUNDING_INDICATION;
        } else if (!strcmp(value, "sig_fig")) {
            conf.rounding_indication = SIG_FIG_ROUNDING_INDICATION;
        } else {
            config_error("Unrecognized rounding indication. Supported types are: none, simple, sig_fig.");
            ok = 0;
        }
    } else if (!strcmp(key, "scientific")) {
        if (!strcmp(value, "auto") || !strcmp(value, "automatic") || !strcmp(value, "yes") || !strcmp(value, "true") || !strcmp(value, "1")) {
            conf.scientific = automatic;
        } else if (!strcmp(value, "always")) {
            conf.scientific = always;
        } else {
            conf.scientific = never;
        }
    } else if (!strcmp(key, "c_style_mod")) {
        conf.c_style_mod = TRUEFALSE;
    } else if (!strcmp(key, "color_ui") || !strcmp(key, "color")) {
        conf.color_ui = TRUEFALSE;
        if (conf.color_ui) {
            uiselect = color_ui;
        } else {
            uiselect = black_and_white_ui;
        }
    } else if (!strncmp(key, "colors[", 7)) {
        ok = assign_color_prefs(key + 7, value);
    } else {
        config_error("Unrecognized key in wcalcrc: %s", key);
    }
    return ok;
}

static void
config_error(const char *format,
             ...)
{   /*{{{*/
    va_list args;

    fprintf(stderr, "Wcalc: Config: %s", config_type);
    va_start(args, format);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
} /*}}}*/

static size_t
copy_string(char       *d,
            const char *s,
            size_t      dmax,
            size_t      smax)
{   /*{{{*/
    size_t     dcurs  = 0, scurs = 0;
    const char quoted = (s[0] == '\'' || s[0] == '"');
    const char quote = s[0];

    if (quoted) {
        scurs = 1;
        while (dcurs < dmax && scurs < smax && s[scurs] != quote) {
            d[dcurs++] = s[scurs++];
        }
        scurs++; // skip the terminating quote
    } else {
        do {
            d[dcurs++] = s[scurs++];
        } while (dcurs < dmax && scurs < smax &&
                 (isalnum(s[scurs]) || s[scurs] == '[' || s[scurs] == ']' || s[scurs] == '.' || s[scurs] == '_' || s[scurs] == ' '));
    }
    if (scurs > smax) { return 0; }
    if (dcurs > dmax) { return 0; }
    d[dcurs] = 0;
    if (!quoted) {
        while (dcurs > 0 && iswspace(d[dcurs - 1])) {
            dcurs--;
            d[dcurs] = 0;
        }
    }
    return scurs;
} /*}}}*/

static int
read_prefs(void)
{                                      /*{{{ */
    int         fd   = openDotFile("wcalcrc", O_RDONLY);
    char        key[BIG_STRING], value[BIG_STRING];
    size_t      curs = 0;
    size_t      curs_max;
    char       *f_mutable;
    const char *f;

    switch (fd) {
        case -1:
            fprintf(stderr, "HOME environment variable unset. Cannot read preferences.\n");
            return 0;

        case -2:
            fprintf(stderr, "HOME environment variable is too long. Cannot read preferences.\n");
            return 0;
    }
    if (fd < 0) {
        if (errno == ENOENT) {
            /* The prefs file does not exist. This is okay, just means we can't read it. */
            return 1;
        }
        perror("Could not open preferences file (~/.wcalcrc)");
        return 0;
    }
    {
        struct stat info;
        if (fstat(fd, &info)) {
            perror("Could not determine the size of the preference file");
            close(fd);
            return 0;
        }
        curs_max  = info.st_size - 1;
        f_mutable = mmap(NULL, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (f_mutable == MAP_FAILED) {
            perror("Could not read the preference file");
            close(fd);
            return 0;
        }
        f = f_mutable;
    }
    config_type = "wcalcrc: ";
    assert(curs_max > curs);
    do {
        memset(value, 0, BIG_STRING);
        memset(key, 0, BIG_STRING);
        // read until we find a non-comment
        while (curs < curs_max && (iswspace(f[curs]) || f[curs] == '#')) {
            if (f[curs] == '#') { // skip to the next line
                do {
                    curs++;
                } while (curs < curs_max && f[curs] != '\n');
            }
            curs++;
        }
        if (curs >= curs_max) { break; } // not an error!
        // read in the key
        {
            size_t skip = copy_string(key, f + curs, BIG_STRING, curs_max - curs);
            if (!skip) {
                config_error("Incomplete key (%s)! (probably an unterminated quote)", key);
                goto err_exit;
            }
            curs += skip;
        }
        // eat the equals sign (and any surrounding space)
        while (curs < curs_max && iswspace(f[curs])) curs++;
        if ((curs == curs_max) || (f[curs] != '=')) {
            config_error("Expected equals (=) after key (%s)!", key);
            goto err_exit;
        }
        do {
            curs++;
        } while (curs < curs_max && iswspace(f[curs]) && f[curs] != '\n');
        if (curs == curs_max) {
            config_error("Key (%s) has no value!", key);
            goto err_exit;
        } else if (f[curs] == '\n') {
            config_error("Key (%s) has no value!", key);
            continue;
        }
        // read in the value
        {
            size_t skip = copy_string(value, f + curs, BIG_STRING, curs_max - curs);
            if (!skip) {
                config_error("Incomplete value (%s) for key (%s)! (probably an unterminated quote)", value, key);
                goto err_exit;
            }
            curs += skip;
        }

        if (!set_pref(key, value)) { goto err_exit; }

        // eat the rest of the line
        while (curs < curs_max && f[curs] != '\n') curs++;
        if (curs < curs_max) { curs++; }
    } while (curs < curs_max);
    if (munmap(f_mutable, curs_max + 1)) {
        perror("Unmapping the config file");
    }
    if (close(fd)) {
        perror("Closing the config file");
    }
    config_type = "";
    return 1;

err_exit:
    config_error("Corrupt config file. Cannot continue.");
    exit(EXIT_FAILURE);
}                                      /*}}} */

static void
prefline(const char *name,
         const char *val,
         const char *cmd)
{/*{{{*/
    if (name && val && cmd) {
        printf("%s%27s:%s %s%-24s%s -> ",
               colors[uiselect[PREF_NAME]], name, colors[uiselect[UNCOLOR]],
               colors[uiselect[PREF_VAL]], val, colors[uiselect[UNCOLOR]]);
        if (strchr(cmd, ',')) {
            unsigned offset = 0;
            while (strchr(cmd + offset, ',')) {
                unsigned cmdlen = strchr(cmd + offset, ',') - (cmd + offset);
                printf("%s%.*s%s, ",
                       colors[uiselect[PREF_CMD]], cmdlen, cmd + offset, colors[uiselect[UNCOLOR]]);
                offset += cmdlen + 1;
            }
            printf("%s%s%s\n",
                   colors[uiselect[PREF_CMD]], cmd + offset, colors[uiselect[UNCOLOR]]);
        } else {
            printf("%s%s%s\n",
                   colors[uiselect[PREF_CMD]], cmd, colors[uiselect[UNCOLOR]]);
        }
    } else if (name && val) {
        printf("%s%27s:%s %s%-24s%s\n",
               colors[uiselect[PREF_NAME]], name, colors[uiselect[UNCOLOR]],
               colors[uiselect[PREF_VAL]], val, colors[uiselect[UNCOLOR]]);
    }
}/*}}}*/

#define DP_YESNO(x) ((x) ? "yes" : "no")
void
display_prefs(void)
{/*{{{*/
    if (standard_output) {
        char tmp[50];
        sprintf(tmp, "%-3i %s", conf.precision, ((conf.precision == -1) ? "(auto)" : "      "));
        prefline("Display Precision", tmp, "\\p");
        sprintf(tmp, "%-24lu", (unsigned long)num_get_default_prec());
        prefline("Internal Precision", tmp, "\\bits");
        prefline("Scientific Notation",
                 (conf.scientific == always) ? "always" : (conf.scientific == never) ? "never " : "auto  ",
                 "\\e");
        prefline("Output Format", output_string(conf.output_format), "\\b,\\d,\\h,\\o");
        prefline("Use Radians", DP_YESNO(conf.use_radians), "\\r");
        prefline("Print Prefixes", DP_YESNO(conf.print_prefixes), "\\pre,\\prefixes");
        prefline("Avoid Abbreviating Integers", DP_YESNO(conf.print_ints), "\\ints");
        prefline("Rounding Indication",
                 conf.rounding_indication == SIMPLE_ROUNDING_INDICATION ? "yes (simple) " : (conf.rounding_indication == SIG_FIG_ROUNDING_INDICATION ? "yes (sig_fig)" : "no           "),
                 "\\round");
        prefline("Save Errors in History", DP_YESNO(conf.remember_errors), "\\re");
        sprintf(tmp, "'%c'", conf.thou_delimiter);
        prefline("Thousands Delimiter", tmp, "\\tsep");
        sprintf(tmp, "'%c'", conf.dec_delimiter);
        prefline("Decimal Delimiter", tmp, "\\dsep");
        prefline("Precision Guard", DP_YESNO(conf.precision_guard), "\\cons");
        prefline("History Limit", DP_YESNO(conf.history_limit), "\\hlimit");
        if (conf.history_limit) {
            sprintf(tmp, "%lu", conf.history_limit_len);
            prefline("History Limited To", tmp, NULL);
        }
        prefline("Verbose", DP_YESNO(conf.verbose), "\\verbose");
        prefline("Display Delimiters", DP_YESNO(conf.print_commas), "\\delim");
        prefline("Modulo Operator", (conf.c_style_mod ? "C-style    " : "not C-style"), "\\cmod");
    }
}/*}}}*/

void
display_status(const char *format,
               ...)
{/*{{{*/
    if (standard_output) {
        va_list args;

        printf("%s", colors[uiselect[STATUS]]);
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("%s\n", colors[uiselect[UNCOLOR]]);
    }
}/*}}}*/

void
display_output_format(int format)
{/*{{{*/
    if (standard_output) {
        switch (format) {
            case HEXADECIMAL_FORMAT:
                display_status("Hexadecimal Formatted Output");
                break;
            case OCTAL_FORMAT:
                display_status("Octal Formatted Output");
                break;
            case DECIMAL_FORMAT:
                display_status("Decimal Formatted Output");
                break;
            case BINARY_FORMAT:
                display_status("Binary Formatted Output");
                break;
        }
    }
}/*}}}*/

void
display_val(const char *name)
{/*{{{*/
    if (standard_output) {
        answer_t val;
        char     approx = 0;
        char    *err;
        display_and_clear_errstring();
        printf("%s%s%s", colors[uiselect[VAR_NAME]], name, colors[uiselect[UNCOLOR]]);
        val = getvar_full(name);
        if (val.exp) {
            printf(" %s=%s %s\n", colors[uiselect[EXACT_ANSWER]], colors[uiselect[UNCOLOR]], val.exp);
        } else {
            char *p = print_this_result(val.val, 0, &approx, &err);
            show_answer(err, approx, p);
        }
        if (val.desc) {
            printf(":: %s%s%s\n", colors[uiselect[VAR_DESC]], val.desc, colors[uiselect[UNCOLOR]]);
        }
    }
}/*}}}*/

void
display_var(variable_t *v,
            unsigned    count,
            unsigned    digits)
{/*{{{*/
    printf("%*u. %s%s%s", digits, count,
           colors[uiselect[VAR_NAME]], v->key, colors[uiselect[UNCOLOR]]);
    if (v->exp) {
        printf(" %s=%s %s\n",
               colors[uiselect[EXACT_ANSWER]], colors[uiselect[UNCOLOR]],
               v->expression);
    } else {
        char  approx = 0;
        char *err;
        char *p      = print_this_result(v->value, 0, &approx, &err);
        printf("display_var\n");
        show_answer(err, approx, p);
    }
    if (v->description) {
        printf("%*s %s%s%s\n", digits + 4, "::",
               colors[uiselect[VAR_DESC]], v->description, colors[uiselect[UNCOLOR]]);
    }
}/*}}}*/

void
display_expvar_explanation(const char *str,
                           const char *exp,
                           List        subvars,
                           const char *desc)
{/*{{{*/
    printf("%s%s%s is the expression: '%s'\n", colors[uiselect[VAR_NAME]], str,
           colors[uiselect[UNCOLOR]], exp);
    if (desc) {
        printf("Description: %s%s%s\n", colors[uiselect[VAR_DESC]], desc, colors[uiselect[UNCOLOR]]);
    }
    if (listLen(subvars) > 0) {
        unsigned maxnamelen = 0;
        {
            /* First, find the longest variable name... */
            ListIterator si     = getListIterator(subvars);
            char        *cursor = (char *)nextListElement(si);
            if (cursor != NULL) {
                maxnamelen = strlen(cursor);
                while ((cursor = (char *)nextListElement(si)) != NULL) {
                    unsigned len = strlen(cursor);
                    if (maxnamelen < len) { maxnamelen = len; }
                }
            }
            freeListIterator(si);
        }
        printf("%s%s%s uses the following variables:\n",
               colors[uiselect[VAR_NAME]], str, colors[uiselect[UNCOLOR]]);
        while (listLen(subvars) > 0) {
            char *curstr = (char *)getHeadOfList(subvars);
            char *value  = evalvar_noparse(curstr);

            printf("\t%s%*s%s\t(currently: %s)\n", colors[uiselect[SUBVAR_NAME]], -maxnamelen, curstr, colors[uiselect[UNCOLOR]],
                   value ? value : "undefined");
            if (curstr) {
                free(curstr);
            }
            if (value) {
                free(value);
            }
        }
    }
}/*}}}*/

void
display_valvar_explanation(const char *str,
                           Number     *val,
                           const char *desc)
{/*{{{*/
    printf("%s%s%s is a variable with the value: %s\n",
           colors[uiselect[VAR_NAME]], str, colors[uiselect[UNCOLOR]],
           print_this_result(*val, 0, NULL, NULL));
    if (desc) {
        printf("Description: %s%s%s\n", colors[uiselect[VAR_DESC]], desc, colors[uiselect[UNCOLOR]]);
    }
}/*}}}*/

void
display_explanation(const char *exp,
                    ...)
{/*{{{*/
    if (standard_output) {
        va_list args;

        printf("%s", colors[uiselect[EXPLANATION]]);
        va_start(args, exp);
        vprintf(exp, args);
        va_end(args);
        printf("%s\n", colors[uiselect[UNCOLOR]]);
    }
}/*}}}*/

void
display_stateline(const char *buf)
{/*{{{*/
    printf("-> %s\n", buf);
}/*}}}*/

void
display_consts(void)
{/*{{{*/
    size_t linelen = 0;

    for (size_t i = 0; consts[i].explanation; i++) {
        const char *const *const names = consts[i].names;
        for (size_t j = 0; names[j]; j++) {
            if (linelen + strlen(names[j]) + 2 > 70) {
                printf(",\n");
                linelen = 0;
            }
            if (linelen == 0) {
                printf("%s", names[j]);
                linelen = strlen(names[j]);
            } else {
                printf(", %s", names[j]);
                linelen += strlen(names[j]) + 2;
            }
        }
    }
    printf("\n");
}/*}}}*/

/* vim:set expandtab: */
