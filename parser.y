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
%token WNOT WLOG WLN WROUND
%token WNEG
%token WSIN WCOS WTAN WASIN WACOS WATAN WSINH WCOSH WTANH WASINH WACOSH WATANH
%token <number> NUMBER
%token <variable> VAR

%type <number> exp
%type <number> value
%type <operation> op postop
%type <function> func

%left WAND WOR
%left WEQUAL WNEQUAL WGT WLT WGEQ WLEQ
%left WMINUS WPLUS
%left WMULT WDIV WMOD
%left WEXP
%left WNEG

%expect 176

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

exp : exp op exp
{
	if (compute) {
        switch ($2) {
            case wor: $$ = $1 || $3; break;
            case wand: $$ = $1 && $3; break;
			case wequal: $$ = ($1 == $3); break;
			case wnequal: $$ = ($1 != $3); break;
			case wgt: $$ = $1 > $3; break;
			case wlt: $$ = $1 < $3; break;
			case wgeq: $$ = $1 >= $3; break;
			case wleq: $$ = $1 <= $3; break;
			case wplus: $$ = $1 + $3; break;
			case wminus: $$ = $1 - $3; break;
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
					$$ = fmod($1,$3);
				}
					break;
			case wexp:
				$$ = pow($1,$3);
				break;
				
            default: $$ = 0;
        }
    } else {
        $$ = 0;
    }
}
| WMINUS exp %prec WNEG
{ $$ = - $2; }
| WPLUS exp %prec WNEG
{ $$ = $2; }
| PAR exp REN { $$ = $2; }
| WBRA exp WKET { $$ = $2; }
| WSBRA exp WSKET { $$ = $2; }
| exp exp %prec WMULT
{ $$ = $1 * $2; }
| value postop %prec WNEG
{
	switch ($2) {
		case wfact: $$ = fact($1); break;
		default: $$ = $1;
	}
}
;

op : WAND { $$ = wand; }
| WOR { $$ = wor; }
| WEQUAL { $$ = wequal; }
| WNEQUAL { $$ = wnequal; }
| WGT { $$ = wgt; }
| WLT { $$ = wlt; }
| WGEQ { $$ = wgeq; }
| WLEQ { $$ = wleq; }
| WPLUS { $$ = wplus; }
| WMINUS { $$ = wminus; }
| WMULT { $$ = wmult; }
| WDIV  { $$ = wdiv; }
| WMOD  { $$ = wmod; }
| WEXP { $$ = wexp; }
;

postop : WNOT { $$ = wfact; }
| { $$ = wnone; }
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
| func exp
{
	  	if (compute) {
			switch ($1) {
				case wsin: $$ = sin(use_radians?$2:($2*WPI/180)); break;
				case wcos: $$ = cos(use_radians?$2:($2*WPI/180)); break;
				case wtan: $$ = tan(use_radians?$2:($2*WPI/180)); break;
				case wasin: $$ = asin(use_radians?$2:($2*WPI/180)); break;
				case wacos: $$ = acos(use_radians?$2:($2*WPI/180)); break;
				case watan: $$ = atan(use_radians?$2:($2*WPI/180)); break;
				case wsinh: $$ = sinh($2); break;
				case wcosh: $$ = cosh($2); break;
				case wtanh: $$ = tanh($2); break;
				case wasinh: $$ = asinh($2); break;
				case wacosh: $$ = acosh($2); break;
				case watanh: $$ = atanh($2); break;
				case wlog: $$ = log10($2); break;
				case wln: $$ = log($2); break;
				case wround: $$ = (fabs(floor($2)-$2)>=0.5)?ceil($2):floor($2); break;
				case wneg: $$ = - $2; break;
				case wnot: $$ = ! $2; break;
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
| WROUND { $$ = wround; }
| WNOT { $$ = wnot; }
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
