%{
/*
 * Simple example lex input file
 * Original provided by
 * Shawn Ostermann -- Mon Sep 24, 2001
 *
 * Modified by Kyle Wheeler
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "calculator.h"
#include "parser.h"

	extern short scanerror;
	
/* Everything up to the funny characters on the next line */
/* goes directly into the lex.yy.c file */
%}

/* shorthand definitions for later */
DIGIT		[0123456789]
LETTER		[a-zA-Z]
WHITESPACE	[ \t]
ACCEPTABLE	[_:]

/* The rest of this after the '%%' is lex rules */
%%

{WHITESPACE}+	{ }  /* Just ignore white space */

"\n" 	        { return(EOLN); } 

%{ /* These are the constants */
	%}
e {
	yylval.number = WE;
	return(NUMBER);
}
pi {
	yylval.number = WPI;
	return(NUMBER);
}

%{ /* These are the grouping symbols */
	%}
[(] { return(PAR); }
[)] { return(REN); }
[{] { return(WBRA); }
[}] { return(WKET); }
\[  { return(WSBRA); }
\]  { return(WSKET); }

%{ /* These are the binary operations */
	%}
[!]  { return(WFACT); }
[+]  { return(WPLUS); }
[*]  { return(WMULT); }
[/]  { return(WDIV); }
[%]  { return(WMOD); }
[=]  { return(WEQL); }
\^   { return(WEXP); }
\|\| { return(WOR); }
\&\& { return(WAND); }
\=\= { return(WEQUAL); }
\!\= { return(WNEQUAL); }
\>   { return(WGT); }
\<   { return(WLT); }
\>\= { return(WGEQ); }
\<\= { return(WLEQ); }

%{ /* This is a special operator/function */
	%}
\-  { return(WMINUS); }

%{ /* These are functions (unary operations) */
	%}
sin { return(WSIN); }
cos { return(WCOS); }
tan { return(WTAN); }
asin { return(WASIN); }
acos { return(WACOS); }
atan { return(WATAN); }
sinh { return(WSINH); }
cosh { return(WCOSH); }
tanh { return(WTANH); }
asinh { return(WASINH); }
acosh { return(WACOSH); }
atanh { return(WATANH); }
log { return(WLOG); }
ln { return(WLN); }

{LETTER}+({LETTER}|{ACCEPTABLE})* {
	yylval.variable = strdup(yytext);
	return(VAR);
								  }

(({DIGIT}+)|({DIGIT}+e{DIGIT}+)|(\.{DIGIT}+)|(\.{DIGIT}+e-?{DIGIT}+)|({DIGIT}+\.{DIGIT}*)|({DIGIT}+\.{DIGIT}*e-?{DIGIT}+))\!?	{
	int fct = (yytext[strlen(yytext)-1]=='!')?1:0;
	if (fct) yytext[strlen(yytext)] = 0;

	yylval.number = strtod(yytext,NULL);
	if (fct)
		yylval.number = fact(yylval.number);
	if ((yylval.number != HUGE_VAL) && (yylval.number != -HUGE_VAL)) {
		return(NUMBER);
	} else {
		perror("parsing numbers");
		return(NUMBER);
	}
}



%{ /* if we haven't matched anything yet, then it's illegal */
	%}
. { char error[26];
	sprintf(error, "scanner: Bad character '%c'\n", *yytext);
	report_error(error);
	scanerror = 1;}

%%
