%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <unistd.h> /* for isatty() */
#include "calculator.h"
#include "variables.h"

	/* Based on the headstart code by Shawn Ostermann
	* modified by Kyle Wheeler
	*/

	/* include debugging code, in case we want it */
#define YYDEBUG 1

	int yylex(); /* to make it stop complaining about undeclared functions */

	int lines = 1;
	int synerrors = 0;
	short scanerror = 0;
	char * errstring = NULL;

	%}


%union	{ /* the types that we use in the tokens */
enum functions function;
enum operations operation;
double number;
enum commands cmd;
char * variable;
}

%token DEC_CMD OCT_CMD HEX_CMD BIN_CMD GUARD_CMD
%token RADIAN_CMD PICKY_CMD STRICT_CMD REMEMBER_CMD LISTVAR_CMD
%token <number> PRECISION_CMD ENG_CMD

%token EOLN PAR REN WBRA WKET WSBRA WSKET WPIPE
%token WPLUS WMINUS WMULT WDIV WMOD WEQL WEXP WSQR
%token WOR WAND WEQUAL WNEQUAL WGT WLT WGEQ WLEQ
%token WLSHFT WRSHFT

%token WNOT WLOG WLN WROUND WABS WSQRT WCEIL WFLOOR WCBRT
%token WSIN WCOS WTAN WASIN WACOS WATAN WSINH WCOSH WTANH WASINH WACOSH WATANH

%token <number> NUMBER
%token <variable> VAR STRING

%type <number> exp exp_l2 exp_l3 exp_l4
%type <number> oval capsule sign
%type <cmd> command
%type <function> func

%left WAND WOR
%left WEQUAL WNEQUAL WGT WLT WGEQ WLEQ
%left WMINUS WPLUS
%left WMULT WDIV WMOD WLSHFT WRSHFT
%left WEXP
%left WNOT

%expect 751

%% 	/* beginning of the parsing rules	*/

input : lines
|
;

lines : oneline
| oneline lines
;

oneline : exp
{
	if (scanerror) {
		scanerror = synerrors = 0;
		report_error("Error in scanner halts parser.");
	} else {
		push_value($1);
		if (! synerrors) {
			print_result();
		} else {
			synerrors = 0;
			report_error("Too many errors.");
		}
	}
	compute = 1;
}
eoln
| assignment eoln
| command eoln {
	switch ($1) {
		case redisplay:
			push_value(last_answer);
			if (! synerrors) {
				print_result();
			} else {
				synerrors = 0;
				report_error("Too many errors.");
			}
				break;
		case nothing: break;
	}
}
| eoln	/* blank line */
{
	if (scanerror) {
		scanerror = synerrors = 0;
		report_error("Error in scanner halts parser.");
	}
	compute = 1;
}	
| error eoln { compute = 0; }
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
| PICKY_CMD {
	$$ = nothing;
	conf.picky_variables = ! conf.picky_variables;
	if (standard_output) {
		if (! conf.picky_variables)
			printf("Unknown variables are assumed to be 0.\n");
		else
			printf("Strict variable parsing.\n");
	}}
| RADIAN_CMD {
	$$ = nothing;
	conf.use_radians = ! conf.use_radians;
	if (standard_output)
		printf("%sUsing Radians\n", conf.use_radians?"":"Not ");}
| GUARD_CMD {
	$$ = nothing;
	conf.precision_guard = ! conf.precision_guard;
	if (standard_output)
		printf("%sUsing Conservative Precision\n", conf.precision_guard?"":"Not ");}
| PRECISION_CMD {
	$$ = isatty(0)?redisplay:nothing;
	conf.precision = $1;
	if (standard_output) {
		printf("Precision = ");
		if (conf.precision == -1) printf("auto\n");
		else printf("%i\n", conf.precision);
	}}
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
			printf("%g\n", keyval->value);
	}
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
| STRICT_CMD {
	$$ = nothing;
	conf.strict_syntax = ! conf.strict_syntax;
	if (standard_output)
		printf("%s Syntax\n",conf.strict_syntax?"Strict":"Forgiving");
}
| REMEMBER_CMD {
	$$ = nothing;
	conf.remember_errors = ! conf.remember_errors;
	if (standard_output)
		printf("Statements that produce errors are %s.\n",conf.remember_errors?"recorded":"forgotten");
}
;

assignment : VAR WEQL exp
{
	if (compute && ! scanerror) {
		/* if standard_error, q is reserved */
		if (standard_output && !strcmp($1,"q")) {
			printf("q cannot be assigned a value. q is used to exit.\n");
		} else {
			if (putval($1,$3) == 0) {
				if (standard_output) {
					printf("%s = %f\n", $1, getvar($1).val);
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
}
| VAR WEQL STRING
{
	if (compute && ! scanerror) {
		if (standard_output && !strcmp($1,"q")) {
			printf("q cannot be assigned a value. q is used to exit.\n");
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
;

exp : exp WMINUS exp { $$ = simple_exp($1, wminus, $3); }
| exp WPLUS exp { $$ = simple_exp($1, wplus, $3); }
| exp WAND exp { $$ = simple_exp($1, wand, $3); }
| exp WOR exp { $$ = simple_exp($1, wor, $3); }
| exp WEQUAL exp { $$ = simple_exp($1, wequal, $3); }
| exp WNEQUAL exp { $$ = simple_exp($1, wnequal, $3); }
| exp WGT exp { $$ = simple_exp($1, wgt, $3); }
| exp WLT exp { $$ = simple_exp($1, wlt, $3); }
| exp WGEQ exp { $$ = simple_exp($1, wgeq, $3); }
| exp WLEQ exp { $$ = simple_exp($1, wleq, $3); }
| exp WLSHFT exp { $$ = simple_exp($1, wlshft, $3); }
| exp WRSHFT exp { $$ = simple_exp($1, wrshft, $3); }
| WNOT exp { $$ = ! $2; }
| exp_l2
;

exp_l2 : exp_l2 WMULT exp_l2 { $$ = simple_exp($1, wmult, $3); }
| exp_l2 WDIV exp_l2 { $$ = simple_exp($1, wdiv, $3); }
| exp_l2 WMOD exp_l2 { $$ = simple_exp($1, wmod, $3); }
| exp_l3
;

func : WSIN { $$ = wsin; }
| WCOS { $$ = wcos; }
| WTAN { $$ = wtan; }
| WASIN { $$ = wasin; }
| WACOS { $$ = wacos; }
| WATAN { $$ = watan; }
| WSINH { $$ = wsinh; }
| WCOSH { $$ = wcosh; }
| WTANH { $$ = wtanh; }
| WASINH { $$ = wasinh; }
| WACOSH { $$ = wacosh; }
| WATANH { $$ = watanh; }
| WLOG { $$ = wlog; }
| WLN { $$ = wln; }
| WROUND { $$ = wround; }
| WABS { $$ = wabs; }
| WSQRT { $$ = wsqrt; }
| WFLOOR { $$ = wfloor; }
| WCEIL { $$ = wceil; }
| WCBRT { $$ = wcbrt; }
;

null : PAR REN
| WBRA WKET
| WSBRA WSKET
;

sign : WMINUS { $$ = -1; }
| WPLUS { $$ = 1; }
| { $$ = 1; }
;

exp_l3 : exp_l4
| sign exp_l3 oval { $$ = $1 * $2 * $3; }
;

oval : exp_l4 oval
| { $$ = 1; }
;

exp_l4 : capsule oval { $$ = $1 * $2; }
| capsule WEXP sign exp_l4 oval { $$ = pow($1,$3*$4) * $5; }
;

capsule: PAR exp REN { $$ = $2; }
| WBRA exp WKET { $$ = $2; }
| WSBRA exp WSKET { $$ = $2; }
| WPIPE exp WPIPE { $$ = fabs($2); }
| null { $$ = 0; }
| NUMBER
| exp_l4 WNOT { $$ = fact($1); }
| exp_l4 WSQR { $$ = pow($1,2); }
| func sign capsule { $$ = uber_function($1,$2*$3); }
| VAR
{
	/* Don't include reserved variables */
	if (standard_output && (!strcmp($1,"P") || !strcmp($1,"E") || !strcmp($1,"M") || !strcmp($1,"q"))) {
		fprintf(stderr,"%s is a reserved variable.\n",$1);
		compute = 0;
		$$ = 0;
	} else {
		struct answer temp = getvar($1);
		if (! temp.err && ! temp.exp) {
			$$ = temp.val;
		} else {
			$$ = 0;
			if (conf.picky_variables) {
				char * error = malloc(sizeof(char)*(strlen($1)+45));
				sprintf(error,"%s does not exist or was not properly parsed.",$1);
				report_error(error);
				compute = 0;
				free(error);
			}
		}
	}
}

;

%%

int
yyerror(char *error_string, ...)
{
    va_list ap;
    int line_nmb(void);

	//    FILE *f = stderr;
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
