%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <float.h>
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
	char errstring[1000] = "";

	%}


%union	{ /* the types that we use in the tokens */
enum functions function;
enum operations operation;
double number;
char * variable;
}

%token DEC_CMD OCT_CMD HEX_CMD BIN_CMD
%token EOLN PAR REN WBRA WKET WSBRA WSKET WPIPE
%token WPLUS WMINUS WMULT WDIV WMOD WEQL WEXP
%token WOR WAND WEQUAL WNEQUAL WGT WLT WGEQ WLEQ
%token WNOT WLOG WLN WROUND WABS WSQRT WCEIL WFLOOR
%token WSIN WCOS WTAN WASIN WACOS WATAN WSINH WCOSH WTANH WASINH WACOSH WATANH
%token <number> NUMBER
%token <variable> VAR

%type <number> exp exp_l2 exp_l3 exp_l4
%type <number> oval capsule sign
%type <number> command
%type <function> func

%left WAND WOR
%left WEQUAL WNEQUAL WGT WLT WGEQ WLEQ
%left WMINUS WPLUS
%left WMULT WDIV WMOD
%left WEXP
%left WNOT

%expect 641

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
	output_format = $1;
	push_value(last_answer);
	if (! synerrors) {
		print_result();
	} else {
		synerrors = 0;
		report_error("Too many errors.");
	}
}
| eoln	/* blank line, do nothing */
| error eoln { compute = 0; }
/* if we got an error on the line */
;

eoln : EOLN
{ ++lines; }
;

command : HEX_CMD {
	$$ = HEXADECIMAL_FORMAT;
	printf("%s",standard_output?"Hexadecimal Formatted Output\n":"");}
| OCT_CMD {
	$$ = OCTAL_FORMAT;
	printf("%s",standard_output?"Octal Formatted Output\n":"");}
| BIN_CMD {
	$$ = BINARY_FORMAT;
	printf("%s",standard_output?"Binary Formatted Output\n":"");}
| DEC_CMD {
	$$ = DECIMAL_FORMAT;
	printf("%s",standard_output?"Decimal Formatted Output\n":"");}
;

assignment : VAR WEQL exp
{
	if (compute && ! scanerror) {
		/* if standard_error, P, E, and q are reserved */
		if (standard_output && !strcmp($1,"P")) {
			precision = $3;
			printf("Precision = %i\n",precision);
		} else if (standard_output && !strcmp($1,"E")) {
			engineering = $3;
			printf("Engineering notation is %s %s\n",engineering?"enabled":"disabled", precision?"":"if the precision is set to a nonzero value");
		} else if (standard_output && !strcmp($1,"q")) {
			printf("q cannot be assigned a value. q is used to exit.\n");
		} else {
			if (putvar($1,$3) == 0) {
				if (standard_output) {
					printf("%s = %g\n", $1, *getvar($1));
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
;

/* Expression priority levels are of the following form:
* expressionlevel : nextexpressionlevel
*                 | anythingspecial
*                 | expressionlevel thisleveloperations nextexpressionlevel
*                 ;
*/

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
| func sign capsule { $$ = uber_function($1,$2*$3); }
| VAR
{
	/* Don't include reserved variables */
	if (standard_output && (!strcmp($1,"P") || !strcmp($1,"E") || !strcmp($1,"M") || !strcmp($1,"q"))) {
		fprintf(stderr,"%s is a reserved variable.\n",$1);
		compute = 0;
		$$ = 0;
	} else {
		double * temp = getvar($1);
		if (temp)
			$$ = *temp;
		else {
			$$ = 0;
			if (picky_variables) {
				char * error = malloc(sizeof(char)*(strlen($1)+18));
				sprintf(error,"%s does not exist.",$1);
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
