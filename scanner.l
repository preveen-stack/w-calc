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
#include <errno.h>
#include "calculator.h"
#include "parser.h"

	extern short scanerror;
	
/* Everything up to the funny characters on the next line */
/* goes directly into the lex.yy.c file */
%}

/* shorthand definitions for later */
DIGIT		[0123456789]
NDIGIT		[123456789]
LETTER		[a-zA-Z]
WHITESPACE	[ \t]
ACCEPTABLE	[_:]

/* The rest of this after the '%%' is lex rules */
%%

{WHITESPACE}+	{ }  /* Just ignore white space */

"\n" 	        { return(EOLN); } 

%{ /* These are commands */
%}

\\h((ex)|(exadecimal))?		{ return(HEX_CMD); }
\\o((ct)|(ctal))?			{ return(OCT_CMD); }
\\d((ec)|(ecimal))?			{ return(DEC_CMD); }
\\b((in)|(inary))?			{ return(BIN_CMD); }

%{ /* These are the constants (except random) */
	%}
(e)				{ yylval.number = W_E; return(NUMBER); }
([pP][iI])|(¹)	{ yylval.number = W_PI; return(NUMBER); }
random			{ yylval.number = kbw_rand(); return(NUMBER); }
N[aA]			{ yylval.number = W_AVOGADROS_CONSTANT; return(NUMBER); }
k				{ yylval.number = W_BOLTZMANN_CONSTANT; return(NUMBER); }
Cc				{ yylval.number = W_COULOMB_CONSTANT; return(NUMBER); }
ec				{ yylval.number = W_ELEMENTARY_CHARGE; return(NUMBER); }
R				{ yylval.number = W_MOLAR_GAS_CONSTANT; return(NUMBER); }
G				{ yylval.number = W_GRAVITATIONAL_CONSTANT; return(NUMBER); }
g				{ yylval.number = W_GRAVITATIONAL_ACCELLERATION; return(NUMBER); }
Me				{ yylval.number = W_ELECTRON_MASS; return(NUMBER); }
Mp				{ yylval.number = W_PROTON_MASS; return(NUMBER); }
Mn				{ yylval.number = W_NEUTRON_MASS; return(NUMBER); }
Md				{ yylval.number = W_DEUTERON_MASS; return(NUMBER); }
(u)|(amu)		{ yylval.number = W_ATOMIC_MASS;  return(NUMBER); }
c				{ yylval.number = W_SPEED_OF_LIGHT; return(NUMBER); }
h				{ yylval.number = W_PLANCK_CONSTANT; return(NUMBER); }
(µ0)|(mu((0)|((zero)|(ZERO)))) { yylval.number = W_PERMEABILITY_OF_FREE_SPACE; return(NUMBER); }
((epsilon)|(EPSILON))((0)|((zero)|(ZERO))) { yylval.number = W_PERMITTIVITY_OF_FREE_SPACE; return(NUMBER); }
(µB)|(muB)		{ yylval.number = W_BOHR_MAGNETON; return(NUMBER); }
(µN)|(muN)		{ yylval.number = W_NUCLEAR_MAGNETON; return(NUMBER); }
b				{ yylval.number = W_WIEN_DISPLACEMENT; return(NUMBER); }
mW				{ yylval.number = W_W_BOSON_MASS; return(NUMBER); }
mZ				{ yylval.number = W_Z_BOSON_MASS; return(NUMBER); }
ao				{ yylval.number = W_BOHR_RADIUS; return(NUMBER); }
F				{ yylval.number = W_FARADAY_CONSTANT; return(NUMBER); }
(Vm)|(NAk)		{ yylval.number = W_MOLAR_VOLUME_OF_IDEAL_GAS; return(NUMBER); }
eV				{ yylval.number = W_ELECTRON_VOLT; return(NUMBER); }
(sigma)|(SIGMA) { yylval.number = W_STEFAN_BOLTZMANN; return(NUMBER); }
(alpha)|(ALPHA) { yylval.number = W_FINE_STRUCTURE; return(NUMBER); }
(gamma)|(GAMMA) { yylval.number = W_EULER; return(NUMBER); }
re				{ yylval.number = W_ELECTRON_RADIUS; return(NUMBER); }

%{ /* These are the grouping symbols */
	%}
[(] { return(PAR); }
[)] { return(REN); }
[{] { return(WBRA); }
[}] { return(WKET); }
\[  { return(WSBRA); }
\]  { return(WSKET); }
\|	{ return(WPIPE); }

%{ /* These are the binary operations */
	%}
([!])|(not)	{ return(WNOT); }
[+]			{ return(WPLUS); }
[*]			{ return(WMULT); }
[/]|[Ö]		{ return(WDIV); }
[%]			{ return(WMOD); }
[=]			{ return(WEQL); }
\^			{ return(WEXP); }
(\|\|)|(or)	{ return(WOR); }
(\&\&)|(and) { return(WAND); }
(\=\=)|(equals)|(eq) { return(WEQUAL); }
(\!\=)|(ne) { return(WNEQUAL); }
\>			{ return(WGT); }
\<			{ return(WLT); }
\>\=		{ return(WGEQ); }
\<\=		{ return(WLEQ); }

%{ /* This is a special operator/function */
	%}
\-  { return(WMINUS); }

%{ /* These are functions (unary operations) */
	%}
sin { return(WSIN); }
cos { return(WCOS); }
tan { return(WTAN); }
(asin)|(arcsin)|(sin^-1) { return(WASIN); }
(acos)|(arccos)|(cos^-1) { return(WACOS); }
(atan)|(arctan)|(tan^-1) { return(WATAN); }
sinh { return(WSINH); }
cosh { return(WCOSH); }
tanh { return(WTANH); }
(asinh)|(argsinh)|(sinh^-1) { return(WASINH); }
(acosh)|(argcosh)|(cosh^-1) { return(WACOSH); }
(atanh)|(argtanh)|(tanh^-1) { return(WATANH); }
log { return(WLOG); }
ln { return(WLN); }
round { return(WROUND); }
abs { return(WABS); }
(sqrt)|(Ã) { return(WSQRT); }
floor { return(WFLOOR); }
(ceil)|(ceiling) { return(WCEIL); }

{LETTER}+({LETTER}|{ACCEPTABLE})* {
	yylval.variable = strdup(yytext);
	return(VAR);
}

(({NDIGIT}{DIGIT}*(e[-+]?{DIGIT}+)?)|([.,]{DIGIT}+(e[-+]?{DIGIT}+)?)|((({NDIGIT}{DIGIT}*)|(0))[.,]{DIGIT}*(e[-+]?{DIGIT}+)?)) {
	if (strchr(yytext,',')) {
		*strchr(yytext,',') = '.';
	}
	yylval.number = strtod(yytext,NULL);
	if ((yylval.number != HUGE_VAL) && (yylval.number != -HUGE_VAL)) {
		return(NUMBER);
	} else {
		perror("parsing numbers");
		return(NUMBER);
	}
}

0x[0-9a-fA-F]+ {
	unsigned long int value;
	sscanf(yytext,"%lx",&value);
	yylval.number = value;
	return(NUMBER);
}

0[0-7]* {
	unsigned long int value;
	sscanf(yytext,"%lo",&value);
	yylval.number = value;
	return(NUMBER);
}

0b[01]+ {
	unsigned long int value = 0;
	unsigned int i, place = 1, len = strlen(yytext);
	for (i=len-1;i>1;--i) {
		value += ((yytext[i]=='0')?0:1) * place;
		place = pow(2,len-i);
	}
	yylval.number = value;
	return(NUMBER);
}

%{ /* if we haven't matched anything yet, then it's illegal */
	%}
. { char error[26];
	sprintf(error, "scanner: Bad character '%c'\n", *yytext);
	report_error(error);
	scanerror = 1;}

%%
