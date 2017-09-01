%{
/*
 * Original provided by
 * Shawn Ostermann -- Mon Sep 24, 2001
 *
 * Modified by Kyle Wheeler
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <strings.h> /* for strcasecmp() */
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include <math.h>
#include <errno.h>
#include <wctype.h> /* for iswdigit(), iswspace() */
#include "number.h"
#include "calculator.h"
#include "conf.h"
#include "definitions.h"
#include "conversion.h"
#ifdef HAVE_CONFIG_H
#include "parser.h"
#else
#include "y.tab.h"
#endif
#include "string_manip.h"
#include "explain.h"
#include "isfunc.h" /* for isfunc() */
#include "isconst.h" /* for isconst() */

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

int line_is_a_command = 0;
int column = 0;
int last_word = 0;

#define POS_CHAR() do { column ++; last_word = 1; } while(0)
#define POS_STR() do { column += yyleng; last_word = yyleng; } while (0)
#define STREQ(y) (!strcasecmp(str, y))
static int parse_yesno(const char *str)
{
    if (STREQ("yes") || STREQ("true") || STREQ("1")) {
        return 1;
    } else if (STREQ("no") || STREQ("false") || STREQ("0")) {
        return 0;
    } else {
        return -1;
    }
}

/* Everything up to the funny characters on the next line */
/* goes directly into the lex.yy.c file */
%}

%pointer
%option nounput noinput noyywrap
/* shorthand definitions for later */
DIGIT       [0123456789]
NDIGIT      [123456789]
LETTER      [a-zA-Z]
WHITESPACE  [ \t]
ACCEPTABLE  [_:0-9]
UNITCHRS    [-_a-zA-Z0-9^./µÅ]
SPACE       [ ]
YESNO       (YES|NO|TRUE|FALSE|1|0|yes|no|true|false)

/* The rest of this after the '%%' is lex rules */
%%

{WHITESPACE}+   { POS_STR(); }

"\n"            { return(EOLN); }

%{ /* These are commands */
%}

\\b(in(ary)?)?           { line_is_a_command = 1; POS_STR(); return(BIN_CMD); }
\\d(ec(imal)?)?          { line_is_a_command = 1; POS_STR(); return(DEC_CMD); }
\\assert{WHITESPACE}*.*  {
    int i = 7;
    while (iswspace(yytext[i])) i++;
    if (yytext[i] == 0) i--;
    YYLVALAC->variable = strdup(yytext + i);
    line_is_a_command = 1;
    POS_STR();
    return(ASSERT_CMD);
}
\\dsep{WHITESPACE}*.     {
    int i = 5;
    while (iswspace(yytext[i])) i++;
    if (yytext[i] == 0) i--;
    YYLVALAC->character = yytext[i];
    line_is_a_command = 1;
    POS_STR();
    return(DSEP_CMD);
}
\\idsep{WHITESPACE}*.     {
    int i = 5;
    while (iswspace(yytext[i])) i++;
    if (yytext[i] == 0) i--;
    YYLVALAC->character = yytext[i];
    line_is_a_command = 1;
    POS_STR();
    return(IDSEP_CMD);
}
\\e(ng(ineering)?)?{WHITESPACE}*{DIGIT}* {
    int i = 2;
    while (yytext[i] != 0 && ! iswdigit((int)(yytext[i]))) ++i;
    if (yytext[i] != 0 && yytext[i] != '\n') {
        YYLVALAC->integer = strtoul(yytext+i, NULL, 0);
    } else {
        YYLVALAC->integer = -1;
    }
    line_is_a_command = 1;
    POS_STR();
    return(ENG_CMD);
}
\\e(ng(ineering)?)?{WHITESPACE}+auto(matic)? {
    POS_STR();
    YYLVALAC->integer = 1;
    line_is_a_command = 1;
    return(ENG_CMD);
}
\\e(ng(ineering)?)?{WHITESPACE}+always {
    POS_STR();
    YYLVALAC->integer = 2;
    line_is_a_command = 1;
    return(ENG_CMD);
}
\\e(ng(ineering)?)?{WHITESPACE}+never {
    POS_STR();
    YYLVALAC->integer = 3;
    line_is_a_command = 1;
    return(ENG_CMD);
}
\\cons(ervative)?({WHITESPACE}+{YESNO})?       {
    int i = 0;
    line_is_a_command = 1;
    POS_STR();
    while(!iswspace(yytext[i])) i++;
    while(iswspace(yytext[i])) i++;
    YYLVALAC->integer = parse_yesno(yytext + i);
    return(GUARD_CMD);
}
\\h(ex(adecimal)?)?     { line_is_a_command = 1; POS_STR(); return(HEX_CMD); }
\\help           { line_is_a_command = 1; POS_STR(); return(PRINT_HELP_CMD); }
\\hlimit{WHITESPACE}*{DIGIT}+ {
    int i = 7;
    while (iswspace(yytext[i])) ++i;
    if (yytext[i] != 0) YYLVALAC->integer = strtoul(yytext+i, NULL, 0);
    else YYLVALAC->integer = 0;
    line_is_a_command = 1;
    POS_STR();
    return(HLIMIT_CMD);
}
\\ints?             { line_is_a_command = 1; POS_STR(); return(INT_CMD); }
\\del(im(iters)?)?  { line_is_a_command = 1; POS_STR(); return(DELIM_CMD); }
\\x                 { line_is_a_command = 1; POS_STR(); return(HEX_CMD); }
\\li(st(vars)?)?    { line_is_a_command = 1; POS_STR(); return(LISTVAR_CMD); }
\\o(ct(al)?)?       { line_is_a_command = 1; POS_STR(); return(OCT_CMD); }
\\open{WHITESPACE}+.*   {
    int i = 5;
    char *returnme;
    Dprintf("open\n");
    while (iswspace(yytext[i])) ++i;
    returnme = strdup(yytext + i);
    /* now trim trailing whitespace */
    i = (int)strlen(returnme) - 1;
    while (iswspace(returnme[i])) {
        returnme[i] = 0;
        i--;
    }
    YYLVALAC->variable = returnme;
    Dprintf("filename: %s\n",returnme);
    line_is_a_command = 1;
    POS_STR();
    return(OPEN_CMD);
}
\\p{WHITESPACE}*(({DIGIT}+)|(-1)) {
    int i = 2;
    while (iswspace(yytext[i])) ++i;
    YYLVALAC->integer = strtol(yytext+i, NULL, 0);
    line_is_a_command = 1;
    POS_STR();
    return(PRECISION_CMD);
}
\\pre(fix(es)?)?        { line_is_a_command = 1; POS_STR(); return(PREFIX_CMD); }
\\pref(s|erences)?      { line_is_a_command = 1; POS_STR(); return(DISPLAY_PREFS_CMD); }
\\r(ad(ians)?)?         { line_is_a_command = 1; POS_STR(); return(RADIAN_CMD); }
\\rou(nd(ing)?)?{WHITESPACE}+no(ne)? {
    YYLVALAC->integer = NO_ROUNDING_INDICATION;
    line_is_a_command = 1;
    POS_STR();
    return(ROUNDING_INDICATION_CMD);
}
\\rou(nd(ing)?)?{WHITESPACE}+simple {
    YYLVALAC->integer = SIMPLE_ROUNDING_INDICATION;
    line_is_a_command = 1;
    POS_STR();
    return(ROUNDING_INDICATION_CMD);
}
\\rou(nd(ing)?)?{WHITESPACE}+sig_fig {
    YYLVALAC->integer = SIG_FIG_ROUNDING_INDICATION;
    line_is_a_command = 1;
    POS_STR();
    return(ROUNDING_INDICATION_CMD);
}
\\rou(nd(ing)?)?        {
    YYLVALAC->integer = -1;
    line_is_a_command = 1;
    POS_STR();
    return(ROUNDING_INDICATION_CMD);
}
\\re(member(_errors)?)?     { line_is_a_command = 1; POS_STR(); return(REMEMBER_CMD); }
\\bits{WHITESPACE}*{DIGIT}+ {
    int i = 5;
    while (iswspace(yytext[i])) ++i;
    YYLVALAC->integer = strtoul(yytext+i, NULL, 0);
    line_is_a_command = 1;
    POS_STR();
    return(BITS_CMD);
}
\\save{WHITESPACE}.* {
    int i = 5;
    char *returnme;
    while (iswspace(yytext[i])) ++i;
    returnme = strdup(yytext + i);
    /* now trim trailing whitespace */
    i = (int)strlen(returnme) - 1;
    while (iswspace(returnme[i])) {
        returnme[i] = 0;
        i--;
    }
    YYLVALAC->variable = returnme;
    line_is_a_command = 1;
    POS_STR();
    return(SAVE_CMD);
}
\\tsep{WHITESPACE}*.       {
    int i = 5;
    while (iswspace(yytext[i])) ++i;
    if (yytext[i] == 0) i--;
    YYLVALAC->character = yytext[i];
    line_is_a_command = 1;
    POS_STR();
    return(TSEP_CMD);
}
\\itsep{WHITESPACE}*.       {
    int i = 5;
    while (iswspace(yytext[i])) ++i;
    if (yytext[i] == 0) i--;
    YYLVALAC->character = yytext[i];
    line_is_a_command = 1;
    POS_STR();
    return(ITSEP_CMD);
}
\?                      { line_is_a_command = 1; POS_STR(); return(PRINT_HELP_CMD); }
[Hh][Ee][Ll][Pp]        { line_is_a_command = 1; POS_STR(); return(PRINT_HELP_CMD); }
\\c(onv(ert)?)?{WHITESPACE}+{UNITCHRS}+({WHITESPACE}+to)?{WHITESPACE}+{UNITCHRS}+ {
    char *unitone;
    int i = 2;
    /* find the first space */
    while (yytext[i] != 0 && ! iswspace(yytext[i])) ++i;
    /* find the end of that space */
    while (iswspace(yytext[i])) ++i;
    unitone = yytext + i;
    /* skip the unit */
    while (yytext[i] != 0 && ! iswspace(yytext[i])) ++i;
    yytext[i++] = 0;
    /* onward to the word "to" */
    while (iswspace(yytext[i])) ++i;
    if (yytext[i] == 't' && yytext[i+1] == 'o' && iswspace(yytext[i+2])) {
        i += 3; /* to */
    }
    /* seek the next unit */
    while (iswspace(yytext[i])) ++i;
    YYLVALAC->conver.u1 = strdup(unitone);
    YYLVALAC->conver.u2 = strdup(yytext+i);
    line_is_a_command = 1;
    POS_STR();
    return(CONVERT_CMD);
}
\\base{WHITESPACE}*{DIGIT}+  {
    int i = 5;
    while (iswspace(yytext[i])) ++i;
    YYLVALAC->integer = strtoul(yytext + i, NULL, 0);
    line_is_a_command = 1;
    POS_STR();
    return(BASE_CMD);
}
\\verbose                   { line_is_a_command = 1; POS_STR(); return(VERBOSE_CMD); }
\\explain{WHITESPACE}*.*    {
    int i = 9, j;
    while (iswspace(yytext[i])) ++i;
    j = strlen(yytext+i);
    while (iswspace(yytext[j])) {
        yytext[j] = 0;
        j--;
    }
    explain(yytext+i);
    line_is_a_command = 1;
    POS_STR();
}
\\store    { line_is_a_command = 1; POS_STR(); return(STORE_CMD); }
\\cmod     { line_is_a_command = 1; POS_STR(); return(CMOD_CMD); }

%{ /* These are comments */
%}
\/\*.*\*\/              { POS_STR(); }
\/\/.*                  { POS_STR(); }
\#.*                    { POS_STR(); }


%{ /* These are the constants (except random) */
%}
(e) {
    num_init_set_str(YYLVALAC->number,W_E,0);
    POS_STR();
    return(NUMBER);
}
([pP][iI])|(\317\200) {
    num_init(YYLVALAC->number);
    num_const_pi(YYLVALAC->number);
    POS_STR();
    return(NUMBER);
}
random {
    num_init(YYLVALAC->number);
    while (num_random(YYLVALAC->number) != 0) ;
    num_mul_ui(YYLVALAC->number,YYLVALAC->number,UINT32_MAX);
    POS_STR();
    return(NUMBER);
}
irandom {
    num_init(YYLVALAC->number);
    while (num_random(YYLVALAC->number) != 0) ;
    num_mul_ui(YYLVALAC->number,YYLVALAC->number,UINT32_MAX);
    num_rint(YYLVALAC->number,YYLVALAC->number);
    POS_STR();
    return(NUMBER);
}
N[aA] {
    num_init_set_str(YYLVALAC->number,W_AVOGADROS_CONSTANT,0);
    POS_STR();
    return(NUMBER);
}
k {
    num_init_set_str(YYLVALAC->number,W_BOLTZMANN_CONSTANT,0);
    POS_STR();
    return(NUMBER);
}
Cc {
    num_init_set_str(YYLVALAC->number,W_COULOMB_CONSTANT,0);
    POS_STR();
    return(NUMBER);
}
ec {
    num_init_set_str(YYLVALAC->number,W_ELEMENTARY_CHARGE,0);
    POS_STR();
    return(NUMBER);
}
R {
    num_init_set_str(YYLVALAC->number,W_MOLAR_GAS_CONSTANT,0);
    POS_STR();
    return(NUMBER);
}
G {
    num_init_set_str(YYLVALAC->number,W_GRAVITATIONAL_CONSTANT,0);
    POS_STR();
    return(NUMBER);
}
g {
    num_init_set_str(YYLVALAC->number,W_GRAVITATIONAL_ACCELLERATION,0);
    POS_STR();
    return(NUMBER);
}
Me {
    num_init_set_str(YYLVALAC->number,W_ELECTRON_MASS,0);
    POS_STR();
    return(NUMBER);
}
Mp {
    num_init_set_str(YYLVALAC->number,W_PROTON_MASS,0);
    POS_STR();
    return(NUMBER);
}
Mn {
    num_init_set_str(YYLVALAC->number,W_NEUTRON_MASS,0);
    POS_STR();
    return(NUMBER);
}
Md {
    num_init_set_str(YYLVALAC->number,W_DEUTERON_MASS,0);
    POS_STR();
    return(NUMBER);
}
(u)|(amu) {
    num_init_set_str(YYLVALAC->number,W_ATOMIC_MASS,0);
    POS_STR();
    return(NUMBER);
}
c {
    num_init_set_str(YYLVALAC->number,W_SPEED_OF_LIGHT,0);
    POS_STR();
    return(NUMBER);
}
h {
    num_init_set_str(YYLVALAC->number,W_PLANCK_CONSTANT,0);
    POS_STR();
    return(NUMBER);
}
(\302\265|\316\274|mu)(0|zero|ZERO) {

    Number temp;
    num_init(YYLVALAC->number);
    num_init(temp);
    num_set_d(temp,1e-7);
    num_const_pi(YYLVALAC->number);
    num_mul_ui(YYLVALAC->number,YYLVALAC->number,4);
    num_mul(YYLVALAC->number,YYLVALAC->number,temp);
    num_free(temp);
    POS_STR();
    return(NUMBER);
}
(epsilon|EPSILON|\316\265)(0|zero|ZERO) {
    num_init_set_str(YYLVALAC->number,W_PERMITTIVITY_OF_FREE_SPACE,0);
    POS_STR();
    return(NUMBER);
}
(\302\265|\316\274|mu)B {
    num_init_set_str(YYLVALAC->number,W_BOHR_MAGNETON,0);
    POS_STR();
    return(NUMBER);
}
(\302\265|\316\274|mu)N {
    num_init_set_str(YYLVALAC->number,W_NUCLEAR_MAGNETON,0);
    POS_STR();
    return(NUMBER);
}
b {
    num_init_set_str(YYLVALAC->number,W_WIEN_DISPLACEMENT,0);
    POS_STR();
    return(NUMBER);
}
a0 {
    num_init_set_str(YYLVALAC->number,W_BOHR_RADIUS,0);
    POS_STR();
    return(NUMBER);
}
F {
    num_init_set_str(YYLVALAC->number,W_FARADAY_CONSTANT,0);
    POS_STR();
    return(NUMBER);
}
(Vm)|(NAk) {
    num_init_set_str(YYLVALAC->number,W_MOLAR_VOLUME_OF_IDEAL_GAS,0);
    POS_STR();
    return(NUMBER);
}
eV {
    num_init_set_str(YYLVALAC->number,W_ELECTRON_VOLT,0);
    POS_STR();
    return(NUMBER);
}
sigma|\317\203 {
    num_init_set_str(YYLVALAC->number,W_STEFAN_BOLTZMANN,0);
    POS_STR();
    return(NUMBER);
}
alpha|\316\261 {
    num_init_set_str(YYLVALAC->number,W_FINE_STRUCTURE,0);
    POS_STR();
    return(NUMBER);
}
gamma|GAMMA|\316\263 {
    num_init(YYLVALAC->number); num_const_euler(YYLVALAC->number);
    POS_STR();
    return(NUMBER);
}
re {
    num_init_set_str(YYLVALAC->number,W_ELECTRON_RADIUS,0);
    POS_STR();
    return(NUMBER);
}
Kj {
    num_init_set_str(YYLVALAC->number,W_JOSEPHSON_CONSTANT,0);
    POS_STR();
    return(NUMBER);
}
Rk {
    num_init_set_str(YYLVALAC->number,W_VON_KLITZING_CONSTANT,0);
    POS_STR();
    return(NUMBER);
}
R(inf|\342\210\236) {
    num_init_set_str(YYLVALAC->number,W_RYDBERG_CONSTANT,0);
    POS_STR();
    return(NUMBER);
}
Eh {
    num_init_set_str(YYLVALAC->number,W_HARTREE_ENERGY,0);
    POS_STR();
    return(NUMBER);
}
Gf {
    num_init_set_str(YYLVALAC->number,W_FERMI_COUPLING_CONSTANT,0);
    POS_STR();
    return(NUMBER);
}
M(\302\265|\316\274|mu) {
    num_init_set_str(YYLVALAC->number,W_MUON_MASS,0);
    POS_STR();
    return(NUMBER);
}
M(t|tau|\317\204) {
    num_init_set_str(YYLVALAC->number,W_TAU_MASS,0);
    POS_STR();
    return(NUMBER);
}
Mh {
    num_init_set_str(YYLVALAC->number,W_HELION_MASS,0);
    POS_STR();
    return(NUMBER);
}
M(alpha|\316\261) {
    num_init_set_str(YYLVALAC->number,W_ALPHA_PARTICLE_MASS,0);
    POS_STR();
    return(NUMBER);
}
n(0|zero|ZERO) {
    num_init_set_str(YYLVALAC->number,W_LOSCHMIDT_CONSTANT,0);
    POS_STR();
    return(NUMBER);
}
c1 {
    num_init_set_str(YYLVALAC->number,W_FIRST_RADIATION_CONSTANT,0);
    POS_STR();
    return(NUMBER);
}
c2 {
    num_init_set_str(YYLVALAC->number,W_SECOND_RADIATION_CONSTANT,0);
    POS_STR();
    return(NUMBER);
}
G(0|zero|ZERO) {
    num_init_set_str(YYLVALAC->number,W_CONDUCTANCE_QUANTUM,0);
    POS_STR();
    return(NUMBER);
}
Z(0|zero|ZERO) {
    num_init_set_str(YYLVALAC->number,W_IMPEDANCE_OF_VACUUM,0);
    POS_STR();
    return(NUMBER);
}
(Phi|\316\246)(0|zero|ZERO) {
    num_init_set_str(YYLVALAC->number,W_MAGNETIC_FLUX_QUANTUM,0);
    POS_STR();
    return(NUMBER);
}
\302\274 {
    num_init_set_d(YYLVALAC->number,0.25);
    POS_STR();
    return(NUMBER);
}
\302\275 {
    num_init_set_d(YYLVALAC->number,0.5);
    POS_STR();
    return(NUMBER);
}
\302\276 {
    num_init_set_d(YYLVALAC->number,0.75);
    POS_STR();
    return(NUMBER);
}
K {
    num_init(YYLVALAC->number);
    num_const_catalan(YYLVALAC->number);
    POS_STR();
    return(NUMBER);
}
@NaN@ {
    num_init(YYLVALAC->number);
    num_set_nan(YYLVALAC->number);
    POS_STR();
    return (NUMBER);
}
@Inf@ {
    num_init(YYLVALAC->number);
    num_set_inf(YYLVALAC->number,1);
    POS_STR();
    return (NUMBER);
}

%{ /* These are the grouping symbols */
    %}
[(] { POS_STR(); return(OPEN_PARENTHESES); }
[)] { POS_STR(); return(CLOSE_PARENTHESES); }
[{] { POS_STR(); return(OPEN_BRACE); }
[}] { POS_STR(); return(CLOSE_BRACE); }
\[  { POS_STR(); return(OPEN_BRACKET); }
\]  { POS_STR(); return(CLOSE_BRACKET); }

%{ /* These are the binary operations */
    %}
not                  { POS_STR(); return(WNOT); }
\302\254             { POS_CHAR(); return(WNOT); }
[!]                  { POS_CHAR(); return(WBANG); }
[*][*]               { POS_STR(); return(WPOW); }
\302\262             { POS_CHAR(); return(WSQR); }
[+]                  { POS_CHAR(); return(WPLUS); }
[*]|\303\227         { POS_CHAR(); return(WMULT); }
[/]|\303\267         { POS_CHAR(); return(WDIV); }
[%]                  { POS_CHAR(); return(WMOD); }
[=]                  { POS_CHAR(); return(EQUALS_SIGN); }
(\^)                 { POS_CHAR(); return(WPOW); }
\|                   { POS_CHAR(); return(WBOR); }
\&                   { POS_CHAR(); return(WBAND); }
\~                   { POS_CHAR(); return(WBNOT); }
(\|\|)|(or)          { POS_STR(); return(WOR); }
\342\210\250         { POS_CHAR(); return(WOR); }
(\&\&)|(and)         { POS_STR(); return(WAND); }
\342\210\247         { POS_CHAR(); return(WAND); }
(\=\=)|(equals)|(eq) { POS_STR(); return(WEQUAL); }
(\!\=)|(ne)          { POS_STR(); return(WNEQUAL); }
\342\211\240         { POS_CHAR(); return(WNEQUAL); }
xor                  { POS_STR(); return(WBXOR); }
\>                   { POS_CHAR(); return(WGT); }
\<                   { POS_CHAR(); return(WLT); }
\>\>                 { POS_STR(); return(WRSHFT); }
\<\<                 { POS_STR(); return(WLSHFT); }
\>\=                 { POS_STR(); return(WGEQ); }
\342\211\245         { POS_CHAR(); return(WGEQ); }
\<\=                 { POS_STR(); return(WLEQ); }
\342\211\244         { POS_CHAR(); return(WLEQ); }

%{ /* This is a special operator/function */
    %}
\-|\342\210\222  { POS_CHAR(); return(WMINUS); }

%{ /* These are functions (unary operations) */
    %}
sin(e)? { POS_STR(); return(WSIN); }
cos(in(e)?)? { POS_STR(); return(WCOS); }
tan { POS_STR(); return(WTAN); }
cot { POS_STR(); return(WCOT); }
sec(ant)? { POS_STR(); return(WSEC); }
csc|cosec(ant)? { POS_STR(); return(WCSC); }
(asin)|(arcsin)|(sin^-1) { POS_STR(); return(WASIN); }
(acos)|(arccos)|(cos^-1) { POS_STR(); return(WACOS); }
(atan)|(arctan)|(tan^-1) { POS_STR(); return(WATAN); }
(acot)|(arccot)|(cot^-1) { POS_STR(); return(WACOT); }
(asec)|(arcsec)|(sec^-1) { POS_STR(); return(WASEC); }
(acsc)|(arccsc)|(csc^-1) { POS_STR(); return(WACSC); }
sinh { POS_STR(); return(WSINH); }
cosh { POS_STR(); return(WCOSH); }
tanh { POS_STR(); return(WTANH); }
coth { POS_STR(); return(WCOTH); }
sech { POS_STR(); return(WSECH); }
csch { POS_STR(); return(WCSCH); }
asinh|arsinh|areasinh|(sinh^-1) { POS_STR(); return(WASINH); }
acosh|arcosh|areacosh|(cosh^-1) { POS_STR(); return(WACOSH); }
atanh|artanh|areatanh|(tanh^-1) { POS_STR(); return(WATANH); }
acoth|arcoth|areacoth|(tanh^-1) { POS_STR(); return(WACOTH); }
asech|areasech|(sech^-1) { POS_STR(); return(WASECH); }
acsch|areacsch|(csch^-1) { POS_STR(); return(WACSCH); }
sinc { POS_STR(); return(WSINC); }
log { POS_STR(); return(WLOG); }
logtwo { POS_STR(); return(WLOGTWO); }
ln { POS_STR(); return(WLN); }
round { POS_STR(); return(WROUND); }
abs { POS_STR(); return(WABS); }
(sqrt)|(\342\210\232) { POS_STR(); return(WSQRT); }
exp { POS_STR(); return(WEXP); }
floor { POS_STR(); return(WFLOOR); }
(ceil)|(ceiling) { POS_STR(); return(WCEIL); }
cbrt { POS_STR(); return(WCBRT); }
rand { POS_STR(); return(WRAND); }
irand { POS_STR(); return(WIRAND); }
fact { POS_STR(); return(WFACT); }
comp { POS_STR(); return(WCOMP); }
eint { POS_STR(); return(WEINT); }
Gamma { POS_STR(); return(WGAMMA); }
ln[gG]amma { POS_STR(); return(WLNGAMMA); }
zeta { POS_STR(); return(WZETA); }

'[^']*' {
    char * temp = strdup(yytext+1);
    temp[yyleng-2] = 0;
    YYLVALAC->variable = temp;
    POS_STR();
    return(STRING);
}

{LETTER}+({LETTER}|{ACCEPTABLE})*{WHITESPACE}*= {
    int i = 0;
    while (yytext[i] != 0 && !iswspace(yytext[i]) && yytext[i] != '=') i++;
    yytext[i] = 0;
    if (isfunc(yytext)) {
        report_error("'%s' is a function and functions cannot be reassigned.", yytext);
        scanerror = 1;
    } else if (isconst(yytext)) {
        report_error("'%s' is a pre-defined constant, which cannot be reassigned.", yytext);
        scanerror = 1;
    } else {
        YYLVALAC->variable = strdup(yytext);
    }
    POS_STR();
    if (! scanerror) {
        return(ASSIGNMENT);
    }
                                                }

{LETTER}+({LETTER}|{ACCEPTABLE})* {
    if (line_is_a_command) {
        YYLVALAC->variable = strdup(yytext);
        POS_STR();
        return(VARIABLE);
    } else {
        report_error("Undefined variable: %s", yytext);
        scanerror = 1;
        POS_STR();
    }
                                  }

%{
%}

({NDIGIT}{DIGIT}?{DIGIT}?\.{DIGIT}{DIGIT}{DIGIT})(e[+-]?{DIGIT}+)? {
    /* simple decimals */
    extern int yydebug;
    int retval;

    /* take out the ignored char */
    strstrip(',', yytext);

    if (yydebug)
        Dprintf("ambiguous %s\n", yytext);

    retval = num_init_set_str(YYLVALAC->number, yytext, DEFAULT_BASE);

    if (-1 == retval) {
        report_error("Invalid characters for base 10");
        scanerror = 1;
    } else {
        unsigned int t = count_digits(yytext);
        Dprintf("simple decimals digits in %s: %u (%u)\n",yytext,t,sig_figs);
        if (t<sig_figs) sig_figs = t;
    }

    POS_STR();
    return(NUMBER);
}

0?[.][0-9]*([eE][+-]?[0-9]+)? {
    /* zero-optional decimal */
    extern int yydebug;
    int retval;

    Dprintf("zero-optional decimal\n");
    if (yydebug) Dprintf("nonambiguous %s => ", yytext);
    retval = num_init_set_str(YYLVALAC->number, yytext, DEFAULT_BASE);
    if (-1 == retval) {
        report_error("Invalid characters for base 10");
        scanerror = 1;
    } else {
        unsigned int t = count_digits(yytext);
        Dprintf("zero optional decimal digits in %s: %u (%u)\n",yytext,t,sig_figs);
        if (t<sig_figs) sig_figs = t;
    }
    POS_STR();
    return(NUMBER);
}

[1-9](([0-9]?[0-9]?([,][0-9][0-9][0-9])+)|[0-9]*)([.][0-9]*)?([eE][+-]?[0-9]+)? {
    /* big ugly */
    extern int yydebug;
    int retval;

    Dprintf("big ugly (%s)\n",yytext);
    /* strip out ignored characters */
    strstrip(',',yytext);

    if (yydebug) Dprintf("complex one %s => ", yytext);
    retval = num_init_set_str(YYLVALAC->number, yytext, DEFAULT_BASE);

    if (-1 == retval) {
        report_error("Invalid characters for base 10");
        scanerror = 1;
    } else {
        unsigned int t = count_digits(yytext);
        char * period = strchr(yytext,'.');
        Dprintf("period: %s\n",period);
        if (period == NULL) { // no period means subtract the zeros
            period = yytext+strlen(yytext)-1;
            while (*period == '0') {
                t--;
                period--;
            }
            Dprintf("period: %s\n",period);
        }
        Dprintf("big ugly digits in %s: %u (%u)\n",yytext,t,sig_figs);
        if (t<sig_figs) sig_figs = t;
    }
    POS_STR();
    return(NUMBER);
}

0x[0-9a-fA-F]+([.][0-9a-fA-F]*)?([@][+-]?[0-9a-fA-F]+)? {
    /* hex */
    extern int yydebug;
    int retval;

    Dprintf("hex\n");
    if (yydebug) Dprintf("complex one %s => ", yytext);
    retval = num_init_set_str(YYLVALAC->number,yytext,16);

    if (-1 == retval) {
        report_error("some characters invalid for base 16");
        scanerror = 1;
    } else {
        unsigned int t = count_digits(yytext); //strlen(yytext+2);
        if (yydebug) {
            num_out_str(stdout,DEFAULT_BASE,YYLVALAC->number);
            printf("\n");
        }
        Dprintf("hex digits in %s: %u (%u)\n",yytext,t,sig_figs);
        if (t<sig_figs) sig_figs = t;
    }
    POS_STR();
    return(NUMBER);
}

0[0-9]{0,4}([0-9]*|(,[0-9]{4})*)([.][0-9]*)?([eE][+-]?[0-9]+)? {
    /* octal */
    //char err[strlen(yytext)+57];
    Dprintf("octal\n");
    if (strchr(yytext,'8') || strchr(yytext,'9')) {
        report_error("Incorrect number format (%s) - expected 0-7 for octal", yytext);
        scanerror = 1;
        num_init_set_ui(YYLVALAC->number,0);
        POS_STR();
    } else {
        /*unsigned long int value;*/
        unsigned int t;
        int retval;

        /* take out the ignored char */
        strstrip(',', yytext);

        retval = num_init_set_str(YYLVALAC->number,yytext,8);
        if (-1 == retval) {
            report_error(strerror(errno));
            scanerror = 1;
        }
        /*sscanf(yytext,"%lo",&value);
          yylval.number = value; */
        t = yyleng-1; //strlen(yytext+1);
        Dprintf("octal digits in %s: %u (%u)\n",yytext,t,sig_figs);
        if (t<sig_figs) sig_figs = t;
        POS_STR();
        return(NUMBER);
    }
}

0b[0-9]{1,8}([0-9]*|(,[0-9]{8})*)([.][0-9]*)?([eE][+-]?[0-9]+)? {
    /* binary */
    extern int yydebug;

    Dprintf("binary\n");
    /* verify the digits */
    if (strchr(yytext,'2') || strchr(yytext,'3') || strchr(yytext,'4') || strchr(yytext,'5') || strchr(yytext,'6') || strchr(yytext,'7') || strchr(yytext,'8') || strchr(yytext,'9')) {
        report_error("Incorrect number format (%s) - expected 0 or 1 for binary", yytext);
        scanerror = 1;
        num_init_set_ui(YYLVALAC->number,0);
    } else {
        int retval;

        /* take out the ignored char */
        strstrip(',', yytext);

        retval = num_init_set_str(YYLVALAC->number,yytext,2);
        if (-1 == retval) {
            report_error("Expected a 0 or 1 for binary.");
            scanerror = 1;
        } else {
            unsigned int t = count_digits(yytext); //strlen(yytext+2);
            if (yydebug) {
                num_out_str(stdout,DEFAULT_BASE,YYLVALAC->number);
                printf("\n");
            }
    Dprintf("binary digits in %s: %u (%u)\n",yytext,t,sig_figs);
            if (t<sig_figs) sig_figs = t;
        }
    }
    POS_STR();
    return(NUMBER);
}

(({DIGIT})|[,.])+ {
    /* This is the garbage-number collector */
    int i;
    const conf_t *conf = getConf();
    for (i=0;i<yyleng;++i) {
        if (yytext[i] == ',') {
            yytext[i] = conf->thou_delimiter;
        } else if (yytext[i] == '.') {
            yytext[i] = conf->dec_delimiter;
        }
    }
    report_error("Confusing number format (%s)", yytext);
    scanerror = 1;
    POS_STR();
}

%{ /* if we haven't matched anything yet, then it's illegal */
    %}
. {
    const conf_t *conf = getConf();
    if (*yytext == ',') *yytext = conf->thou_delimiter;
    else if (*yytext == '.') *yytext = conf->dec_delimiter;
    report_error("scanner(%i): cannot understand character", (unsigned char) (yytext[0]));
    scanerror = 1;
    POS_CHAR();
}

%%
