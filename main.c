/* Based (very loosely) on Ostermann's headstart for the shell project
* Modified by Kyle Wheeler
*/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <ctype.h> /* for isdigit and isalpha */
#include <errno.h>
#include <fcntl.h> /* for open() */
#include <limits.h> /* for stroul() */

#ifdef HAVE_LIBREADLINE
# if defined(HAVE_READLINE_READLINE_H)
#  include <readline/readline.h>
# elif defined(HAVE_READLINE_H)
#  include <readline.h>
# else /* !defined(HAVE_READLINE_H) */
extern char *readline();
# endif /* !defined(HAVE_READLINE_H) */
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
extern void add_history ();
extern int write_history ();
extern int read_history ();
# endif /* defined(HAVE_READLINE_HISTORY_H) */
/* no history */
#endif /* HAVE_READLINE_HISTORY */

#include "calculator.h"
#include "parser.h"
#include "variables.h"
#include "help.h"

#define TRUEFALSE (! strcmp(value,"yes") || ! strcmp(value,"true"))

int read_prefs(char* filename);

/*
 * These are declared here because they're not in any header files.
 * yyparse() is declared with an empty argument list so that it is
 * compatible with the generated C code from yacc/bison.
 * This part is taken from http://www.bgw.org/tutorials/programming/c/lex_yacc/main.c
 */
extern int yyparse(void);
extern int yy_scan_string(const char*);

int main (int argc, char *argv[])
{
	extern int yydebug;
	extern int lines;
#ifdef HAVE_LIBREADLINE
	char *readme = NULL;
#else
	char readme[1000];
#endif
	int tty, i;
	short cmdline_input = 0;

	yydebug = 1;  /* turn on ugly YACC debugging */
	yydebug = 0;  /* turn off ugly YACC debugging */

	initvar();

	/* initialize the preferences */
	conf.precision = -1;
	conf.engineering = 0;
	standard_output = 1;
	conf.picky_variables = 1;
	conf.print_prefixes = 1;
	conf.precision_guard = 1;
	conf.thou_delimiter = ',';
	conf.dec_delimiter = '.';
	conf.print_equal = 1;

	/* load the preferences */
	{
		char *filename;
		filename = malloc((strlen(getenv("HOME"))+16)*sizeof(char));
		sprintf(filename,"%s/.wcalcrc",getenv("HOME"));
		if (read_prefs(filename))
			perror("Writing Preferences");
	}

	/* Parse commandline options */
	for (i = 1; i < argc; ++i) {
		if (!strncmp(argv[i],"-P",2)) {
			if ((isdigit((int)(argv[i][2]))) || (((argv[i][2]) == '-') && (isdigit((int)(argv[i][3]))))) {
				conf.precision = atoi((argv[i])+2);
			} else {
				fprintf(stderr,"-P option requires a valid integer without spaces.\n");
				fflush(stderr);
				exit(0);
			}
		} else if (!strcmp(argv[i],"-E")) {
			conf.engineering = 1;
		} else if (!strcmp(argv[i],"--help")) {
			print_command_help();
			exit(0);
		} else if (!strcmp(argv[i],"-v") || !strcmp(argv[i],"--version")) {
			printf("wcalc %s\n", VERSION);
			exit(0);
		} else if (!strcmp(argv[i],"-d")||!strcmp(argv[i],"--decimal")||!strcmp(argv[i],"-dec")) {
			conf.output_format = DECIMAL_FORMAT;
		} else if (!strcmp(argv[i],"-h")||!strcmp(argv[i],"--hexadecimal")||!strcmp(argv[i],"-hex")) {
			conf.output_format = HEXADECIMAL_FORMAT;
		} else if (!strcmp(argv[i],"-o")||!strcmp(argv[i],"--octal")||!strcmp(argv[i],"-oct")) {
			conf.output_format = OCTAL_FORMAT;
		} else if (!strcmp(argv[i],"-b")||!strcmp(argv[i],"--binary")||!strcmp(argv[i],"-bin")) {
			conf.output_format = BINARY_FORMAT;
		} else if (!strcmp(argv[i],"-p")) {
			conf.print_prefixes = 0;
		} else if (!strcmp(argv[i],"-l")||!strcmp(argv[i],"--lenient")) {
			conf.picky_variables = 0;
		} else if (!strcmp(argv[i],"-r")||!strcmp(argv[i],"--radians")) {
			conf.use_radians = 1;
		} else if (!strcmp(argv[i],"--yydebug")) {
			yydebug = 1;
		} else if (!strcmp(argv[i],"-n")) {
			conf.print_equal = 0;
		} else {
			cmdline_input = 1;
			parseme(argv[i]);
		}
	}

	if (cmdline_input) exit(0);

	tty = isatty(0); /* Find out where stdin is coming from... */
	if (tty > 0) {
		/* if stdin is a keyboard or terminal, then use readline and prompts */
#ifdef HAVE_READLINE_HISTORY
		char *filename;
		filename = malloc((strlen(getenv("HOME"))+16)*sizeof(char));
		sprintf(filename,"%s/.wcalc_history",getenv("HOME"));
		if (read_history(filename) && errno != ENOENT)
			perror("Reading History");
#endif
		printf("Enter an expression to evaluate, q to quit, or ? for help:\n");
		while (1) {
			lines = 1;
			fflush(NULL);
#ifdef HAVE_LIBREADLINE
			readme = readline("-> ");
#else
			{
				char c;
				unsigned int i = 0;
				memset(readme,0,1000);
				printf("-> ");
				fflush(stdout);
				c = fgetc(stdin);
				while (c != '\n' && i < 1000 && ! feof(stdin) && ! ferror(stdin)) {
					readme[i] = c;
					c = fgetc(stdin);
					++i;
				}
				if (feof(stdin) || ferror(stdin)) break;
			}
#endif
			if (! readme) {
				if (errno != 2)
					perror("reading line");
				break;
			}
			/* if there is a line */
			if (strlen(readme)) {
				if (!strcmp(readme,"q") || !strcmp(readme,"quit") || !strcmp(readme,"\\q")) {
					break;
				} else if (!strncmp(readme,"\\yydebug",8)) {
					addToHistory(readme);
					yydebug = ! yydebug;
					printf("Debug Mode %s\n",yydebug?"On":"Off");
				} else {
					parseme(readme);
					{
						extern char * errstring;
						if (!errstring || (errstring && !strlen(errstring)) || conf.remember_errors) {
							addToHistory(readme);
						}
					}
				}
				putval("a",last_answer);

				{ extern char * errstring;
				if (errstring && strlen(errstring)) {
					fprintf(stderr,"%s",errstring);
					if (errstring[strlen(errstring)-1] != '\n')
						fprintf(stderr,"\n");
					free(errstring);
					errstring=NULL;
				}
				}
			}
#ifdef HAVE_LIBREADLINE
			free(readme);
#endif
		}
#ifdef HAVE_READLINE_HISTORY
		if (write_history(filename))
			perror("Saving History");
		if (conf.history_limit) {
			if (history_truncate_file(filename,conf.history_limit_len))
				perror("Truncating History");
		}
		free(filename);
#endif
	} else if (tty < 0) {
		fprintf(stderr, "Could not determine terminal type.\n");
	} else {
		/* if stdin is ANYTHING ELSE (a pipe, a file, etc), don't prompt */
		yyparse();
	}

	exit(0);
}

int read_prefs(char * filename)
{
	int fd = open(filename,O_RDONLY);
	char key[1000], value[100];
	char cur, *curs = key;
	int retval = 1;
	fd = open(filename,O_RDONLY);
	if (fd < 0) return 0;
	retval = read(fd,curs,1);
	while (retval == 1) {
		char quoted = 0;
		curs = key;
		// read until we find a non-comment
		while (retval == 1) {
			// if we find a comment
			if (*curs == '#')
				// read until the end of line
				while (read(fd,curs,1) == 1 && *curs != '\n')
					;
			else if (isalpha(*curs))
				break;
			retval = read(fd,curs,1);
		}
		// read in the key
		quoted = 1;
		if (*curs != '\'') {
			++curs;
			quoted = 0;
		}
		while (retval == 1) {
			retval = read(fd,curs,1);
			if ((! quoted && isspace(*curs)) ||
				(quoted && *curs != '\''))
				break;
			++curs;
		}
		// read in the =
		while (retval == 1 && *curs != '=') {
			retval = read(fd,curs,1);
		}
		while (retval == 1 && (isspace(*curs) || *curs == '=')) {
			retval = read(fd,curs,1);
		}
		value[0] = *curs;
		*curs = 0;
		curs = value;
		// read in the value
		quoted = 1;
		if (*curs != '\'') {
			++curs;
			quoted = 0;
		}
		while (retval == 1) {
			retval = read(fd,curs,1);
			if ((! quoted && isspace(*curs)) ||
				(  quoted && *curs != '\''))
				break;
			++curs;
		}
		if (*curs == '\n')
			*curs = 0;
		if (!strcmp(key,"precision"))
			conf.precision = atoi(value);
		else if (!strcmp(key,"show_equals"))
			conf.print_equal = TRUEFALSE;
		else if (!strcmp(key,"engineering"))
			conf.engineering = TRUEFALSE;
		else if (!strcmp(key,"flag_undeclared"))
			conf.picky_variables = TRUEFALSE;
		else if (!strcmp(key,"use_radians"))
			conf.use_radians = TRUEFALSE;
		else if (!strcmp(key,"print_prefixes"))
			conf.print_prefixes = TRUEFALSE;
		else if (!strcmp(key,"save_errors"))
			conf.remember_errors = TRUEFALSE;
		else if (!strcmp(key,"precision_guard"))
			conf.precision_guard = TRUEFALSE;
		else if (!strcmp(key,"thousands_delimiter"))
			conf.thou_delimiter = value[0];
		else if (!strcmp(key,"decimal_delimiter"))
			conf.dec_delimiter = value[0];
		else if (!strcmp(key,"history_limit")) {
			if (!strcmp(value,"no")) 
				conf.history_limit = conf.history_limit_len = 0;
			else {
				conf.history_limit = 1;
				conf.history_limit_len = strtoul(value,NULL,0);
			}
		} else if (!strcmp(key,"output_format")) {
			if (! strcmp(value, "decimal"))
				conf.output_format = DECIMAL_FORMAT;
			else if (!strcmp(value, "octal"))
				conf.output_format = OCTAL_FORMAT;
			else if (!strcmp(value, "binary"))
				conf.output_format = BINARY_FORMAT;
			else if (!strcmp(value, "hex") || !strcmp(value,"hexadecimal"))
				conf.output_format = HEXADECIMAL_FORMAT;
		} else if (!strcmp(key,"rounding_indication")) {
			if (! strcmp(value, "no"))
				conf.rounding_indication = NO_ROUNDING_INDICATION;
			else if (! strcmp(value, "simple"))
				conf.rounding_indication = SIMPLE_ROUNDING_INDICATION;
			else if (! strcmp(value, "sig_fig"))
				conf.rounding_indication = SIG_FIG_ROUNDING_INDICATION;
		}
		memset(key,0,sizeof(key));
		memset(value,0,sizeof(value));
	}
	return 0;
}

