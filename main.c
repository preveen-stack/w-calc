/* Based (very loosely) on Ostermann's headstart for the shell project
* Modified by Kyle Wheeler
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <ctype.h> /* for isdigit */
#include <readline/readline.h>
#include <readline/history.h>

#include "calculator.h"
#include "parser.h"
#include "variables.h"

#define VERSION "1.0"

#define SUPPORTED_SYMBOLS "+ - () {} [] * %% / ! ^ > >= < <= == != && ||\nsin cos tan asin acos atan sinh cosh tanh asinh acosh atanh\nlog ln e pi\n"

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
	char *readme = NULL;
	int tty, i;
	short cmdline_input = 0;

	yydebug = 1;  /* turn on ugly YACC debugging */
	yydebug = 0;  /* turn off ugly YACC debugging */

	initvar();

	precision = -1;
	engineering = 0;
	standard_output = 1;

	/* Parse commandline options */
	for (i = 1; i < argc; ++i) {
		if (!strncmp(argv[i],"-P",2)) {
			if ((isdigit(*(argv[i]+2))) || ((*(argv[i]+2) == '-') && (isdigit(*(argv[i]+3))))) {
				precision = atoi((argv[i])+2);
			} else {
				fprintf(stderr,"-P option requires a valid integer without spaces.\n");
				fflush(stderr);
				exit(0);
			}
		} else if (!strcmp(argv[i],"-E")) {
			engineering = 1;
		} else if (!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help")) {
			print_command_help();
			exit(0);
		} else if (!strcmp(argv[i],"-v") || !strcmp(argv[i],"--version")) {
			printf("wcalc %s\n", VERSION);
			exit(0);
		} else {
			cmdline_input = 1;
			parseme(argv[i]);
		}
	}

	if (cmdline_input) exit(0);

	tty = isatty(0); /* Find out where stdin is coming from... */
	if (tty > 0) {
		/* if stdin is a keyboard or terminal, then use readline and prompts */
		printf("Enter an expression to evaluate, q to quit, or ? for help:\n");
		while (1) {
			lines = 1;
			fflush(NULL);
			readme = readline("-> ");
			if (! readme) {
				perror("reading line");
				break;
			}
			/* we need to add lines to the history */
			if (strlen(readme)) {
				add_history(readme);
				if (!strcmp(readme,"q") || !strcmp(readme,"quit")) {
					exit(0);
				} else if (!strcmp(readme,"?") || !strcmp(readme,"help")) {
					print_interactive_help();
				} else {
					parseme(readme);
				}
				putvar("a",last_answer);
			}
			free(readme);
		}
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
	printf("Wcalc 1.0\nby Kyle Wheeler.\nVersion 0.1.x written in C++ for CS240C at Ohio University, Jan 14 2000\n");
	printf("Version 0.2.x rewritten for lex/yacc in C, Nov 25 2001\n\n");
	printf( "This calculator recognized the following symbols:\n"
		 SUPPORTED_SYMBOLS
		 "The ! is factorial, not boolean.\n"
		 "Spaces are ignored (unknown symbols cause errors or are treated as variables)\n"
		 "This calculator accepts input in decimal and scientific notation.\n\n"
		 "P=x is the way to set the precision, where x is the desired precision.\n"
		 "    This setting only affects output.\n"
		 "E   toggles the formatting of output between decimal and scientific notation.\n"
		 "    This setting only applies when precision is specified.\n"
		 "a   in an expression represents the previous answer.\n"
		 "q   exits the program.\n"
		 "?   displays this help screen.\n\n"
		 "Variables are supported and may be assigned using the = operator.\n"
		 "To assign a variable, use the form:\n"
		 "foo = anylegalexpression\n"
		 "Thereafter, that variable name is the same as the literal value it represents.\n"
		 );
}

void print_command_help (void)
{
	printf("Wcalc 1.0\nby Kyle Wheeler.\nVersion 0.1.x written in C++ for CS240C at Ohio University, Jan 14 2000\n");
	printf("Version 0.2.x rewritten for lex/yacc in C, Nov 25 2001\n\n");
	printf( "This calculator recognized the following symbols:\n"
		 SUPPORTED_SYMBOLS
		 "The ! is factorial, not boolean.\n"
		 "Spaces are ignored (unknown symbols cause errors or are treated as variables)\n"
		 "This calculator accepts input in decimal and scientific notation.\n\n"
		 "-Px is the way to set the precision, where x is the desired precision.\n"
		 "    This setting only affects output.\n"
		 "-E  toggles the formatting of output between decimal and scientific notation.\n"
		 "    This setting only applies when precision is specified.\n"
		 "a   in an expression represents the previous answer.\n\n"
		 "Variables are supported and may be assigned using the = operator.\n"
		 "To assign a variable, use the form:\n"
		 "foo = anylegalexpression\n"
		 "Thereafter, that variable name is the same as the literal value it represents.\n\n"
		 "Executing this program without input expressions enters an 'interactive'\n"
		 "mode with a few more features and options.\n\n"
		 );
}
