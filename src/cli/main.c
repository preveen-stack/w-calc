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
#include <ctype.h>                     /* for isdigit and isalpha */
#include <errno.h>
#include <fcntl.h>                     /* for open() */
#include <limits.h>                    /* for stroul() */
#include <sys/mman.h>                  /* for mmap() */
#include <assert.h>                    /* for assert() */
#include <stdarg.h>                    /* for va_start() */
#include "number.h"

#ifdef HAVE_LIBREADLINE
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
#include "help.h"
#include "files.h"
#include "historyManager.h"
#include "list.h"
#include "isconst.h"
#include "output.h"

#define TRUEFALSE  (!strcmp(value, "yes") || !strcmp(value, "true") || !strcmp(value, "1"))
#define BIG_STRING 4096
#define VERSION    PACKAGE_VERSION

static int read_prefs(void);
static int read_preload(void);

/*
 * These are declared here because they're not in any header files.
 * yyparse() is declared with an empty argument list so that it is
 * compatible with the generated C code from yacc/bison.
 * This part is taken from http://www.bgw.org/tutorials/programming/c/lex_yacc/main.c
 */
extern int yyparse(void);
extern int yy_scan_string(const char *);

#ifdef HAVE_LIBREADLINE
/*@null@*/
static List tc_options = NULL;

/*@null@*/
static char *tc_generator(const char *text,
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
static char *tc_rounding(const char *text,
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
static char *tc_engineering(const char *text,
                            int         state)
{                                      /*{{{ */
    static unsigned int i             = 0;
    char               *engineering[] = { "always", "never", "auto", "automatic", 0 };

    if (state == 0) {
        i = 0;
    }
    while (engineering[i] != NULL) {
        if (strncmp(text, engineering[i], strlen(text)) == 0) {
            return strdup(engineering[i++]);
        }
        i++;
    }
    return NULL;
}                                      /*}}} */

# define COMPLETE(strs)  do {                                                 \
        int compareword = 0;                                                  \
        int comparechar = 0;                                                  \
        int textcurs    = 0;                                                  \
        while (strs[compareword] != NULL) {                                   \
            if (text[textcurs] == 0) { /* add to the list of possibilities */ \
                addToList( &tc_options, strdup(strs[compareword]));           \
                textcurs    = 0;                                              \
                comparechar = 0;                                              \
                compareword ++;                                               \
            } else if (text[textcurs] == strs[compareword][comparechar]) {    \
                textcurs ++;                                                  \
                comparechar ++;                                               \
            } else { /* not a possibility: next! */                           \
                textcurs    = 0;                                              \
                compareword ++;                                               \
                comparechar = 0;                                              \
            }                                                                 \
        }                                                                     \
} while (0)
# define COMPLETE2(strs) do {                                                    \
        int compareword = 0;                                                     \
        int comparechar = 0;                                                     \
        int textcurs    = 0;                                                     \
        while (strs[compareword].label != NULL) {                                \
            if (text[textcurs] == 0) { /* add to the list of possibilities */    \
                addToList( &tc_options, strdup(strs[compareword].label));        \
                textcurs    = 0;                                                 \
                comparechar = 0;                                                 \
                compareword ++;                                                  \
            } else if (text[textcurs] == strs[compareword].label[comparechar]) { \
                textcurs ++;                                                     \
                comparechar ++;                                                  \
            } else { /* not a possibility: next! */                              \
                textcurs    = 0;                                                 \
                compareword ++;                                                  \
                comparechar = 0;                                                 \
            }                                                                    \
        }                                                                        \
} while (0)

static char **wcalc_completion(const char *text,
                               int         start,
                               int         end)
{                                      /*{{{ */
    /*extern const char *commands[];*/
    extern const char *qcommands[];
    extern const char *funcs[];
    char             **variables;
    char             **retvals = NULL;

    // printf("\ncompleting: %s\n", text);
    if ('\\' == rl_line_buffer[0]) {
        if ((NULL == strchr(rl_line_buffer, ' ')) &&
            (NULL == strchr(rl_line_buffer, '\t'))) {
            COMPLETE(qcommands);
        } else if (strncmp("\\explain", rl_line_buffer, 8) == 0) {
            int i = 8;

            while (isspace(rl_line_buffer[i])) ++i;
            if (i == start) {
                COMPLETE(qcommands);
                COMPLETE2(consts);
                COMPLETE(funcs);
                variables = listvarnames();
                COMPLETE(variables);
                free(variables);
            }
        } else if ((strncmp("\\open", rl_line_buffer, 5) == 0) ||
                   (strncmp("\\save", rl_line_buffer, 5) == 0)) {
            int i = 5;

            while (isspace(rl_line_buffer[i])) ++i;
            if (i == start) {
                retvals =
                    rl_completion_matches(text,
                                          rl_filename_completion_function);
                return retvals;
            }
        } else if (((strncmp("\\rou", rl_line_buffer, 4) == 0) &&
                    isspace(rl_line_buffer[4])) ||
                   ((strncmp("\\round", rl_line_buffer, 6) == 0) &&
                    isspace(rl_line_buffer[6])) ||
                   ((strncmp("\\rounding", rl_line_buffer, 9) == 0) &&
                    isspace(rl_line_buffer[9]))) {
            int i = 4;

            while (!isspace(rl_line_buffer[i])) ++i;
            while (isspace(rl_line_buffer[i])) ++i;
            if (i == start) {
                retvals = rl_completion_matches(text, tc_rounding);
                return retvals;
            }
        } else if (((strncmp("\\e", rl_line_buffer, 2) == 0) &&
                    isspace(rl_line_buffer[2])) ||
                   ((strncmp("\\eng", rl_line_buffer, 4) == 0) &&
                    isspace(rl_line_buffer[4])) ||
                   ((strncmp("\\engineering", rl_line_buffer, 12) == 0) &&
                    isspace(rl_line_buffer[12]))) {
            int i = 2;

            while (!isspace(rl_line_buffer[i])) ++i;
            while (isspace(rl_line_buffer[i])) ++i;
            if (i == start) {
                retvals = rl_completion_matches(text, tc_engineering);
                return retvals;
            }
        } else if (((strncmp("\\c", rl_line_buffer, 2) == 0) &&
                    isspace(rl_line_buffer[2])) ||
                   ((strncmp("\\conv", rl_line_buffer, 5) == 0) &&
                    isspace(rl_line_buffer[5])) ||
                   ((strncmp("\\convert", rl_line_buffer, 8) == 0) &&
                    isspace(rl_line_buffer[8]))) {
            int i = 2;
            /*extern const struct conversion lengths[], areas[], volumes[],
             *  masses[], speeds[], powers[], forces[], accelerations[];*/
            extern const struct conversion *conversions[];

            while (!isspace(rl_line_buffer[i])) ++i;
            while (isspace(rl_line_buffer[i])) ++i;
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

                while (!isspace(rl_line_buffer[i])) ++i;
                saved_char        = rl_line_buffer[i];
                rl_line_buffer[i] = 0;
                unit_cat          = identify_unit(unit1);
                rl_line_buffer[i] = saved_char;
                if (unit_cat != -1) {
                    while (isspace(rl_line_buffer[i])) ++i;
                    if (i == start) {
                        size_t unit;

                        /* complete on COMPATABLE units */
                        for (unit = 0; conversions[unit_cat][unit].name;
                             unit++) {
                            COMPLETE(conversions[unit_cat][unit].aka);
                        }
                        addToList(&tc_options, strdup("to"));
                    } else if ((strncmp(rl_line_buffer + i, "to", 2) == 0) &&
                               isspace(rl_line_buffer[i + 2])) {
                        i += 2;
                        while (isspace(rl_line_buffer[i])) ++i;
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
        COMPLETE(funcs);
        variables = listvarnames();
        COMPLETE(variables);
        free(variables);
    }
    rl_attempted_completion_over = 1;  // do not use standard file completion
    rl_completion_entry_function = tc_generator;
    retvals                      = rl_completion_matches(text, tc_generator);
    return retvals;
}                                      /*}}} */

#endif /* ifdef HAVE_LIBREADLINE */

enum ui_colors {
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

const char *const  black_and_white_ui[] = {
    "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
};
const char *const  color_ui[] = {
    "\033[0m",
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
const char *const *colors = black_and_white_ui;

static void PrintConversionUnitCategory(int nCategory)
{
    printf("\n%s%s%s\n", colors[BOLDYELLOW], conversion_names[nCategory], colors[RESET]);
    size_t unit, nAka;

    for (unit = 0; conversions[nCategory][unit].name; ++unit) {
        printf("\n%s%s%s: ", colors[CYAN], conversions[nCategory][unit].name, colors[RESET]);
        for (nAka = 0; nAka < 9; ++nAka) {
            if (conversions[nCategory][unit].aka[nAka] != NULL) {
                if (nAka > 0) { printf(", "); }
                printf("%s", conversions[nCategory][unit].aka[nAka]);
            }
        }
    }
    printf("\n\n");
}

void show_answer(char *err,
                 int   uncertain,
                 char *answer)
{   /*{{{*/
    if (err && strlen(err)) {
        display_and_clear_errstring();
    }
    if (uncertain) {
        printf("%s%s %s%s\n", colors[YELLOW], conf.print_equal ? "~=" : "  ", colors[RESET], answer);
    } else {
        printf("%s%s %s%s\n", colors[GREEN], conf.print_equal ? " =" : "  ", colors[RESET], answer);
    }
} /*}}}*/

void display_and_clear_errstring()
{                                      /*{{{ */
    extern int   scanerror;
    extern char *errstring;
    extern int   errloc;

    if (errstring && errstring[0]) {
        if (errloc != -1) {
            int i;

            fprintf(stderr, "   ");
            for (i = 0; i < errloc; i++) {
                fprintf(stderr, " ");
            }
            fprintf(stderr, "%s^%s\n", colors[BOLDMAGENTA], colors[RESET]);
            errloc = -1;
        }
        fprintf(stderr, "%s%s%s", colors[RED], errstring, colors[RESET]);
        if (errstring[strlen(errstring) - 1] != '\n') {
            fprintf(stderr, "\n");
        }
        free(errstring);
        errstring = NULL;
        scanerror = 0;
    }
}                                      /*}}} */

#define EXIT_EARLY(code) do {  \
        clearHistory();        \
        cleanupvar();          \
        num_free(last_answer); \
        lists_cleanup();       \
        fflush(NULL);          \
        exit(code);            \
} while (0)

int main(int   argc,
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

    yydebug = 1;                       /* turn on ugly YACC debugging */
    yydebug = 0;                       /* turn off ugly YACC debugging */

    initvar();
    lists_init();

    /* initialize the preferences */
    conf.precision           = -1;
    conf.engineering         = automatic;
    standard_output          = 1;
    conf.picky_variables     = 1;
    conf.print_prefixes      = 1;
    conf.precision_guard     = 1;
    conf.in_thou_delimiter   = 0;
    conf.in_dec_delimiter    = 0;
    conf.thou_delimiter      = ',';
    conf.dec_delimiter       = '.';
    conf.print_equal         = 1;
    conf.print_ints          = 0;
    conf.print_commas        = 0;
    conf.verbose             = 0;
    conf.c_style_mod         = 1;
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
    for (i = 1; i < argc; ++i) {
        if (!strncmp(argv[i], "-P", 2)) {
            long int argnum;
            char    *endptr;
            char    *valstr;

            if ((strlen(&argv[i][2]) == 0) && (i + 1 < argc)) {
                // pull the arg from i+1
                valstr = argv[++i];
            } else {
                valstr = &argv[i][2];
            }
            argnum = strtol(valstr, &endptr, 0);
            if ((*endptr != '\0') || (endptr == valstr)) {
                if (strlen(valstr)) {
                    fprintf(stderr,
                            "-P option requires a valid integer (found '%s' instead).\n", valstr);
                } else {
                    fprintf(stderr,
                            "-P option requires a valid integer.\n");
                }
                EXIT_EARLY(EXIT_FAILURE);
            } else {
                conf.precision = (int)argnum;
            }
        } else if (!strcmp(argv[i], "-E") ||
                   !strcmp(argv[i], "--engineering")) {
            conf.engineering = always;
        } else if (!strcmp(argv[i], "-EE")) {
            conf.engineering = never;
        } else if (!strcmp(argv[i], "-H") || !strcmp(argv[i], "--help")) {
            print_command_help();
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
                    int nCategory;
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
            conf.print_equal = !conf.print_equal;
        } else if (!strcmp(argv[i], "-c") ||
                   !strcmp(argv[i], "--conservative")) {
            conf.precision_guard = !conf.precision_guard;
        } else if (!strcmp(argv[i], "-R") || !strcmp(argv[i], "--remember")) {
            conf.remember_errors = !conf.remember_errors;
        } else if (!strncmp(argv[i], "--round=", 8)) {
            if (!strcmp(&(argv[i][8]), "no") ||
                !strcmp(&(argv[i][8]), "none")) {
                conf.rounding_indication = NO_ROUNDING_INDICATION;
            } else if (!strcmp(&(argv[i][8]), "simple")) {
                conf.rounding_indication = SIMPLE_ROUNDING_INDICATION;
            } else if (!strcmp(&(argv[i][8]), "sig_fig")) {
                conf.rounding_indication = SIG_FIG_ROUNDING_INDICATION;
            }
        } else if (!strcmp(argv[i], "--round")) {
            fprintf(stderr,
                    "--round requires an argument (none|simple|sig_fig)\n");
            EXIT_EARLY(EXIT_FAILURE);
        } else if (!strncmp(argv[i], "--idsep=", 7)) {
            if ((strlen(argv[i]) > 8) || (strlen(argv[i]) == 7)) {
                fprintf(stderr, "--idsep= must have an argument\n");
                EXIT_EARLY(EXIT_FAILURE);
            }
            if ((conf.in_thou_delimiter != argv[i][7]) && ((conf.in_thou_delimiter != 0) || (conf.thou_delimiter != argv[i][7]))) {
                conf.in_dec_delimiter = argv[i][7];
            } else {
                fprintf(stderr,
                        "%c cannot be the decimal separator. It is the thousands separator.\n",
                        argv[i][7]);
                EXIT_EARLY(EXIT_FAILURE);
            }
        } else if (!strncmp(argv[i], "--dsep=", 7)) {
            if ((strlen(argv[i]) > 8) || (strlen(argv[i]) == 7)) {
                fprintf(stderr, "--dsep= must have an argument\n");
                EXIT_EARLY(EXIT_FAILURE);
            }
            if (conf.thou_delimiter != argv[i][7]) {
                conf.dec_delimiter = argv[i][7];
            } else {
                fprintf(stderr,
                        "%c cannot be the decimal separator. It is the thousands separator.\n",
                        argv[i][7]);
                EXIT_EARLY(EXIT_FAILURE);
            }
        } else if (!strncmp(argv[i], "--itsep=", 7)) {
            if ((strlen(argv[i]) > 8) || (strlen(argv[i]) == 7)) {
                fprintf(stderr, "--itsep= must have an argument\n");
                EXIT_EARLY(EXIT_FAILURE);
            }
            if ((conf.in_dec_delimiter != argv[i][7]) && ((conf.in_dec_delimiter != 0) || (conf.dec_delimiter != argv[i][7]))) {
                conf.in_thou_delimiter = argv[i][7];
            } else {
                fprintf(stderr,
                        "%c cannot be the thousands separator. It is the decimal separator.\n",
                        argv[i][7]);
                EXIT_EARLY(EXIT_FAILURE);
            }
        } else if (!strncmp(argv[i], "--tsep=", 7)) {
            if ((strlen(argv[i]) > 8) || (strlen(argv[i]) == 7)) {
                fprintf(stderr, "--tsep= must have an argument\n");
                EXIT_EARLY(EXIT_FAILURE);
            }
            if (conf.dec_delimiter != argv[i][7]) {
                conf.thou_delimiter = argv[i][7];
            } else {
                fprintf(stderr,
                        "%c cannot be the thousands separator. It is the decimal separator.\n",
                        argv[i][7]);
                EXIT_EARLY(EXIT_FAILURE);
            }
        } else if (!strncmp(argv[i], "--bits", 6)) {
            unsigned long int argnum;
            char             *endptr;

            argnum = strtoul(&(argv[i][6]), &endptr, 0);
            if ((endptr != NULL) && (strlen(endptr) > 0)) {
                fprintf(stderr,
                        "--bits option requires a valid integer without spaces.\n");
                EXIT_EARLY(EXIT_FAILURE);
            } else {
                if (argnum < NUM_PREC_MIN) {
                    fprintf(stderr, "Minimum precision is %lu.\n", (unsigned long)NUM_PREC_MIN);
                } else if (argnum > NUM_PREC_MAX) {
                    fprintf(stderr, "Maximum precision is %lu.\n", (unsigned long)NUM_PREC_MAX);
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
                colors = color_ui;
            } else {
                colors = black_and_white_ui;
            }
        } else if (!strcmp(argv[i], "--defaults")) {
            /* ignore this argument */
        } else {
            extern char *errstring;

            if (isatty(1) == 0) { /* Find out where stdout is going */
                colors = black_and_white_ui;
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

    if (!cmdline_input) {
        extern char *errstring;
        char        *envinput = getenv("wcalc_input");
        if (envinput) {
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
        rl_attempted_completion_function = wcalc_completion;
        rl_basic_word_break_characters   = " \t\n\"\'+-*/[{()}]=<>!|~&^%";
#endif
        printf
            ("Enter an expression to evaluate, q to quit, or ? for help:\n");
        while (1) {
            lines = 1;
            fflush(NULL);
#ifdef HAVE_LIBREADLINE
            {
                char prompt[10];
                sprintf(prompt, "%s->%s ", colors[BLUE], colors[RESET]);
                readme = readline(prompt);
            }
#else
            {
                char         c;
                unsigned int i = 0;

                memset(readme, 0, BIG_STRING);
                printf("%s->%s ", colors[BLUE], colors[RESET]);
                fflush(stdout);
                c = fgetc(stdin);
                while (c != '\n' && i < BIG_STRING && !feof(stdin) &&
                       !ferror(stdin)) {
                    readme[i] = c;
                    c         = fgetc(stdin);
                    ++i;
                }
                if (feof(stdin) || ferror(stdin)) {
                    break;
                }
            }
#endif      /* ifdef HAVE_LIBREADLINE */
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
                        colors = color_ui;
                    } else {
                        colors = black_and_white_ui;
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
        char        *line, gotten;
        unsigned int linelen = 0, maxlinelen = BIG_STRING;
        extern int   show_line_numbers;

        show_line_numbers = 1;
        while (1) {
            line   = calloc(maxlinelen, sizeof(char));
            gotten = fgetc(stdin);
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
                    errloc = -1;
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

static int read_preload(void)
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

static int set_pref(const char *key,
                    const char *value)
{
    if (!strcmp(key, "precision")) {
        conf.precision = atoi(value);
    } else if (!strcmp(key, "show_equals")) {
        conf.print_equal = TRUEFALSE;
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
        conf.in_thou_delimiter = value[0];
    } else if (!strcmp(key, "input_decimal_delimiter")) {
        conf.in_dec_delimiter = value[0];
    } else if (!strcmp(key, "thousands_delimiter")) {
        conf.thou_delimiter = value[0];
    } else if (!strcmp(key, "decimal_delimiter")) {
        conf.dec_delimiter = value[0];
    } else if (!strcmp(key, "history_limit")) {
        if (!strcmp(value, "no")) {
            conf.history_limit = conf.history_limit_len = 0;
        } else {
            conf.history_limit     = 1;
            conf.history_limit_len = strtoul(value, NULL, 0);
        }
    } else if (!strcmp(key, "output_format")) {
        if (!strcmp(value, "decimal")) {
            conf.output_format = DECIMAL_FORMAT;
        } else if (!strcmp(value, "octal")) {
            conf.output_format = OCTAL_FORMAT;
        } else if (!strcmp(value, "binary")) {
            conf.output_format = BINARY_FORMAT;
        } else if (!strcmp(value, "hex") || !strcmp(value, "hexadecimal")) {
            conf.output_format = HEXADECIMAL_FORMAT;
        } else {
            fprintf(stderr, "Unrecognized output_format in wcalcrc.\n\tSupported formats are decimal, octal, binary, hex.\n");
            return 0;
        }
    } else if (!strcmp(key, "rounding_indication")) {
        if (!strcmp(value, "no") || !strcmp(value, "none")) {
            conf.rounding_indication = NO_ROUNDING_INDICATION;
        } else if (!strcmp(value, "simple")) {
            conf.rounding_indication = SIMPLE_ROUNDING_INDICATION;
        } else if (!strcmp(value, "sig_fig")) {
            conf.rounding_indication = SIG_FIG_ROUNDING_INDICATION;
        } else {
            fprintf(stderr, "Unrecognized rounding_indication in wcalcrc.\n\tSupported indication types are none, simple, sig_fig.\n");
            return 0;
        }
    } else if (!strcmp(key, "engineering")) {
        if (!strcmp(value, "auto") || !strcmp(value, "automatic") || !strcmp(value, "yes") || !strcmp(value, "true") || !strcmp(value, "1")) {
            conf.engineering = automatic;
        } else if (!strcmp(value, "always")) {
            conf.engineering = always;
        } else {
            conf.engineering = never;
        }
    } else if (!strcmp(key, "c_style_mod")) {
        conf.c_style_mod = TRUEFALSE;
    } else if (!strcmp(key, "color_ui")) {
        conf.color_ui = TRUEFALSE;
        if (conf.color_ui) {
            colors = color_ui;
        } else {
            colors = black_and_white_ui;
        }
    } else {
        fprintf(stderr, "Unrecognized key in wcalcrc: %s\n", key);
        return 0;
    }
    return 1;
}

static void config_error(const char *format,
                         ...)
{   /*{{{*/
    va_list args;

    fprintf(stderr, "Wcalc: Config: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
} /*}}}*/

static size_t copy_string(char       *d,
                          const char *s,
                          size_t      dmax,
                          size_t      smax)
{   /*{{{*/
    size_t     dcurs  = 0, scurs = 0;
    const char quoted = (s[0] == '\'');

    if (quoted) {
        scurs = 1;
        while (dcurs < dmax && scurs < smax && s[scurs] != '\'') {
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
    while (dcurs > 0 && isspace(d[dcurs - 1])) {
        dcurs--;
        d[dcurs] = 0;
    }
    return scurs;
} /*}}}*/

static int read_prefs(void)
{                                      /*{{{ */
    int         fd     = openDotFile("wcalcrc", O_RDONLY);
    char        key[BIG_STRING], value[BIG_STRING];
    size_t      curs   = 0;
    size_t      curs_max;
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
        curs_max = info.st_size - 1;
        f        = mmap(NULL, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (f == MAP_FAILED) {
            perror("Could not read the preference file");
            close(fd);
            return 0;
        }
    }
    assert(curs_max > curs);
    do {
        memset(value, 0, BIG_STRING);
        memset(key, 0, BIG_STRING);
        // read until we find a non-comment
        while (curs < curs_max && (isspace(f[curs]) || f[curs] == '#')) {
            if (f[curs] == '#') { // skip to the next line
                do {
                    curs++;
                } while (f[curs] != '\n' && curs < curs_max);
            }
            curs++;
        }
        if (curs >= curs_max) { break; } // not an error!
        // read in the key
        {
            size_t skip = copy_string(key, f + curs, BIG_STRING, curs_max - curs);
            if (!skip) {
                config_error("Incomplete key (%s)! (probably an unterminated quote)\n", key);
                goto err_exit;
            }
            curs += skip;
        }
        // eat the equals sign (and any surrounding space)
        while (curs < curs_max && isspace(f[curs])) curs++;
        if ((curs == curs_max) || (f[curs] != '=')) {
            config_error("Expected equals (=) after key (%s)!\n", key);
            goto err_exit;
        }
        do {
            curs++;
        } while (curs < curs_max && isspace(f[curs]));
        if (curs == curs_max) {
            config_error("Key (%s) has no value!\n", key);
            goto err_exit;
        }
        // read in the value
        {
            size_t skip = copy_string(value, f + curs, BIG_STRING, curs_max - curs);
            if (!skip) {
                config_error("Incomplete value (%s) for key (%s)! (probably an unterminated quote)\n", value, key);
                goto err_exit;
            }
            curs += skip;
        }

        if (!set_pref(key, value)) { goto err_exit; }

        // eat the rest of the line
        while (curs < curs_max && f[curs] != '\n') curs++;
        if (curs < curs_max) { curs++; }
    } while (curs < curs_max);
    if (munmap((void *)f, curs_max + 1)) {
        perror("Unmapping the config file");
    }
    if (close(fd)) {
        perror("Closing the config file");
    }
    return 1;

err_exit:
    config_error("Corrupt config file. Cannot continue.\n");
    exit(EXIT_FAILURE);
}                                      /*}}} */

/* vim:set expandtab: */
