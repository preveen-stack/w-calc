/*%pure-parser */
%{
/*#define REENTRANT_PARSER */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#if defined(STDC_HEADERS) || ! defined(HAVE_CONFIG_H)
# include <string.h>
#else
# if !HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
char *strchr (), *strrchr ();
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>
#include <unistd.h> /* for isatty() */
#include "calculator.h"
#include "variables.h"
#include "help.h"
#include "files.h"
#include "conversion.h"

	/* Based on the headstart code by Shawn Ostermann
	* modified by Kyle Wheeler
	*/

	/* include debugging code, in case we want it */
#define YYDEBUG 1

	/* to make it stop complaining about undeclared functions */
	int yylex();
	int yyerror(char *error_string, ...);

	int lines = 1;
	int synerrors = 0;
	short scanerror = 0;
	char * errstring = NULL;

	%}


%union	{ /* the types that we use in the tokens */
enum functions function;
enum operations operation;
mpfr_t number;
int integer;
enum commands cmd;
char * variable;
char character;
struct conv_req conver;
}

%token DEC_CMD OCT_CMD HEX_CMD BIN_CMD GUARD_CMD DISPLAY_PREFS_CMD
%token RADIAN_CMD PICKY_CMD REMEMBER_CMD LISTVAR_CMD
%token PRINT_HELP_CMD PREFIX_CMD INT_CMD
%token <integer> ENG_CMD HLIMIT_CMD ROUNDING_INDICATION_CMD
%token <integer> PRECISION_CMD BITS_CMD
%token <conver> CONVERT_CMD

%token EOLN PAR REN WBRA WKET WSBRA WSKET
%token WPLUS WMINUS WMULT WDIV WMOD WEQL WPOW WEXP WSQR
%token WOR WAND WEQUAL WNEQUAL WGT WLT WGEQ WLEQ
%token WLSHFT WRSHFT WBOR WBAND WBXOR

%token WBNOT WNOT WLOG WLN WROUND WABS WSQRT WCEIL WFLOOR WCBRT WLOGTWO WBANG
%token WSIN WCOS WTAN WASIN WACOS WATAN WSINH WCOSH WTANH WASINH WACOSH WATANH
%token WCOT WACOT WCOTH WACOTH WRAND WIRAND WFACT WCOMP

%token <number> NUMBER
%token <variable> VAR STRING OPEN_CMD SAVE_CMD
%token <character> DSEP_CMD TSEP_CMD

%type <number> exp exp_l2 exp_l3
%type <number> oval capsule parenthated
%type <integer> sign
%type <cmd> command
%type <function> func

%left WAND WOR WBAND WBOR
%left WEQUAL WNEQUAL WGT WLT WGEQ WLEQ
%left WMINUS WPLUS
%left WMULT WDIV WMOD WLSHFT WRSHFT
%right WBANG
%left WNOT WBNOT WNEG
%right WPOW

%expect 1211

%% 	/* beginning of the parsing rules	*/

input : lines
|
;

lines : oneline
| oneline lines
;

oneline : exp eoln
{
	if (scanerror) {
		scanerror = synerrors = 0;
		report_error("Error in scanner halts parser.");
	} else {
		if (! synerrors && ! yynerrs) {
			set_prettyanswer($1);
			mpfr_set(last_answer,$1,GMP_RNDN);
		} else {
			synerrors = 0;
			report_error("Too many errors.");
		}
	}
	mpfr_clear($1);
	compute = 1;
}
| assignment eoln {
	compute = 1;
}
| command eoln {
	switch ($1) {
		case redisplay:
			if (! synerrors) {
				set_prettyanswer(last_answer);
			} else {
				synerrors = 0;
				report_error("Too many errors.");
			}
				break;
		case nothing: break;
	}
	compute = 1;
}
| eoln	/* blank line */
{
	if (scanerror) {
		scanerror = synerrors = 0;
		report_error("2 Error in scanner halts parser.");
	}
	compute = 1;
}
| error eoln {
	report_error("3 Error in scanner halts parser.");
	compute = 1;
}
/* if we got an error on the line */
;

eoln : EOLN
{ ++lines; }
;

command : HEX_CMD {
	$$ = isatty(0)?redisplay:nothing;
	conf.output_format = HEXADECIMAL_FORMAT;
	if (standard_output)
		printf("Hexadecimal Formatted Output\n");}
| OCT_CMD {
	$$ = isatty(0)?redisplay:nothing;
	conf.output_format = OCTAL_FORMAT;
	if (standard_output)
		printf("Octal Formatted Output\n");}
| BIN_CMD {
	$$ = isatty(0)?redisplay:nothing;
	conf.output_format = BINARY_FORMAT;
	if (standard_output)
		printf("Binary Formatted Output\n");}
| DEC_CMD {
	$$ = isatty(0)?redisplay:nothing;
	conf.output_format = DECIMAL_FORMAT;
	if (standard_output)
		printf("Decimal Formatted Output\n");}
| DSEP_CMD {
	$$ = nothing;
	if (conf.thou_delimiter != $1) {
		conf.dec_delimiter = $1;
		if (standard_output)
			printf("%c is now the decimal separator.\n", $1);
	} else if (standard_output) {
		printf("%c cannot be the decimal separator. It is the thousands separator.\n", $1);
	}}
| TSEP_CMD {
	$$ = nothing;
	if (conf.dec_delimiter != $1) {
		conf.thou_delimiter = $1;
		if (standard_output)
			printf("%c is now the thousands separator.\n", $1);
	} else if (standard_output) {
		printf("%c cannot be the thousands separator. It is the decimal separator.\n", $1);
	}}
| PICKY_CMD {
	$$ = nothing;
	conf.picky_variables = ! conf.picky_variables;
	if (standard_output) {
		if (! conf.picky_variables)
			printf("Unknown variables are assumed to be 0.\n");
		else
			printf("Strict variable parsing.\n");
	}}
| INT_CMD {
	$$ = nothing;
	conf.print_ints = ! conf.print_ints;
	if (standard_output) {
		printf("Will %suse abbreviations for large integers.\n",conf.print_ints?"not ":"");
	}
}
| DISPLAY_PREFS_CMD {
	$$ = nothing;
	if (standard_output) {
		printf("        Display Precision: %i %s\n",conf.precision,((conf.precision==-1)?"(auto)":""));
		printf("       Internal Precision: %lu bits\n", (unsigned long) mpfr_get_default_prec());
		printf("       Engineering Output: %s\n",conf.engineering?"yes":"no");
		printf("            Output Format: %s\n",output_string(conf.output_format));
		printf("Flag Undeclared Variables: %s\n",conf.picky_variables?"yes":"no");
		printf("              Use Radians: %s\n",conf.use_radians?"yes":"no");
		printf("           Print Prefixes: %s\n",conf.print_prefixes?"yes":"no");
		printf("      Rounding Indication: %s\n",conf.rounding_indication==SIMPLE_ROUNDING_INDICATION?"yes (simple)":(conf.rounding_indication==SIG_FIG_ROUNDING_INDICATION?"yes (sig_fig)":"no"));
		printf("   Save Errors in History: %s\n",conf.remember_errors?"yes":"no");
		printf("      Thousands Delimiter: '%c'\n",conf.thou_delimiter);
		printf("        Decimal Delimiter: '%c'\n",conf.dec_delimiter);
		printf("          Precision Guard: %s\n",conf.precision_guard?"yes":"no");
		printf("            History Limit: %s\n",conf.history_limit?"yes":"no");
		if (conf.history_limit)
			printf("       History Limited To: %lu\n",conf.history_limit_len);
	}
}
| RADIAN_CMD {
	$$ = nothing;
	conf.use_radians = ! conf.use_radians;
	if (standard_output)
		printf("Now Using %s\n", conf.use_radians?"Radians":"Degrees");}
| GUARD_CMD {
	$$ = nothing;
	conf.precision_guard = ! conf.precision_guard;
	if (standard_output)
		printf("Now %sUsing Conservative Precision\n", conf.precision_guard?"":"Not ");}
| PRECISION_CMD {
	$$ = isatty(0)?redisplay:nothing;
	conf.precision = $1;
	if (standard_output) {
		printf("Precision = ");
		if (conf.precision == -1) printf("auto\n");
		else printf("%i\n", conf.precision);
	}}
| HLIMIT_CMD {
    $$ = nothing;
	if ($1) {
		conf.history_limit = 1;
		conf.history_limit_len = $1;
	} else {
		conf.history_limit = 0;
		conf.history_limit_len = 0;
	}
}
| LISTVAR_CMD {
	extern int contents;
	int i;
	for (i=0;i<=contents;i++) {
		struct variable *keyval = getrealnvar(i);
		if (! keyval) continue;
		printf("%s = ", keyval->key);
		if (keyval->exp)
			printf("%s\n", keyval->expression);
		else
			printf("%g\n", mpfr_get_d(keyval->value, GMP_RNDN));
	}
	$$ = nothing;
}
| ENG_CMD {
	$$ = isatty(0)?redisplay:nothing;
	if ($1 < 0) {
		conf.engineering = ! conf.engineering;
	} else {
		conf.engineering = $1;
	}
	if (standard_output)
		printf("Engineering notation is %s %s\n",conf.engineering?"enabled":"disabled", (conf.precision==-1)?"":"if the precision is set");}
| ROUNDING_INDICATION_CMD {
	$$ = nothing;
	if ($1 != -1)
		conf.rounding_indication = $1;
	else {
		conf.rounding_indication += 1;
		conf.rounding_indication %= 3;
	}
	if (standard_output) {
		printf("Will display ");
		switch(conf.rounding_indication) {
			case NO_ROUNDING_INDICATION: printf("no"); break;
			case SIMPLE_ROUNDING_INDICATION: printf("simple"); break;
			case SIG_FIG_ROUNDING_INDICATION: printf("significant figure"); break;
		}
		printf(" rounding indication\n");
	}
}
| PREFIX_CMD {
	$$ = nothing;
	conf.print_prefixes = ! conf.print_prefixes;
	if (standard_output)
		printf("Will %sprint number prefixes\n",conf.print_prefixes?"":"not ");
}
| REMEMBER_CMD {
	$$ = nothing;
	conf.remember_errors = ! conf.remember_errors;
	if (standard_output)
		printf("Statements that produce errors are %s.\n",conf.remember_errors?"recorded":"forgotten");
}
| PRINT_HELP_CMD {
	$$ = nothing;
	if (standard_output) {
		print_interactive_help();
	}
}
| OPEN_CMD {
	extern char* open_file;
	int i;
	open_file = malloc(strlen($1)+1);
	sprintf(open_file,"%s",$1);
	/* strip trailing spaces */
	for (i=strlen(open_file)-1;i>=0;i--) {
		if (open_file[i] != ' ') break;
		open_file[i] = 0;
	}
	if (strlen(open_file) == 0) {
		free(open_file);
		open_file = NULL;
		report_error("Please specify a file name to open.");
	} /*else {
		int retval;
		retval = loadState(open_file);
		if (retval) {
			report_error("Could not open file.");
			report_error((char*)strerror(retval));
		}
		free(open_file);
		open_file = NULL;
	}*/
	$$ = nothing;
}
| SAVE_CMD {
	int retval;
	retval = saveState($1);
	if (retval) {
		report_error("Could not save file.");
		report_error((char*)strerror(retval));
	}
	$$ = nothing;
}
| BITS_CMD {
	if ($1 < MPFR_PREC_MIN) {
		report_error("Number too small.");
	} else if ($1 > MPFR_PREC_MAX) {
		report_error("Number too large.");
	} else {
		mpfr_set_default_prec($1);
	}
	$$ = nothing;
}
| CONVERT_CMD
{
	int category;
	char * errstr;
	category = identify_units($1.u1,$1.u2);
	if (category == -1) {
		report_error("Units must be in the same category.");
	} else if (category == -2) {
		report_error("Units provided are not recognized.");
	} else if (category == -3) {
		errstr = calloc(sizeof(char),strlen($1.u1)+45);
		sprintf(errstr,"First unit provided was not recognized (%s).",$1.u1);
		report_error(errstr);
		free(errstr);
	} else if (category == -4) {
		errstr = calloc(sizeof(char),strlen($1.u2)+46);
		sprintf(errstr,"Second unit provided was not recognized (%s).",$1.u2);
		report_error(errstr);
		free(errstr);
	} else {
		uber_conversion(last_answer,category,unit_id(category,$1.u1),unit_id(category,$1.u2),last_answer);
	}
	free($1.u1);
	free($1.u2);
	$$ = redisplay;
}
;

assignment : VAR WEQL exp
{
	if (compute && ! scanerror) {
		/* if standard_error, q is reserved */
		if (standard_output && !strcmp($1,"q")) {
			report_error("q cannot be assigned a value. q is used to exit.");
		} else {
			if (putval($1,$3) == 0) {
				if (standard_output) {
					mpfr_t val;
					mpfr_init(val);
					printf($1);
					getvarval(val, $1);
					print_this_result(val);
					mpfr_clear(val);
				}
			} else {
				report_error("There was a problem assigning variables.");
			}
			mpfr_clear($3);
		}
	} else {
		scanerror = 0;
		report_error("Scanner error halts parser.");
	}
	free($1);
}
| VAR WEQL STRING
{
	if (compute && ! scanerror) {
		if (standard_output && !strcmp($1,"q")) {
			report_error("q cannot be assigned an expression. q is used to exit.");
		} else {
			if (putexp($1,$3) == 0) {
				if (standard_output) {
					printf("%s = %s\n", $1, getvar_full($1).exp);
				}
			} else {
				report_error("There was a problem assigning variables.");
			}
		}
	} else {
		scanerror = 0;
		report_error("Scanner error halts parser.");
	}
	free($1);
	free($3);
}
| NUMBER WEQL exp
{
	report_error("Constants cannot be assigned to other values.");
	mpfr_clear($3);
}
| NUMBER WEQL STRING
{
	report_error("Constants cannot be assigned to other values.");
}
| func WEQL STRING
{
	report_error("Functions cannot be assigned to values.");
}
| func WEQL exp
{
	report_error("Functions cannot be assigned to values.");
	mpfr_clear($3);
}
;

exp : exp WMINUS exp { mpfr_init($$); simple_exp($$, $1, wminus, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WPLUS exp { mpfr_init($$); simple_exp($$, $1, wplus, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WAND exp { mpfr_init($$); simple_exp($$, $1, wand, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WOR exp { mpfr_init($$); simple_exp($$, $1, wor, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WBOR exp { mpfr_init($$); simple_exp($$, $1, wbor, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WBXOR exp { mpfr_init($$); simple_exp($$, $1, wbxor, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WBAND exp { mpfr_init($$); simple_exp($$, $1, wband, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WEQUAL exp { mpfr_init($$); simple_exp($$, $1, wequal, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WNEQUAL exp { mpfr_init($$); simple_exp($$, $1, wnequal, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WGT exp { mpfr_init($$); simple_exp($$, $1, wgt, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WLT exp { mpfr_init($$); simple_exp($$, $1, wlt, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WGEQ exp { mpfr_init($$); simple_exp($$, $1, wgeq, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WLEQ exp { mpfr_init($$); simple_exp($$, $1, wleq, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WLSHFT exp { mpfr_init($$); simple_exp($$, $1, wlshft, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WRSHFT exp { mpfr_init($$); simple_exp($$, $1, wrshft, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WMULT exp { mpfr_init($$); simple_exp($$, $1, wmult, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WDIV exp { mpfr_init($$); simple_exp($$, $1, wdiv, $3); mpfr_clear($1); mpfr_clear($3); }
| exp WMOD exp { mpfr_init($$); simple_exp($$, $1, wmod, $3); mpfr_clear($1); mpfr_clear($3); }
| exp_l2
;

func : WSIN { $$ = wsin; }
| WCOS { $$ = wcos; }
| WTAN { $$ = wtan; }
| WCOT { $$ = wcot; }
| WASIN { $$ = wasin; }
| WACOS { $$ = wacos; }
| WATAN { $$ = watan; }
| WACOT { $$ = wacot; }
| WSINH { $$ = wsinh; }
| WCOSH { $$ = wcosh; }
| WTANH { $$ = wtanh; }
| WCOTH { $$ = wcoth; }
| WASINH { $$ = wasinh; }
| WACOSH { $$ = wacosh; }
| WATANH { $$ = watanh; }
| WACOTH { $$ = wacoth; }
| WLOG { $$ = wlog; }
| WLOGTWO { $$ = wlogtwo; }
| WLN { $$ = wln; }
| WROUND { $$ = wround; }
| WABS { $$ = wabs; }
| WSQRT { $$ = wsqrt; }
| WEXP { $$ = wexp; }
| WFLOOR { $$ = wfloor; }
| WCEIL { $$ = wceil; }
| WCBRT { $$ = wcbrt; }
| WRAND { $$ = wrand; }
| WIRAND { $$ = wirand; }
| WBNOT { $$ = wbnot; }
| WNOT { $$ = wnot; }
| WFACT { $$ = wfact; }
| WCOMP { $$ = wcomp; }
;

nullexp : PAR REN
| WBRA WKET
| WSBRA WSKET
;

sign : WMINUS { $$ = -1; }
| WPLUS { $$ = 1; }
| { $$ = 1; }
;

exp_l2 : exp_l3
| exp_l3 WSQR { /* this is a dumb feature */
				mpfr_init($$); mpfr_sqr($$,$1,GMP_RNDN); mpfr_clear($1); }
| sign exp_l2 oval { mpfr_init($$);
					 mpfr_mul($2,$2,$3,GMP_RNDN);
					 mpfr_mul_si($$,$2,$1,GMP_RNDN);
					 mpfr_clear($2);
					 mpfr_clear($3); }
;

oval : exp_l3 oval
| { mpfr_init_set_ui($$,1,GMP_RNDN); }
;

exp_l3 : capsule oval { mpfr_init($$);
                        simple_exp($$,$1,wmult,$2);
						mpfr_clear($1);
						mpfr_clear($2);}
| capsule WPOW sign exp_l3 oval { mpfr_init($$);
								  mpfr_mul_si($4,$4,$3,GMP_RNDN);
								  mpfr_pow($1,$1,$4,GMP_RNDN);
								  mpfr_mul($$,$1,$5,GMP_RNDN);
								  mpfr_clear($1);
								  mpfr_clear($4);
								  mpfr_clear($5);}
;

parenthated: PAR exp REN
		   { mpfr_init($$); mpfr_set($$,$2,GMP_RNDN); mpfr_clear($2); }
		   | WBRA exp WKET
		   { mpfr_init($$); mpfr_set($$,$2,GMP_RNDN); mpfr_clear($2); }
		   | WSBRA exp WSKET
		   { mpfr_init($$); mpfr_set($$,$2,GMP_RNDN); mpfr_clear($2); }
		   | nullexp { mpfr_init_set_ui($$,0,GMP_RNDN); }
		   ;

capsule: parenthated
	   { mpfr_init($$); mpfr_set($$,$1,GMP_RNDN); mpfr_clear($1); }
	   | NUMBER
	   | capsule WBANG
	   {
	   mpfr_init($$);
	   mpfr_fac_ui($$,mpfr_get_ui($1,GMP_RNDN),GMP_RNDN);
	   mpfr_clear($1);
	   }
	   | func parenthated %prec WMULT
	   {
	   mpfr_init($$);
	   uber_function($$,$1,$2);
	   mpfr_clear($2);
	   }
	   | func sign capsule %prec WEQUAL
	   {
	   mpfr_init($$);
	   mpfr_mul_si($3,$3,$2,GMP_RNDN);
	   uber_function($$,$1,$3);
	   mpfr_clear($3);
	   }
;

%%

int
yyerror(char *error_string, ...) {
    va_list ap;
    int line_nmb(void);

	/*    FILE *f = stderr; */
	char error[1000];

    va_start(ap,error_string);

    ++synerrors;

	sprintf(error,"Error on line %i: ", lines);
	vsprintf(error,error_string,ap);
	report_error(error);

	/*    fprintf(f,"Error on line %d: ", lines);
    vfprintf(f,error_string,ap);
    fprintf(f,"\n");
	fflush(f); */
    va_end(ap);
	return 0;
}
