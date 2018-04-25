#include "simpleCalc.h"
#include "calculator.h"
#include "number.h"
#include "string_manip.h"
#include <ctype.h>		       // for isdigit
#include <string.h>		       // for strcmp/strlen/stpcpy/strdup
#include <stdlib.h>		       // for calloc
#include <stdio.h>
#include "includes/conf.h"

static char append = 1;
static Number cur_number;
static Number prev_number;
static unsigned char operationPending = 0;

void simpleCalcInit();

void simpleCalcEval(const unsigned char op)
{
    Dprintf("op: %c\n", op);
    switch (op) {
	case '+':
	    simple_exp(prev_number, prev_number, wplus, cur_number);
	    break;
	case '-':
	    simple_exp(prev_number, prev_number, wminus, cur_number);
	    break;
	case '*':
	    simple_exp(prev_number, prev_number, wmult, cur_number);
	    break;
	case '/':
	    simple_exp(prev_number, prev_number, wdiv, cur_number);
	    break;
	default:
	    num_set(prev_number, cur_number);
	    break;
    }
}

char *simpleCalc(const unsigned char input, const char *expStr)
{
    unsigned int expStrLen = strlen(expStr);
    simpleCalcInit();
    Dprintf("simpleCalc: %c, %s\n", input, expStr);
    Dprintf(" ~ cur: %RNf, prev: %RNf\n", cur_number, prev_number);
    switch (input) {
	case '+': case '-': case '*': case '/':
	    {
		char * expdup = strdup(expStr);
		// Americanize the numbers
		if (conf.in_thou_delimiter != 0) {
		    strstrip(conf.in_thou_delimiter, expdup);
		} else {
		    strstrip(conf.thou_delimiter, expdup);
		}
		if (conf.in_dec_delimiter != 0 && conf.in_dec_delimiter != '.') {
		    strswap(conf.in_dec_delimiter, '.', expdup);
		} else if (conf.dec_delimiter != '.') {
		    strswap(conf.dec_delimiter, '.', expdup);
		}
		// ... and now, depending on the state...
		if (operationPending) {
		    num_set_str(cur_number, expdup, 10);
		    free(expdup);

		    simpleCalcEval(operationPending);
		    operationPending = input;
		    append = 0;
		    set_prettyanswer(prev_number);
		    return strdup(pretty_answer);
		} else {
		    num_set_str(prev_number, expdup, 10);
		    free(expdup);

		    operationPending = input;
		    append = 0;
		    return strdup(expStr);
		}
	    }
	    break;
	case '=':
	    append = 0;
	    {
		char * expdup = strdup(expStr);
		// Americanize the numbers
		if (conf.in_thou_delimiter != 0) {
		    strstrip(conf.in_thou_delimiter, expdup);
		} else {
		    strstrip(conf.thou_delimiter, expdup);
		}
		if (conf.in_dec_delimiter != 0 && conf.in_dec_delimiter != '.') {
		    strswap(conf.in_dec_delimiter, '.', expdup);
		} else if (conf.dec_delimiter != '.') {
		    strswap(conf.dec_delimiter, '.', expdup);
		}
		// ... and now, depending on the state...
		if (! operationPending) {
		    num_set_str(prev_number, expdup, 10);
		    free(expdup);
		} else {
		    num_set_str(cur_number, expdup, 10);
		    free(expdup);
		    simpleCalcEval(operationPending);
		    operationPending = 0;
		}
	    }
	    set_prettyanswer(prev_number);
	    return NULL; // This should cause it to go through the displayAnswer logic
	    break;
	default:
	    if (input == conf.dec_delimiter || isdigit(input)) {
		char *newStr;
		if (append && (strcmp("0",expStr) || input == conf.dec_delimiter)) {
		    newStr = malloc(expStrLen + 2);
		    snprintf(newStr, expStrLen + 2, "%s%c", expStr, input);
		} else {
		    newStr = malloc(2);
		    snprintf(newStr, 2, "%c", input);
		    append = 1;
		}
		return newStr;
	    }
    }
    return NULL;
}

void simpleClearEntry()
{
    simpleCalcInit();
    num_set_ui(cur_number, 0);
}

void simpleCalcInit()
{
    static int initialized = 0;

    if (!initialized) {
	num_init(cur_number);
	num_init(prev_number);
	num_init(last_answer);
	num_init_set_ui(cur_number, 0);
	num_init_set_ui(last_answer, 0);
	initialized = 1;
    }
}

void simpleClearAll()
{
    simpleCalcInit();
    num_set_ui(cur_number, 0);
    num_set_ui(last_answer, 0);
    num_set_ui(prev_number, 0);
    operationPending = 0;
}
