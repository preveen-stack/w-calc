/*%pure-parser */
/* this may be a bison-only thing... */
/*%error-verbose*/
%{
/*#define REENTRANT_PARSER */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#if ! defined(HAVE_CONFIG_H) || HAVE_STRING_H
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

#include "number.h"

#include <stdarg.h>
#include <math.h>
#include <float.h>
#include <unistd.h> /* for isatty() */
#include "conf.h"
#include "calculator.h"
#include "result_printer.h" /* for get_last_result_str() */
#include "variables.h"
#include "files.h"
#include "conversion.h"
#include "number_formatting.h"
#include "output.h"

/* Based on the headstart code by Shawn Ostermann
* modified by Kyle Wheeler
*/

/* include debugging code, in case we want it */
#define YYDEBUG 1

/* to make it stop complaining about undeclared functions */
int yylex(void);
int yyerror(char *error_string, ...);

int lines = 1;
int synerrors = 0;
short scanerror = 0;
char * errstring = NULL;
int errloc = -1;
int show_line_numbers = 0;

/* Injected Dependencies (output functions) */
static void (*_display_prefs)(void);
static void (*_display_output_format)(int format);
static void (*_display_status)(const char *format,
                    ...);
static void (*_display_interactive_help)(void);
static void (*_display_val)(const char *name);

void init_parser(void (*dp)(void),
                 void (*dof)(int),
                 void (*ds)(const char*, ...),
                 void (*dih)(void),
                 void (*dv)(const char*))
{
    _display_prefs = dp;
    _display_output_format = dof;
    _display_status = ds;
    _display_interactive_help = dih;
    _display_val = dv;
}

%}


%union	{ /* the types that we use in the tokens */
enum functions function;
enum operations operation;
Number number;
long integer;
enum commands cmd;
char * variable;
char character;
struct conv_req conver;
}

%token DEC_CMD OCT_CMD HEX_CMD BIN_CMD DISPLAY_PREFS_CMD
%token RADIAN_CMD REMEMBER_CMD LISTVAR_CMD STORE_CMD CMOD_CMD
%token PRINT_HELP_CMD PREFIX_CMD INT_CMD VERBOSE_CMD DELIM_CMD
%token <variable> ASSERT_CMD
%token <integer> ENG_CMD HLIMIT_CMD ROUNDING_INDICATION_CMD
%token <integer> PRECISION_CMD BITS_CMD BASE_CMD GUARD_CMD
%token <conver> CONVERT_CMD

%token EOLN OPEN_PARENTHESES CLOSE_PARENTHESES OPEN_BRACE
%token CLOSE_BRACE OPEN_BRACKET CLOSE_BRACKET
%token WPLUS WMINUS WMULT WDIV WMOD EQUALS_SIGN WPOW WEXP WSQR
%token WOR WAND WEQUAL WNEQUAL WGT WLT WGEQ WLEQ
%token WLSHFT WRSHFT WBOR WBAND WBXOR

%token WBNOT WNOT WLOG WLN WROUND WABS WSQRT WCEIL WFLOOR WCBRT WLOGTWO WBANG
%token WSIN WCOS WTAN WASIN WACOS WATAN WSINH WCOSH WTANH WASINH WACOSH WATANH
%token WCOT WACOT WCOTH WACOTH WRAND WIRAND WFACT WCOMP WSEC WCSC WASEC WACSC
%token WSECH WCSCH WASECH WACSCH WEINT WGAMMA WLNGAMMA WZETA WSINC

%token <number> NUMBER
%token <variable> VARIABLE STRING OPEN_CMD SAVE_CMD ASSIGNMENT
%token <character> IDSEP_CMD DSEP_CMD ITSEP_CMD TSEP_CMD

%type <number> exp exp_l2 exp_l3
%type <number> oval capsule parenthated
%type <integer> sign
%type <cmd> command
%type <function> func
%type <variable> optionalstring

%left WAND WOR WBAND WBOR
%left WEQUAL WNEQUAL WGT WLT WGEQ WLEQ
%left WMINUS WPLUS
%left WMULT WDIV WMOD WLSHFT WRSHFT
%right WBANG WSQR
%left WNOT WBNOT WNEG
%right WPOW

%expect 1560

%% 	/* beginning of the parsing rules	*/

input : lines
|
;

lines : oneline
| oneline lines
;

oneline : exp eoln
{
	extern int line_is_a_command;

	if (scanerror) {
		scanerror = synerrors = 0;
	} else {
		if (! synerrors && ! yynerrs) {
                        set_last_answer($1);
		} else {
			synerrors = 0;
			report_error("Too many errors.");
		}
	}
	num_free($1);
	compute = 1;
	line_is_a_command = 0;
}
| assignment eoln {
	extern int line_is_a_command;

	compute = 1;
	line_is_a_command = 0;
}
| command eoln {
	extern int line_is_a_command;

	switch ($1) {
		case redisplay:
			if (! synerrors) {
                                reset_last_answer_str();
			} else {
				synerrors = 0;
				report_error("Too many errors.");
			}
				break;
		case nothing: break;
	}
	compute = 1;
	line_is_a_command = 0;
}
| eoln	/* blank line */
{
	extern int line_is_a_command;

	if (scanerror) {
		scanerror = synerrors = 0;
	}
	compute = 1;
	line_is_a_command = 0;
}
| error eoln {
	extern int line_is_a_command;

    /* there is the possibility of lost memory here,
     * because "error" has no data type
     * (and because I'm passing around *actual* Number's
	 * rather than pointers to them) */
    /* report_error("3 Error in scanner halts parser."); */
    compute = 1;
	line_is_a_command = 0;
}
/* if we got an error on the line */
;

eoln : EOLN { ++lines; }
;

command : HEX_CMD {
	$$ = isatty(0)?redisplay:nothing;
	getConf()->output_format = HEXADECIMAL_FORMAT;
        _display_output_format(HEXADECIMAL_FORMAT); }
| OCT_CMD {
	$$ = isatty(0)?redisplay:nothing;
	getConf()->output_format = OCTAL_FORMAT;
        _display_output_format(OCTAL_FORMAT); }
| BIN_CMD {
	$$ = isatty(0)?redisplay:nothing;
	getConf()->output_format = BINARY_FORMAT;
        _display_output_format(BINARY_FORMAT); }
| DEC_CMD {
	$$ = isatty(0)?redisplay:nothing;
	getConf()->output_format = DECIMAL_FORMAT;
        _display_output_format(DECIMAL_FORMAT); }
| ASSERT_CMD {
	if (strcmp($1, get_last_answer_str())) {
                fprintf(stderr, "Assertion on line %u:\n", lines);
		fprintf(stderr, "    Last answer was: '%s'\n", get_last_answer_str());
		fprintf(stderr, "...should have been: '%s'\n", $1);
		exit(EXIT_FAILURE);
	}
	free($1);
}
| DSEP_CMD {
	$$ = nothing;
	if (getConf()->thou_delimiter != $1) {
            getConf()->dec_delimiter = $1;
	    _display_status("%c is now the decimal separator.", $1);
	} else {
            extern int column;
            column --;
	    report_error("%c cannot be the decimal separator. It is the thousands separator.", $1);
	}}
| IDSEP_CMD {
	$$ = nothing;
        conf_t *conf = getConf();
	if (conf->in_thou_delimiter != $1 && (conf->in_thou_delimiter != 0 || conf->thou_delimiter != $1)) {
	    conf->in_dec_delimiter = $1;
	    _display_status("%c is now the decimal separator for input.\n", $1);
	} else {
            extern int column;
            column --;
	    report_error("%c cannot be the decimal separator. It is the thousands separator.\n", $1);
	}}
| TSEP_CMD {
	$$ = nothing;
	if (getConf()->dec_delimiter != $1) {
	    getConf()->thou_delimiter = $1;
	    _display_status("%c is now the thousands separator.\n", $1);
	} else if (standard_output) {
            extern int column;
            column --;
	    report_error("%c cannot be the thousands separator. It is the decimal separator.\n", $1);
	}}
| ITSEP_CMD {
	$$ = nothing;
        conf_t *conf = getConf();
	if (conf->in_dec_delimiter != $1 && (conf->in_dec_delimiter != 0 || conf->dec_delimiter != $1)) {
	    conf->in_thou_delimiter = $1;
	    _display_status("%c is now the thousands separator for input.\n", $1);
	} else  {
            extern int column;
            column --;
	    report_error("%c cannot be the thousands separator. It is the decimal separator.\n", $1);
	}
}
| DELIM_CMD {
	$$ = nothing;
        conf_t *conf = getConf();
	conf->print_commas = ! conf->print_commas;
	_display_status("Will %sshow separators when printing large numbers.\n",conf->print_commas?"":"not ");
}
| INT_CMD {
	$$ = nothing;
        conf_t *conf = getConf();
	conf->print_ints = ! conf->print_ints;
	_display_status("Will %suse abbreviations for large integers.\n",conf->print_ints?"not ":"");
}
| VERBOSE_CMD
{
	$$ = nothing;
        conf_t *conf = getConf();
	conf->verbose = ! conf->verbose;
	_display_status("Will %secho the lines to be evaluated.\n",conf->verbose?"":"not ");
}
| DISPLAY_PREFS_CMD {
        _display_prefs();
	$$ = nothing;
}
| RADIAN_CMD {
	$$ = nothing;
        conf_t *conf = getConf();
	conf->use_radians = ! conf->use_radians;
	_display_status("Now Using %s\n", conf->use_radians?"Radians":"Degrees");}
| GUARD_CMD {
	$$ = nothing;
        conf_t *conf = getConf();
        switch ($1) {
            case 0: conf->precision_guard = 0; break;
            case 1: conf->precision_guard = 1; break;
            case -1: conf->precision_guard = ! conf->precision_guard; break;
        }
	_display_status("Now %sUsing Conservative Precision\n", conf->precision_guard?"":"Not ");}
| PRECISION_CMD {
	$$ = isatty(0)?redisplay:nothing;
        conf_t *conf = getConf();
	conf->precision = $1;
        if (conf->precision == -1) {
            _display_status("Precision = auto");
        } else {
            _display_status("Precision = %i", conf->precision);
        } }
| HLIMIT_CMD {
        $$ = nothing;
        conf_t *conf = getConf();
	if ($1) {
		conf->history_limit = 1;
		conf->history_limit_len = $1;
	} else {
		conf->history_limit = 0;
		conf->history_limit_len = 0;
	}
}
| LISTVAR_CMD {
	printvariables();
	$$ = nothing;
}
| ENG_CMD {
        conf_t *conf = getConf();
	if ($1 < 0) {
		switch (conf->scientific) {
			case always: conf->scientific = never; break;
			case never: conf->scientific = automatic; break;
			case automatic: conf->scientific = always; break;
		}
	} else {
		switch($1) {
			case 1: conf->scientific = automatic; break;
			case 2: conf->scientific = always; break;
			case 3: conf->scientific = never; break;
		}
	}
	_display_status("Scientific notation is %s\n",(conf->scientific==always)?"always used":(conf->scientific==never)?"never used":"used if convenient");
	$$ = isatty(0)?redisplay:nothing;
}
| ROUNDING_INDICATION_CMD {
	$$ = nothing;
        conf_t *conf = getConf();
	if ($1 != -1)
		conf->rounding_indication = $1;
	else {
		conf->rounding_indication += 1;
		conf->rounding_indication %= 3;
	}
	_display_status("Will display %s rounding indication",
		(conf->rounding_indication==NO_ROUNDING_INDICATION)?"no":
                ((conf->rounding_indication==SIMPLE_ROUNDING_INDICATION)?"simple":"significant figure"));
}
| PREFIX_CMD {
	$$ = nothing;
        conf_t *conf = getConf();
	conf->print_prefixes = ! conf->print_prefixes;
	_display_status("Will %sprint number prefixes\n",conf->print_prefixes?"":"not ");
}
| REMEMBER_CMD {
	$$ = nothing;
        conf_t *conf = getConf();
	conf->remember_errors = ! conf->remember_errors;
	_display_status("Statements that produce errors are %s.\n",conf->remember_errors?"recorded":"forgotten");
}
| PRINT_HELP_CMD {
	$$ = nothing;
	_display_interactive_help();
}
| OPEN_CMD {
	extern char* open_file;
	int i;
	unsigned int len = strlen($1)+1;
	open_file = malloc(len);
        strncpy(open_file, $1, len);
	/* strip trailing spaces */
	for (i=strlen(open_file)-1;i>=0;i--) {
		if (open_file[i] != ' ') break;
		open_file[i] = 0;
	}
	if (strlen(open_file) == 0) {
		free(open_file);
		open_file = NULL;
		report_error("Please specify a file name to open.");
	}
	$$ = nothing;
}
| SAVE_CMD {
	int retval;
	retval = saveState($1);
	if (retval) {
		report_error("Could not save file. (%s)", (char*)strerror(retval));
	}
	$$ = nothing;
}
| BITS_CMD {
	if ($1 < NUM_PREC_MIN) {
		report_error("Minimum precision is %lu (you asked for %lu).\n", (unsigned long)NUM_PREC_MIN, $1);
	} else if ($1 > NUM_PREC_MAX) {
		report_error("Maximum precision is %lu (you asked for %lu).\n", (unsigned long)NUM_PREC_MAX, $1);
	} else {
		num_set_default_prec($1);
	}
	$$ = nothing;
}
| CONVERT_CMD
{
	int category = identify_units($1.u1,$1.u2);
	switch (category) {
		case -1:
			report_error("Units must be in the same category.");
			break;
		case -2:
			report_error("Units provided are not recognized.");
			break;
		case -3:
			report_error("First unit provided was not recognized (%s).", $1.u1);
			break;
		case -4:
			report_error("Second unit provided was not recognized (%s).", $1.u2);
			break;
		default:
			uber_conversion(*get_last_answer(),
                                        category,
                                        unit_id(category,$1.u1),
                                        unit_id(category,$1.u2),
                                        *get_last_answer());
	}
	free($1.u1);
	free($1.u2);
	$$ = redisplay;
}
| BASE_CMD
{
	if ($1 >= 2 && $1 <= 36) {
		char * str, junk;
                conf_t *conf = getConf();
		str = num_to_str_complex(*get_last_answer(), $1, conf->scientific, -1, conf->print_prefixes, &junk);
		_display_status("base %i: %s\n",$1,str);
	} else {
		report_error("Base must be greater than one and less than 37.");
	}
	$$ = nothing;
}
| STORE_CMD VARIABLE
{
	int retval = storeVar($2);
	if (retval == 0) {
		_display_status("successfully stored %s\n",$2);
	} else {
		report_error("Failure to store variable!");
	}
	free($2);
}
| CMOD_CMD
{
	$$ = nothing;
        conf_t *conf = getConf();
	conf->c_style_mod = ! conf->c_style_mod;
	_display_status("The mod (%%) operation will %sbehave like it does in the C programming language.\n",conf->c_style_mod?"":"not ");
}
;

optionalstring : STRING {
			   $$ = $1;
			   }
			   | {
			   $$ = NULL;
			   }
			   ;

assignment : ASSIGNMENT exp optionalstring
{
	if (compute && ! scanerror) {
		/* if standard_error, q is reserved */
		if (standard_output && !strcmp($1,"q")) {
			report_error("q cannot be assigned a value. q is used to exit.");
		} else {
			if (putval($1,$2,$3) == 0) {
                                _display_val($1);
			} else {
				report_error("There was a problem assigning the value.");
			}
		}
		num_free($2);
	} else {
		scanerror = 0;
		report_error("Scanner error halts parser.");
	}
	free($1);
	if ($3 != NULL) {
		free($3);
	}
}
| ASSIGNMENT STRING optionalstring
{
	if (compute && ! scanerror) {
		if (standard_output && !strcmp($1,"q")) {
			report_error("q cannot be assigned an expression. q is used to exit.");
		} else {
			if (putexp($1,$2,$3) == 0) {
                                _display_val($1);
			} else {
				report_error("There was a problem assigning the expression.");
			}
		}
	} else {
		scanerror = 0;
		report_error("Scanner error halts parser.");
	}
	free($1);
	free($2);
	if ($3 != NULL) {
		free($3);
	}
}
| NUMBER EQUALS_SIGN exp optionalstring
{
	report_error("Constants cannot be assigned to other values.");
	num_free($3);
	if ($4 != NULL) {
		free($4);
	}
}
| NUMBER EQUALS_SIGN STRING optionalstring
{
	report_error("Constants cannot be assigned to other values.");
	free($3);
	if ($4 != NULL) {
		free($4);
	}
}
;

exp : exp WMINUS exp { num_init($$); simple_exp($$, $1, wminus, $3); num_free($1); num_free($3); }
| exp WPLUS exp { num_init($$); simple_exp($$, $1, wplus, $3); num_free($1); num_free($3); }
| exp WAND exp { num_init($$); simple_exp($$, $1, wand, $3); num_free($1); num_free($3); }
| exp WOR exp { num_init($$); simple_exp($$, $1, wor, $3); num_free($1); num_free($3); }
| exp WBOR exp { num_init($$); simple_exp($$, $1, wbor, $3); num_free($1); num_free($3); }
| exp WBXOR exp { num_init($$); simple_exp($$, $1, wbxor, $3); num_free($1); num_free($3); }
| exp WBAND exp { num_init($$); simple_exp($$, $1, wband, $3); num_free($1); num_free($3); }
| exp WEQUAL exp { num_init($$); simple_exp($$, $1, wequal, $3); num_free($1); num_free($3); }
| exp WNEQUAL exp { num_init($$); simple_exp($$, $1, wnequal, $3); num_free($1); num_free($3); }
| exp WGT exp { num_init($$); simple_exp($$, $1, wgt, $3); num_free($1); num_free($3); }
| exp WLT exp { num_init($$); simple_exp($$, $1, wlt, $3); num_free($1); num_free($3); }
| exp WGEQ exp { num_init($$); simple_exp($$, $1, wgeq, $3); num_free($1); num_free($3); }
| exp WLEQ exp { num_init($$); simple_exp($$, $1, wleq, $3); num_free($1); num_free($3); }
| exp WLSHFT exp { num_init($$); simple_exp($$, $1, wlshft, $3); num_free($1); num_free($3); }
| exp WRSHFT exp { num_init($$); simple_exp($$, $1, wrshft, $3); num_free($1); num_free($3); }
| exp WMULT exp { num_init($$); simple_exp($$, $1, wmult, $3); num_free($1); num_free($3); }
| exp WDIV exp { num_init($$); simple_exp($$, $1, wdiv, $3); num_free($1); num_free($3); }
| exp WMOD exp { num_init($$); simple_exp($$, $1, wmod, $3); num_free($1); num_free($3); }
| exp_l2
;

func : WSIN { $$ = wsin; }
| WCOS { $$ = wcos; }
| WTAN { $$ = wtan; }
| WCOT { $$ = wcot; }
| WSEC { $$ = wsec; }
| WCSC { $$ = wcsc; }
| WASIN { $$ = wasin; }
| WACOS { $$ = wacos; }
| WATAN { $$ = watan; }
| WACOT { $$ = wacot; }
| WASEC { $$ = wasec; }
| WACSC { $$ = wacsc; }
| WSINH { $$ = wsinh; }
| WCOSH { $$ = wcosh; }
| WTANH { $$ = wtanh; }
| WCOTH { $$ = wcoth; }
| WSECH { $$ = wsech; }
| WCSCH { $$ = wcsch; }
| WASINH { $$ = wasinh; }
| WACOSH { $$ = wacosh; }
| WATANH { $$ = watanh; }
| WACOTH { $$ = wacoth; }
| WASECH { $$ = wasech; }
| WACSCH { $$ = wacsch; }
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
| WEINT { $$ = weint; }
| WGAMMA { $$ = wgamma; }
| WLNGAMMA { $$ = wlngamma; }
| WZETA { $$ = wzeta; }
| WSINC { $$ = wsinc; }
;

nullexp : OPEN_PARENTHESES CLOSE_PARENTHESES
| OPEN_BRACE CLOSE_BRACE
| OPEN_BRACKET CLOSE_BRACKET
;

sign : WMINUS { $$ = -1; }
| WPLUS { $$ = 1; }
| { $$ = 1; }
;

exp_l2 : exp_l3
| sign exp_l2 oval { num_init($$);
		     num_mul($2,$2,$3);
		     num_mul_si($$,$2,$1);
		     num_free($2);
		     num_free($3); }
;

oval : exp_l3 oval {
     num_init_set($$,$1);
     num_free($1);
     num_free($2);
}
| { num_init_set_ui($$,1); }
;

exp_l3 : capsule oval { num_init($$);
                        simple_exp($$,$1,wmult,$2);
			num_free($1);
			num_free($2);}
| capsule WPOW sign exp_l3 oval { num_init($$);
				  num_mul_si($4,$4,$3);
				  num_pow($1,$1,$4);
				  num_mul($$,$1,$5);
				  num_free($1);
				  num_free($4);
				  num_free($5);}
;

parenthated: OPEN_PARENTHESES exp CLOSE_PARENTHESES
		   { num_init($$); num_set($$,$2); num_free($2); }
		   | OPEN_BRACE exp CLOSE_BRACE
		   { num_init($$); num_set($$,$2); num_free($2); }
		   | OPEN_BRACKET exp CLOSE_BRACKET
		   { num_init($$); num_set($$,$2); num_free($2); }
		   | nullexp { num_init_set_ui($$,0); }
		   ;

capsule: parenthated
	   { num_init($$); num_set($$,$1); num_free($1); }
	   | NUMBER
	   | capsule WBANG
	   {
	   num_init($$);
	   num_factorial($$,num_get_ui($1));
	   num_free($1);
	   }
	   | capsule WSQR { num_init($$); num_sqr($$,$1); num_free($1); }
	   | func parenthated %prec WMULT
	   {
	   num_init($$);
	   uber_function($$,$1,$2);
	   num_free($2);
	   }
	   | func sign capsule %prec WEQUAL
	   {
	   num_init($$);
	   num_mul_si($3,$3,$2);
	   uber_function($$,$1,$3);
	   num_free($3);
	   }
;

%%

int
yyerror(char *error_string, ...) {
    va_list ap;
	char error[1000];

    ++synerrors;

    va_start(ap,error_string);
	vsnprintf(error,1000,error_string,ap);
    va_end(ap);

	report_error(error);
	return 0;
}
