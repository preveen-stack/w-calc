/* Based (very loosely) on Ostermann's headstart for the shell project
* Modified by Kyle Wheeler
*/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <ctype.h> /* for isdigit */
#include <errno.h>

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

#define SUPPORTED_SYMBOLS "+ - () {} [] * %% / ! ^ > >= < <= == != && || << >>\nsin cos tan asin acos atan sinh cosh tanh asinh acosh atanh\nlog ln round abs ceil floor sqrt\ne pi Na k Cc ec R G g Me Mp Mn Md u amu c h mu0 epsilon0\nmuB muN b mW mZ ao F Vm NAk eV sigma alpha gamma re\n"

#ifdef HAVE_READLINE_HISTORY
#define ADD_HISTORY(x) add_history(x)
#else
#define ADD_HISTORY(x)
#endif

void print_command_help(void);
void print_interactive_help(void);

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

	conf.precision = -1;
	conf.engineering = 0;
	standard_output = 1;
	conf.picky_variables = 1;
	conf.print_prefixes = 1;
	conf.precision_guard = 1;
	conf.thou_delimiter = ',';
	conf.dec_delimiter = '.';

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
		char filename[1000];
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
				if (feof(stdin) || ferror(stdin)) exit(0);
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
					ADD_HISTORY(readme);
					yydebug = ! yydebug;
					printf("Debug Mode %s\n",yydebug?"On":"Off");
				} else if (!strncmp(readme,"?",1) || !strncmp(readme,"help",4)) {
					ADD_HISTORY(readme);
					print_interactive_help();
				} else {
					parseme(readme);
					{
						extern char * errstring;
						if (!errstring || (errstring && !strlen(errstring)) || conf.remember_errors) {
							ADD_HISTORY(readme);
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
		if (history_truncate_file(filename,1000))
			perror("Truncating History");
#endif
	} else if (tty < 0) {
		fprintf(stderr, "Could not determine terminal type.\n");
	} else {
		/* if stdin is ANYTHING ELSE (a pipe, a file, etc), don't prompt */
		yyparse();
	}

	exit(0);
}

void print_interactive_help (void)
{
	printf("Wcalc %s by Kyle Wheeler.\nVersion 0.1.x written in C++ for CS240C at Ohio University, Jan 14 2000\n",VERSION);
	printf("Version 0.2.x rewritten for lex/yacc in C, Nov 25 2001\n");
	printf( "This calculator recognized the following symbols:\n"
		 SUPPORTED_SYMBOLS
		 "The ! is factorial or boolean, depending on use.\n"
		 "Spaces are ignored (unknown symbols cause errors or are treated as variables)\n"
		 "This calculator accepts input in decimal (1.00), scientific notation (1.0e5),\n"
		 "octal (088), hexadecimal (0xff), and binary (0b11).\n\n"
		 "\\px is the way to set the precision, where x is the desired precision.\n"
		 "    This setting only affects output.\n"
		 "\\e  toggles the formatting of output between decimal and scientific notation.\n"
		 "    This setting only applies when precision is specified.\n"
		 "a   in an expression represents the previous answer.\n"
		 "\\q  exits the program.\n"
		 "?   displays this help screen.\n"
		 "\\radians toggles radian mode.\n"
		 "\\commas toggles output delineation between commas and periods.\n"
		 "\\p or \\picky toggles stricter variable parsing rules.\n"
		 "\\r or \\radians toggles radian mode for sin-related functions.\n"
		 "\\s or \\strict toggles stricter number parsing rules.\n"
		 "\\dsepX sets the decimal separator character to X.\n"
		 "\\tsepX sets the thousands-place separator character to X.\n"
		 "\\dec, \\hex, \\oct, \\bin set the output format.\n"
		 "\\cons or \\conservative toggles precision guards.\n"
		 "\\li or \\list or \\listvars prints out the currently defined variables.\n\n"
		 "Variables are supported and may be assigned using the = operator.\n"
		 "To assign a variable, use the form:\n"
		 "foo = anylegalexpression\n"
		 "Thereafter, that variable name is the same as the literal value it represents.\n"
		 );
}

void print_command_help (void)
{
	printf("Wcalc %s by Kyle Wheeler.\nVersion 0.1.x written in C++ for CS240C at Ohio University, Jan 14 2000\n",VERSION);
	printf("Version 0.2.x rewritten for lex/yacc in C, Nov 25 2001\n\n");
	printf( "This calculator recognized the following symbols:\n"
		 SUPPORTED_SYMBOLS
		 "\nThe ! is factorial or boolean, depending on use.\n"
		 "Spaces are ignored (unknown symbols cause errors or are treated as variables)\n"
		 "This calculator accepts input in decimal (1.00), scientific notation (1.0e5),\n"
		 "octal (088), hexadecimal (0xff), and in binary (0b11).\n\n"
		 "-v, --version            Prints the version number.\n"
		 "-Px                      Sets the precision, where x is the desired precision.\n"
		 "                         This setting only affects output.\n"
		 "-E                       Toggles the formatting of output between decimal\n"
		 "                         and scientific notation.\n"
		 "                         This setting only applies when precision is specified.\n"
		 "-d, -dec, --decimal      Prints the output in decimal form.\n"
		 "-h, -hex, --hexadecimal  Prints the output in hexadeximal form.\n"
		 "-o, -oct, --octal        Prints the output in octal form.\n"
		 "-b, -bin, --binary       Prints the output in binary form.\n"
		 "-p                       Prints the prefixes for hexadecimal, octal and\n"
		 "                         binary forms.\n"
		 "-l, --lenient            Makes the parser assume that uninitialized variables\n"
		 "                         have a value of zero.\n"
		 "-r, --radians            This makes all trigonometric functions work in radians.\n\n"
		 "Variables are supported and may be assigned using the = operator.\n"
		 "To assign a variable, use the form:\n"
		 "foo = anylegalexpression\n"
		 "Thereafter, that variable name is the same as the literal value it represents.\n\n"
		 "Executing this program without any arguments enters an 'interactive' mode\n"
		 "with a few more features and options.\n\n"
		 );
}
