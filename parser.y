%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
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
	short compute = 1;
	char errstring[1000] = "";

	%}


%union	{ /* the types that we use in the tokens */
enum functions function;
enum operations operation;
double number;
char * variable;
}



%token EOLN PAR REN WBRA WKET WSBRA WSKET
%token WPLUS WMINUS WMULT WDIV WMOD WEQL WEXP
%token WOR WAND WEQUAL WNEQUAL WGT WLT WGEQ WLEQ
%token WFACT WLOG WLN
%token WSIN WCOS WTAN WASIN WACOS WATAN WSINH WCOSH WTANH WASINH WACOSH WATANH
%token <number> NUMBER
%token <operation> OPER
%token <variable> VAR

%type <number> expression expression_ltwo expression_lthree expression_lfour expression_lfive
%type <number> value
%type <operation> base_level_op level_two_op level_three_op level_four_op level_five_op
%type <function> func

%% 	/* beginning of the parsing rules	*/
input : lines
|
;

lines : oneline
| oneline lines
;

oneline : expression
{
	if (scanerror) {
		scanerror = synerrors = 0;
		report_error("Error in scanner halts parser.");
	} else {
		push_value($1);
		if (! synerrors)
			print_result();
		else {
			synerrors = 0;
			report_error("Too many errors.");
		}
	}
	compute = 1;
}
eoln
| assignment eoln
| eoln	/* blank line, do nothing */
| error eoln
/* if we got an error on the line, don't call the C program */
;

eoln : EOLN
{ ++lines; }
;

assignment : VAR WEQL expression
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
//			static char temp[20];
//			sprintf(temp,"%g", $3);
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
expression : expression_ltwo
| expression base_level_op expression_ltwo
{
	if (compute) {
		switch ($2) {
			case wor: $$ = $1 || $3; break;
			case wand: $$ = $1 && $3; break;
			default: $$ = 0;
		}
	} else {
		$$ = 0;
	}
}
;

expression_ltwo : expression_lthree
| WMINUS expression_lthree { $$ = - $2; }
| WFACT expression_lthree { $$ = ! $2; }
| expression_ltwo level_two_op expression_lthree
{
	if (compute) {
		switch ($2) {
			case wequal: $$ = ($1 == $3); break;
			case wnequal: $$ = ($1 != $3); break;
			case wgt: $$ = $1 > $3; break;
			case wlt: $$ = $1 < $3; break;
			case wgeq: $$ = $1 >= $3; break;
			case wleq: $$ = $1 <= $3; break;
			default: $$ = 0;
		}
	} else {
		$$ = 0;
	}
}
;

expression_lthree : expression_lfour
| expression_lthree level_three_op expression_lfour
{
	if (compute) {
					  	switch ($2) {
							case wplus: $$ = $1 + $3; break;
							case wminus: $$ = $1 - $3; break;
							default: $$ = $1;
						}
	} else {
		$$ = 0;
	}
}
;

expression_lfour : expression_lfive
| expression_lfour level_four_op expression_lfive
{
	if (compute) {
		switch ($2) {
			case wmult: $$ = $1 * $3; break;
			case wdiv:
				if ($3 != 0)
					$$ = $1 / $3;
				else
					$$ = HUGE_VAL;
				break;
			case wmod:
				/* first, are they both integers */
				if (ceil($1) == floor($1) && ceil($3) == floor($3))
					$$ = (int)$1 % (int)$3;
				else {
					report_error("Non-integer arguments to mod are illegal.");
					$$ = 0;
				}
					break;
			default: $$ = $1;
		}
	} else {
		$$ = 0;
	}
}
;

expression_lfive : value
| expression_lfive level_five_op value
{
	if (compute) {
		switch ($2) {
			case wexp:
				$$ = pow($1,$3);
				break;
			default: $$ = $1;
		}
	} else {
		$$ = 0;
	}
}
;

base_level_op : WAND { $$ = wand; }
| WOR { $$ = wor; }
;

level_two_op : WEQUAL { $$ = wequal; }
| WNEQUAL { $$ = wnequal; }
| WGT { $$ = wgt; }
| WLT { $$ = wlt; }
| WGEQ { $$ = wgeq; }
| WLEQ { $$ = wleq; }
;

level_three_op : WPLUS { $$ = wplus; }
| WMINUS { $$ = wminus; }
;

level_four_op : WMULT { $$ = wmult; }
| WDIV  { $$ = wdiv; }
| WMOD  { $$ = wmod; }
|       { $$ = wmult; }
;

level_five_op : WEXP { $$ = wexp; }
;

value : NUMBER
| VAR {
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
| null { $$ = 0; }
| PAR expression REN { $$ = $2; }
| WBRA expression WKET { $$ = $2; }
| WSBRA expression WSKET { $$ = $2; }
| func value
{
	  	if (compute) {
			switch ($1) {
				case wsin: $$ = sin($2); break;
				case wcos: $$ = cos($2); break;
				case wtan: $$ = tan($2); break;
				case wasin: $$ = asin($2); break;
				case wacos: $$ = acos($2); break;
				case watan: $$ = atan($2); break;
				case wsinh: $$ = sinh($2); break;
				case wcosh: $$ = cosh($2); break;
				case wtanh: $$ = tanh($2); break;
				case wasinh: $$ = asinh($2); break;
				case wacosh: $$ = acosh($2); break;
				case watanh: $$ = atanh($2); break;
				case wlog: $$ = log10($2); break;
				case wln: $$ = log($2); break;
				default: $$ = $2;
			}
		}
}
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
;

null : PAR REN
| WBRA WKET
| WSBRA WSKET
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
