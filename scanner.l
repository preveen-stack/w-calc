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
#include <ctype.h> /* for isdigit() */
#include "calculator.h"
#include "parser.h"
#include "string_manip.h"

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

\\b(in(ary)?)?			{ return(BIN_CMD); }
\\d(ec(imal)?)?			{ return(DEC_CMD); }
\\dsep.                 { yylval.character = yytext[5]; return(DSEP_CMD); }
\\e(ng(ineering)?)?{DIGIT}*		{ char * foo = yytext;
	while (*foo && ! isdigit((int)(*foo))) ++foo;
	if (*foo != 0) yylval.number = atoi(foo);
	else yylval.number = -1;
	return(ENG_CMD);}
\\cons(ervative)?       { return(GUARD_CMD); }
\\h(ex(adecimal)?)?     { return(HEX_CMD); }
\\help                  { return(PRINT_HELP_CMD); }
\\hlimit{DIGIT}+        { yylval.number = atoi(yytext+7); return(HLIMIT_CMD); }
\\ints?                 { return(INT_CMD); }
\\x                     { return(HEX_CMD); }
\\l(enient)?            { return(PICKY_CMD); }
\\li(st(vars)?)?        { return(LISTVAR_CMD); }
\\o(ct(al)?)?           { return(OCT_CMD); }
\\open[ ].*             {
	char * curs = yytext+5, *returnme;
	while (curs && *curs && *curs == ' ')
		curs++;
	returnme = strdup(curs);
	curs = returnme+(int)strlen(returnme)-1;
	while (curs && *curs && *curs == ' ') {
		*curs = 0;
		curs--;
	}
	yylval.variable = returnme;
	return(OPEN_CMD); }
\\p[-]?{DIGIT}+         { yylval.number = atoi(yytext+2); return(PRECISION_CMD);}
\\p(icky)?              { return(PICKY_CMD); }
\\pre(fix(es)?)?        { return(PREFIX_CMD); }
\\pref(s|erences)?      { return(DISPLAY_PREFS_CMD); }
\\r(ad(ians)?)?         { return(RADIAN_CMD); }
\\rou(nd(ing)?)?{WHITESPACE}+no(ne)? { yylval.number = NO_ROUNDING_INDICATION;
	                      return(ROUNDING_INDICATION_CMD); }
\\rou(nd(ing)?)?{WHITESPACE}+simple { yylval.number = SIMPLE_ROUNDING_INDICATION;
	                      return(ROUNDING_INDICATION_CMD); }
\\rou(nd(ing)?)?{WHITESPACE}+sig_fig { yylval.number = SIG_FIG_ROUNDING_INDICATION;
	                      return(ROUNDING_INDICATION_CMD); }
\\rou(nd(int)?)?        { yylval.number = -1;
	                      return(ROUNDING_INDICATION_CMD); }
\\re(member(_errors)?)? { return(REMEMBER_CMD); }
\\save.*                { 
	char * curs = yytext+5, *returnme;
	while (curs && *curs && *curs == ' ')
		curs++;
	returnme = strdup(curs);
	curs = returnme+(int)strlen(returnme)-1;
	while (curs && *curs && *curs == ' ') {
		*curs = 0;
		curs--;
	}
	yylval.variable = returnme;
	return(SAVE_CMD); }
\\tsep.                 { yylval.character = yytext[5]; return(TSEP_CMD); }
\\c(onv(ert)?)?{WHITESPACE}+{LETTER}+{WHITESPACE}+{LETTER}+{WHITESPACE}+{LETTER}+ {}
\?                      { return(PRINT_HELP_CMD); }
[Hh][Ee][Ll][Pp]        { return(PRINT_HELP_CMD); }

%{ /* These are comments */
%}
\/\*.*\*\/              { }
\/\/.*                  { }
\#.*                    { }


%{ /* These are the constants (except random) */
	%}
(e)				{ yylval.number = W_E; return(NUMBER); }
([pP][iI])|(¹)	{ yylval.number = W_PI; return(NUMBER); }
random			{ yylval.number = kbw_rand(); return(NUMBER); }
irandom			{ yylval.number = (double) kbw_int_rand(); return(NUMBER); }
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
([!])|(not)				{ return(WNOT); }
[*][*]					{ return(WSQR); }
[+]						{ return(WPLUS); }
[*]						{ return(WMULT); }
[/]|[Ö]					{ return(WDIV); }
[%]						{ return(WMOD); }
[=]						{ return(WEQL); }
(\^)					{ return(WEXP); }
'|'                     { return(WBOR); }
\&                      { return(WBAND); }
\~                      { return(WBNOT); }
(\|\|)|(or)				{ return(WOR); }
(\&\&)|(and)			{ return(WAND); }
(\=\=)|(equals)|(eq)	{ return(WEQUAL); }
(\!\=)|(ne)|(­)			{ return(WNEQUAL); }
\>						{ return(WGT); }
\<						{ return(WLT); }
\>\>                    { return(WRSHFT); }
\<\<                    { return(WLSHFT); }
(\>\=)|(³)				{ return(WGEQ); }
(\<\=)|(²)				{ return(WLEQ); }

%{ /* This is a special operator/function */
	%}
\-  { return(WMINUS); }

%{ /* These are functions (unary operations) */
	%}
sin { return(WSIN); }
cos { return(WCOS); }
tan { return(WTAN); }
cot { return(WCOT); }
(asin)|(arcsin)|(sin^-1) { return(WASIN); }
(acos)|(arccos)|(cos^-1) { return(WACOS); }
(atan)|(arctan)|(tan^-1) { return(WATAN); }
(acot)|(arccot)|(cot^-1) { return(WACOT); }
sinh { return(WSINH); }
cosh { return(WCOSH); }
tanh { return(WTANH); }
coth { return(WCOTH); }
(asinh)|(arcsinh)|(sinh^-1) { return(WASINH); }
(acosh)|(arccosh)|(cosh^-1) { return(WACOSH); }
(atanh)|(arctanh)|(tanh^-1) { return(WATANH); }
(acoth)|(arccoth)|(tanh^-1) { return(WACOTH); }
log { return(WLOG); }
logtwo { return(WLOGTWO); }
ln { return(WLN); }
round { return(WROUND); }
abs { return(WABS); }
(sqrt)|(Ã) { return(WSQRT); }
floor { return(WFLOOR); }
(ceil)|(ceiling) { return(WCEIL); }
cbrt { return(WCBRT); }
rand { return(WRAND); }
irand { return(WIRAND); }

'.*' {
	char * temp = strdup(yytext+1);
	temp[(int)strlen(temp)-1] = 0;
	yylval.variable = temp;
	return(STRING);
}

{LETTER}+({LETTER}|{ACCEPTABLE})* {
	yylval.variable = strdup(yytext);
	return(VAR);
}

%{ /* international */
%}

({NDIGIT}{DIGIT}{0,2}\.{DIGIT}{3})(e[+-]?{DIGIT}+)? {
	/* simple decimals */
	extern int yydebug;

	/* take out the ignored char */
	strstrip(',', yytext);
	
	if (yydebug)
		printf("ambiguous %s\n", yytext);

	yylval.number = strtod(yytext,NULL);
	
	if (fabs(yylval.number) == HUGE_VAL)
		report_error(strerror(errno));
	else {
		unsigned int t = count_digits(yytext);
		if (t<sig_figs) sig_figs = t;
	}

	return(NUMBER);
}

(0?\.{DIGIT}+) {
	extern int yydebug;

	if (yydebug) printf("nonambiguous %s => ", yytext);
	yylval.number = strtod(yytext,NULL);
	if (yydebug) printf("%f\n", yylval.number);
	if (fabs(yylval.number) == HUGE_VAL)
		report_error(strerror(errno));
	else {
		unsigned int t = count_digits(yytext) - (yytext[0]=='0')?1:0;
		if (t<sig_figs) sig_figs = t;
	}
	return(NUMBER);
}

(({NDIGIT}(({DIGIT}*)|({DIGIT}{0,2}([,]{DIGIT}{3})+)))|({NDIGIT}(({DIGIT}*)|({DIGIT}{0,2}([,]{DIGIT}{3})+))[.]{DIGIT}*))(e[+-]?{DIGIT}+)? {
	const char strip = ',';
	extern int yydebug;

	/* strip out ignored characters */
	strstrip(strip,yytext);

	if (yydebug) printf("complex one %s => ", yytext);
	yylval.number = strtod(yytext,NULL);
	if (yydebug) printf("%f\n",yylval.number);

	if (fabs(yylval.number) == HUGE_VAL)
		report_error(strerror(errno));
	else {
		unsigned int t = count_digits(yytext);
		if (t<sig_figs) sig_figs = t;
	}
	return(NUMBER);
}

0x[0-9a-fA-F]+ {
	unsigned long int value;
	unsigned int t = 0;
	char * curs = yytext+2;
	
	sscanf(yytext,"%lx",&value);
	yylval.number = value;

	while (curs && *curs) {
		++t;
		++curs;
	}
	if (t<sig_figs) sig_figs = t;
	return(NUMBER);
}

0{DIGIT}* {
	unsigned long int value;
	char err[(int)strlen(yytext)+57];
	if (strchr(yytext,'8') || strchr(yytext,'9')) {
		sprintf(err,"Incorrect number format (%s) - expected 0-7 for octal",yytext);
		report_error(err);
		scanerror = 1;
	} else {
		unsigned int t;
		sscanf(yytext,"%lo",&value);
		yylval.number = value;
		t = count_digits(yytext)-1;
		if (t<sig_figs) sig_figs = t;
		return(NUMBER);
	}
}

0b[0-9]+ {
	unsigned long int value = 0;
	unsigned int i, place = 1, len = (unsigned int)strlen(yytext);
	char err[(int)strlen(yytext)+57];
	if (strchr(yytext,'2') || strchr(yytext,'3') || strchr(yytext,'4') || strchr(yytext,'5') || strchr(yytext,'6') || strchr(yytext,'7') || strchr(yytext,'8') || strchr(yytext,'9')) {
		sprintf(err,"Incorrect number format (%s) - expected 0 or 1 for binary",yytext);
		report_error(err);
		scanerror = 1;
	} else {
		unsigned int t;
		for (i=len-1;i>1;--i) {
			value += ((yytext[i]=='0')?0:1) * place;
			place = pow(2,len-i);
		}
		yylval.number = value;
		t = count_digits(yytext)-2;
		if (t<sig_figs) sig_figs = t;
		return(NUMBER);
	}
}

(({DIGIT})|[,.])* {
	/* This is the garbage-number collector */
	/* char errmsg[] = "Confusing number format (%s)\nDecimal Separator: '%c'\nThousands Separator: '%c'"; */
	char errmsg[] = "Confusing number format (%s)";
	char err[(int)strlen(yytext)+(int)strlen(errmsg)-5];
	int i;
	for (i=0;i<(int)strlen(yytext);++i) {
	    if (yytext[i] == ',')
		yytext[i] = conf.thou_delimiter;
	    else if (yytext[i] == '.')
		yytext[i] = conf.dec_delimiter;
	}	
	sprintf(err,errmsg,yytext,conf.dec_delimiter,conf.thou_delimiter);
	report_error(err);
	scanerror = 1;
}

%{ /* if we haven't matched anything yet, then it's illegal */
	%}
. { char error[26];
	if (*yytext == ',') *yytext = conf.thou_delimiter;
	else if (*yytext == '.') *yytext = conf.dec_delimiter;
	sprintf(error, "scanner: cannot parse next token (%c)\n", *yytext);
	report_error(error);
	scanerror = 1;}

%%
