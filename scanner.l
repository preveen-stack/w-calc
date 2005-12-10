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
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include <math.h>
#include <errno.h>
#include <ctype.h> /* for isdigit() */
#include "calculator.h"
#include "conversion.h"
#include "parser.h"
#include "string_manip.h"
#include "explain.h"

	extern short scanerror;

#ifndef HUGE_VALF
# define HUGE_VALF HUGE_VAL
#endif
#ifndef UINT32_MAX
# define UINT32_MAX 4294967295U
#endif

#ifdef REENTRANT_PARSER
	/* Re-entrant */
#define YY_DECL int yylex (YYSTYPE *yylval)
#define YYLVALAC yylval
#else
#define YYLVALAC (&yylval)
#endif

/* Everything up to the funny characters on the next line */
/* goes directly into the lex.yy.c file */
%}

%pointer
%option nounput noinput
/* shorthand definitions for later */
DIGIT		[0123456789]
NDIGIT		[123456789]
LETTER		[a-zA-Z]
WHITESPACE	[ \t]
ACCEPTABLE	[_:0-9]
UNITCHRS	[-_a-zA-Z0-9^/µÅ]
SPACE		[ ]

/* The rest of this after the '%%' is lex rules */
%%

{WHITESPACE}+	{ /* Just ignore white space */ }

"\n" 	        { return(EOLN); }

%{ /* These are commands */
%}

\\b(in(ary)?)?			{ return(BIN_CMD); }
\\d(ec(imal)?)?			{ return(DEC_CMD); }
\\dsep.                 { YYLVALAC->character = yytext[5]; return(DSEP_CMD); }
\\e(ng(ineering)?)?{DIGIT}*		{ char * foo = yytext;
	while (*foo && ! isdigit((int)(*foo))) ++foo;
	if (*foo != 0) YYLVALAC->integer = atoi(foo);
	else YYLVALAC->integer = -1;
	return(ENG_CMD);}
\\cons(ervative)?       { return(GUARD_CMD); }
\\h(ex(adecimal)?)?     { return(HEX_CMD); }
\\help                  { return(PRINT_HELP_CMD); }
\\hlimit{DIGIT}+        { YYLVALAC->integer = atoi(yytext+7); return(HLIMIT_CMD); }
\\ints?                 { return(INT_CMD); }
\\x                     { return(HEX_CMD); }
\\li(st(vars)?)?        { return(LISTVAR_CMD); }
\\o(ct(al)?)?           { return(OCT_CMD); }
\\open[ ].*             {
	char * curs = yytext+5, *returnme;
	Dprintf("open\n");
	while (curs && *curs && *curs == ' ')
		curs++;
	returnme = strdup(curs);
	curs = returnme+(int)strlen(returnme)-1;
	while (curs && *curs && *curs == ' ') {
		*curs = 0;
		curs--;
	}
	YYLVALAC->variable = returnme;
	Dprintf("filename: %s\n",returnme);
	return(OPEN_CMD); }
\\p[-]?{DIGIT}+         { YYLVALAC->integer = atoi(yytext+2); return(PRECISION_CMD);}
\\pre(fix(es)?)?        { return(PREFIX_CMD); }
\\pref(s|erences)?      { return(DISPLAY_PREFS_CMD); }
\\r(ad(ians)?)?         { return(RADIAN_CMD); }
\\rou(nd(ing)?)?{WHITESPACE}+no(ne)? { YYLVALAC->integer = NO_ROUNDING_INDICATION;
	                      return(ROUNDING_INDICATION_CMD); }
\\rou(nd(ing)?)?{WHITESPACE}+simple { YYLVALAC->integer = SIMPLE_ROUNDING_INDICATION;
	                      return(ROUNDING_INDICATION_CMD); }
\\rou(nd(ing)?)?{WHITESPACE}+sig_fig { YYLVALAC->integer = SIG_FIG_ROUNDING_INDICATION;
	                      return(ROUNDING_INDICATION_CMD); }
\\rou(nd(ing)?)?        { YYLVALAC->integer = -1;
	                      return(ROUNDING_INDICATION_CMD); }
\\re(member(_errors)?)? { return(REMEMBER_CMD); }
\\bits{DIGIT}+			{ YYLVALAC->integer = atoi(yytext+5); return(BITS_CMD); }
\\save[ ].*                {
	char * curs = yytext+5, *returnme;
	while (curs && *curs && *curs == ' ')
		curs++;
	returnme = strdup(curs);
	curs = returnme+(int)strlen(returnme)-1;
	while (curs && *curs && *curs == ' ') {
		*curs = 0;
		curs--;
	}
	YYLVALAC->variable = returnme;
	return(SAVE_CMD); }
\\tsep.                 { YYLVALAC->character = yytext[5]; return(TSEP_CMD); }
\?                      { return(PRINT_HELP_CMD); }
[Hh][Ee][Ll][Pp]        { return(PRINT_HELP_CMD); }
\\c(onv(ert)?)?{SPACE}+{UNITCHRS}+{SPACE}+to{SPACE}+{UNITCHRS}+ {
	char *unitone, *unittwo;
	unitone = strchr(yytext,' ')+1;
	unittwo = strchr(unitone,' ');
	while (*unittwo == ' ') { *unittwo++ = 0; /* space */ }
	*unittwo++ = 0; /* t */
	*unittwo++ = 0; /* o */
	while (*unittwo == ' ') { *unittwo++ = 0; /* space */ }
	YYLVALAC->conver.u1 = strdup(unitone);
	YYLVALAC->conver.u2 = strdup(unittwo);
	return(CONVERT_CMD);
}
\\c(onv(ert)?)?{SPACE}+{UNITCHRS}+{SPACE}+{UNITCHRS}+ {
	char *unitone, *unittwo;
	unitone = strchr(yytext,' ')+1;
	unittwo = strchr(unitone,' ');
	while (*unittwo == ' ') { *unittwo++ = 0; /* space */ }
	YYLVALAC->conver.u1 = strdup(unitone);
	YYLVALAC->conver.u2 = strdup(unittwo);
	return(CONVERT_CMD);
}
\\base{DIGIT}+          { YYLVALAC->integer = atoi(yytext+5); return(BASE_CMD); }
\\verbose               { return(VERBOSE_CMD); }
\\explain{SPACE}.* 		{ explain(yytext+9); }

%{ /* These are comments */
%}
\/\*.*\*\/              { }
\/\/.*                  { }
\#.*                    { }


%{ /* These are the constants (except random) */
	%}
(e)				{ mpfr_init_set_str(YYLVALAC->number,W_E,0,GMP_RNDN); return(NUMBER); }
([pP][iI])|(\317\200)	{ mpfr_init(YYLVALAC->number); mpfr_const_pi(YYLVALAC->number,GMP_RNDN);
				  return(NUMBER); }
random			{ seed_random();
				  mpfr_init(YYLVALAC->number);
				  while (mpfr_urandomb(YYLVALAC->number,randstate) != 0) ;
				  mpfr_mul_ui(YYLVALAC->number,YYLVALAC->number,UINT32_MAX,GMP_RNDN);
				  return(NUMBER); }
irandom			{ seed_random();
				  mpfr_init(YYLVALAC->number);
				  while (mpfr_urandomb(YYLVALAC->number,randstate) != 0) ;
				  mpfr_mul_ui(YYLVALAC->number,YYLVALAC->number,UINT32_MAX,GMP_RNDN);
				  mpfr_rint(YYLVALAC->number,YYLVALAC->number,GMP_RNDN);
				  return(NUMBER); }
N[aA]			{ mpfr_init_set_str(YYLVALAC->number,W_AVOGADROS_CONSTANT,0,GMP_RNDN);
				  return(NUMBER); }
k				{ mpfr_init_set_str(YYLVALAC->number,W_BOLTZMANN_CONSTANT,0,GMP_RNDN);
				  return(NUMBER); }
Cc				{ mpfr_init_set_str(YYLVALAC->number,W_COULOMB_CONSTANT,0,GMP_RNDN);
				  return(NUMBER); }
ec				{ mpfr_init_set_str(YYLVALAC->number,W_ELEMENTARY_CHARGE,0,GMP_RNDN);
				  return(NUMBER); }
R				{ mpfr_init_set_str(YYLVALAC->number,W_MOLAR_GAS_CONSTANT,0,GMP_RNDN);
				  return(NUMBER); }
G				{ mpfr_init_set_str(YYLVALAC->number,W_GRAVITATIONAL_CONSTANT,0,GMP_RNDN);
				  return(NUMBER); }
g				{ mpfr_init_set_str(YYLVALAC->number,W_GRAVITATIONAL_ACCELLERATION,0,GMP_RNDN);
				  return(NUMBER); }
Me				{ mpfr_init_set_str(YYLVALAC->number,W_ELECTRON_MASS,0,GMP_RNDN);
				  return(NUMBER); }
Mp				{ mpfr_init_set_str(YYLVALAC->number,W_PROTON_MASS,0,GMP_RNDN);
				  return(NUMBER); }
Mn				{ mpfr_init_set_str(YYLVALAC->number,W_NEUTRON_MASS,0,GMP_RNDN);
				  return(NUMBER); }
Md				{ mpfr_init_set_str(YYLVALAC->number,W_DEUTERON_MASS,0,GMP_RNDN);
				  return(NUMBER); }
(u)|(amu)		{ mpfr_init_set_str(YYLVALAC->number,W_ATOMIC_MASS,0,GMP_RNDN);
				  return(NUMBER); }
c				{ mpfr_init_set_str(YYLVALAC->number,W_SPEED_OF_LIGHT,0,GMP_RNDN);
				  return(NUMBER); }
h				{ mpfr_init_set_str(YYLVALAC->number,W_PLANCK_CONSTANT,0,GMP_RNDN);
				  return(NUMBER); }
(\302\265|\316\274|mu)(0|zero|ZERO) {
				  mpfr_t temp;
				  mpfr_init(YYLVALAC->number);
				  mpfr_init(temp);
				  mpfr_set_d(temp,1e-7,GMP_RNDN);
				  mpfr_const_pi(YYLVALAC->number,GMP_RNDN);
				  mpfr_mul_ui(YYLVALAC->number,YYLVALAC->number,4,GMP_RNDN);
				  mpfr_mul(YYLVALAC->number,YYLVALAC->number,temp,GMP_RNDN);
				  mpfr_clear(temp);
				  return(NUMBER); }
(epsilon|EPSILON|\316\265)(0|zero|ZERO) { mpfr_init_set_str(YYLVALAC->number,W_PERMITTIVITY_OF_FREE_SPACE,0,GMP_RNDN);
				  return(NUMBER); }
(\302\265|\316\274|mu)B	{ mpfr_init_set_str(YYLVALAC->number,W_BOHR_MAGNETON,0,GMP_RNDN);
				  return(NUMBER); }
(\302\265|\316\274|mu)N	{ mpfr_init_set_str(YYLVALAC->number,W_NUCLEAR_MAGNETON,0,GMP_RNDN);
				  return(NUMBER); }
b				{ mpfr_init_set_str(YYLVALAC->number,W_WIEN_DISPLACEMENT,0,GMP_RNDN);
				  return(NUMBER); }
ao				{ mpfr_init_set_str(YYLVALAC->number,W_BOHR_RADIUS,0,GMP_RNDN);
				  return(NUMBER); }
F				{ mpfr_init_set_str(YYLVALAC->number,W_FARADAY_CONSTANT,0,GMP_RNDN);
				  return(NUMBER); }
(Vm)|(NAk)		{ mpfr_init_set_str(YYLVALAC->number,W_MOLAR_VOLUME_OF_IDEAL_GAS,0,GMP_RNDN);
				  return(NUMBER); }
eV				{ mpfr_init_set_str(YYLVALAC->number,W_ELECTRON_VOLT,0,GMP_RNDN);
				  return(NUMBER); }
sigma|\317\203 { mpfr_init_set_str(YYLVALAC->number,W_STEFAN_BOLTZMANN,0,GMP_RNDN);
				  return(NUMBER); }
alpha|\316\261 { mpfr_init_set_str(YYLVALAC->number,W_FINE_STRUCTURE,0,GMP_RNDN);
				  return(NUMBER); }
gamma|GAMMA|\316\263 { mpfr_init_set_str(YYLVALAC->number,W_EULER,0,GMP_RNDN);
				  return(NUMBER); }
re				{ mpfr_init_set_str(YYLVALAC->number,W_ELECTRON_RADIUS,0,GMP_RNDN);
				  return(NUMBER); }
Kj				{ mpfr_init_set_str(YYLVALAC->number,W_JOSEPHSON_CONSTANT,0,GMP_RNDN);
				  return(NUMBER); }
Rk				{ mpfr_init_set_str(YYLVALAC->number,W_VON_KLITZING_CONSTANT,0,GMP_RNDN);
				  return(NUMBER); }
R(inf|\342\210\236)			{ mpfr_init_set_str(YYLVALAC->number,W_RYDBERG_CONSTANT,0,GMP_RNDN);
				  return(NUMBER); }
Eh				{ mpfr_init_set_str(YYLVALAC->number,W_HARTREE_ENERGY,0,GMP_RNDN);
				  return(NUMBER); }
Gf				{ mpfr_init_set_str(YYLVALAC->number,W_FERMI_COUPLING_CONSTANT,0,GMP_RNDN);
				  return(NUMBER); }
M(\302\265|\316\274|mu)	{ mpfr_init_set_str(YYLVALAC->number,W_MUON_MASS,0,GMP_RNDN);
				  return(NUMBER); }
M(t|tau|\317\204)				{ mpfr_init_set_str(YYLVALAC->number,W_TAU_MASS,0,GMP_RNDN);
				  return(NUMBER); }
Mh				{ mpfr_init_set_str(YYLVALAC->number,W_HELION_MASS,0,GMP_RNDN);
				  return(NUMBER); }
M(alpha|\316\261)	{ mpfr_init_set_str(YYLVALAC->number,W_ALPHA_PARTICLE_MASS,0,GMP_RNDN);
				  return(NUMBER); }
n(0|zero|ZERO)	{ mpfr_init_set_str(YYLVALAC->number,W_LOSCHMIDT_CONSTANT,0,GMP_RNDN);
				  return(NUMBER); }
c1				{ mpfr_init_set_str(YYLVALAC->number,W_FIRST_RADIATION_CONSTANT,0,GMP_RNDN);
				  return(NUMBER); }
c2				{ mpfr_init_set_str(YYLVALAC->number,W_SECOND_RADIATION_CONSTANT,0,GMP_RNDN);
				  return(NUMBER); }
G(0|zero|ZERO)	{ mpfr_init_set_str(YYLVALAC->number,W_CONDUCTANCE_QUANTUM,0,GMP_RNDN);
				  return(NUMBER); }
Z(0|zero|ZERO)	{ mpfr_init_set_str(YYLVALAC->number,W_IMPEDANCE_OF_VACUUM,0,GMP_RNDN);
				  return(NUMBER); }
(Phi|\316\246)(0|zero|ZERO)	{ mpfr_init_set_str(YYLVALAC->number,W_MAGNETIC_FLUX_QUANTUM,0,GMP_RNDN);
				  return(NUMBER); }
\302\274		{ mpfr_init_set_d(YYLVALAC->number,0.25,GMP_RNDN);
				  return(NUMBER); }
\302\275		{ mpfr_init_set_d(YYLVALAC->number,0.5,GMP_RNDN);
				  return(NUMBER); }
\302\276		{ mpfr_init_set_d(YYLVALAC->number,0.75,GMP_RNDN);
				  return(NUMBER); }

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
not|\302\254			{ return(WNOT); }
[!]						{ return(WBANG); }
[*][*]|\302\262			{ return(WSQR); }
[+]						{ return(WPLUS); }
[*]|\303\227			{ return(WMULT); }
[/]|\303\267			{ return(WDIV); }
[%]						{ return(WMOD); }
[=]						{ return(WEQL); }
(\^)					{ return(WPOW); }
\|                      { return(WBOR); }
\&                      { return(WBAND); }
\~                      { return(WBNOT); }
(\|\|)|(or)|\342\210\250	{ return(WOR); }
(\&\&)|(and)|\342\210\247	{ return(WAND); }
(\=\=)|(equals)|(eq)	{ return(WEQUAL); }
(\!\=)|(ne)|(\342\211\240)	{ return(WNEQUAL); }
xor						{ return(WBXOR); }
\>						{ return(WGT); }
\<						{ return(WLT); }
\>\>                    { return(WRSHFT); }
\<\<                    { return(WLSHFT); }
(\>\=)|(\342\211\245)				{ return(WGEQ); }
(\<\=)|(\342\211\244)				{ return(WLEQ); }

%{ /* This is a special operator/function */
	%}
\-|\342\210\222  { return(WMINUS); }

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
asinh|arsinh|areasinh|(sinh^-1) { return(WASINH); }
acosh|arcosh|areacosh|(cosh^-1) { return(WACOSH); }
atanh|artanh|areatanh|(tanh^-1) { return(WATANH); }
acoth|arcoth|areacoth|(tanh^-1) { return(WACOTH); }
log { return(WLOG); }
logtwo { return(WLOGTWO); }
ln { return(WLN); }
round { return(WROUND); }
abs { return(WABS); }
(sqrt)|(\342\210\232) { return(WSQRT); }
exp { return(WEXP); }
floor { return(WFLOOR); }
(ceil)|(ceiling) { return(WCEIL); }
cbrt { return(WCBRT); }
rand { return(WRAND); }
irand { return(WIRAND); }
fact { return(WFACT); }
comp { return(WCOMP); }

'.*' {
	char * temp = strdup(yytext+1);
	//temp[(int)strlen(temp)-1] = 0;
	temp[yyleng-2] = 0;
	YYLVALAC->variable = temp;
	return(STRING);
}

{LETTER}+({LETTER}|{ACCEPTABLE})* {
	YYLVALAC->variable = strdup(yytext);
	return(VAR);
}

%{ /* international */
%}

({NDIGIT}{DIGIT}{0,2}\.{DIGIT}{3})(e[+-]?{DIGIT}+)? {
	/* simple decimals */
	extern int yydebug;
	int retval;

	/* take out the ignored char */
	strstrip(',', yytext);

	if (yydebug)
		printf("ambiguous %s\n", yytext);

	retval = mpfr_init_set_str(YYLVALAC->number, yytext, DEFAULT_BASE, GMP_RNDN);

	if (-1 == retval) {
		report_error("Invalid characters for base 10");
	} else {
		unsigned int t = count_digits(yytext);
		if (t<sig_figs) sig_figs = t;
	}

	return(NUMBER);
}

(0?\.{DIGIT}*) {
	/* zero-optional decimal */
	extern int yydebug;
	int retval;

	if (yydebug) printf("nonambiguous %s => ", yytext);
	retval = mpfr_init_set_str(YYLVALAC->number, yytext, DEFAULT_BASE, GMP_RNDN);
	if (-1 == retval) {
		report_error("Invalid characters for base 10");
	} else {
		unsigned int t = count_digits(yytext) - ((yytext[0]=='0')?1:0);
		if (t<sig_figs) sig_figs = t;
	}
	return(NUMBER);
}

(({NDIGIT}(({DIGIT}*)|({DIGIT}{0,2}([,]{DIGIT}{3})+)))|({NDIGIT}(({DIGIT}*)|({DIGIT}{0,2}([,]{DIGIT}{3})+))[.]{DIGIT}*))(e[+-]?{DIGIT}+)? {
	/* big ugly */
	extern int yydebug;
	int retval;

	/* strip out ignored characters */
	strstrip(',',yytext);

	if (yydebug) printf("complex one %s => ", yytext);
	retval = mpfr_init_set_str(YYLVALAC->number, yytext, DEFAULT_BASE, GMP_RNDN);

	if (-1 == retval) {
		report_error("Invalid characters for base 10");
	} else {
		unsigned int t = count_digits(yytext);
		if (t<sig_figs) sig_figs = t;
	}
	return(NUMBER);
}

0x[0-9a-fA-F]+ {
	/* hex */
	extern int yydebug;
	int retval;

	if (yydebug) printf("complex one %s => ", yytext);
	retval = mpfr_init_set_str(YYLVALAC->number,yytext,16,GMP_RNDN);

	if (-1 == retval) {
		report_error("some characters invalid for base 16");
	} else {
		unsigned int t = yyleng-2; //strlen(yytext+2);
		if (yydebug) {
			mpfr_out_str(stdout,DEFAULT_BASE,0, YYLVALAC->number, GMP_RNDN);
			printf("\n");
		}
		if (t<sig_figs) sig_figs = t;
	}
	return(NUMBER);
}

0{DIGIT}* {
	/* octal */
	//char err[strlen(yytext)+57];
	char err[yyleng+57];
	char *errpat = "Incorrect number format (%s) - expected 0-7 for octal";
	if (strchr(yytext,'8') || strchr(yytext,'9')) {
		snprintf(err,yyleng+57,errpat,yytext);
		report_error(err);
		scanerror = 1;
	} else {
		/*unsigned long int value;*/
		unsigned int t;
		int retval;
		retval = mpfr_init_set_str(YYLVALAC->number,yytext,8,GMP_RNDN);
		if (-1 == retval) report_error(strerror(errno));
		/*sscanf(yytext,"%lo",&value);
		yylval.number = value; */
		t = yyleng-1; //strlen(yytext+1);
		if (t<sig_figs) sig_figs = t;
		return(NUMBER);
	}
}

0b[0-9]+ {
	/* binary */
	extern int yydebug;
	//char err[strlen(yytext)+57];
	char err[yyleng+57];

	/* verify the digits */
	if (strchr(yytext,'2') || strchr(yytext,'3') || strchr(yytext,'4') || strchr(yytext,'5') || strchr(yytext,'6') || strchr(yytext,'7') || strchr(yytext,'8') || strchr(yytext,'9')) {
		snprintf(err,yyleng+57,"Incorrect number format (%s) - expected 0 or 1 for binary",yytext);
		report_error(err);
		scanerror = 1;
	} else {
		int retval;

		retval = mpfr_init_set_str(YYLVALAC->number,yytext,2,GMP_RNDN);
		if (-1 == retval) {
			report_error("Expected a 0 or 1 for binary.");
		} else {
			unsigned int t = yyleng-2; //strlen(yytext+2);
			if (yydebug) {
				mpfr_out_str(stdout,DEFAULT_BASE,0, YYLVALAC->number, GMP_RNDN);
				printf("\n");
			}
			if (t<sig_figs) sig_figs = t;
		}
	}
	return(NUMBER);
}

(({DIGIT})|[,.])* {
	/* This is the garbage-number collector */
	/* char errmsg[] = "Confusing number format (%s)\nDecimal Separator: '%c'\nThousands Separator: '%c'"; */
	char errmsg[] = "Confusing number format (%s)";
	char *err;
	int i;
	unsigned int len = yyleng + (int)strlen(errmsg) - 5;
	err = (char *)calloc(sizeof(char),len);
	for (i=0;i<yyleng;++i) {
	    if (yytext[i] == ',')
		yytext[i] = conf.thou_delimiter;
	    else if (yytext[i] == '.')
		yytext[i] = conf.dec_delimiter;
	}
	snprintf(err,len,errmsg,yytext,conf.dec_delimiter,conf.thou_delimiter);
	report_error(err);
	scanerror = 1;
	free(err);
}

%{ /* if we haven't matched anything yet, then it's illegal */
	%}
. { char error[50];
	if (*yytext == ',') *yytext = conf.thou_delimiter;
	else if (*yytext == '.') *yytext = conf.dec_delimiter;
	snprintf(error, 50, "scanner(%i): cannot understand character\n", (unsigned char) (*yytext));
	report_error(error);
	scanerror = 1;}

%%
