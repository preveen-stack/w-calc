#include "simpleCalc.h"
#include "calculator.h"
#include "number.h"
#include <ctype.h>		       // for isdigit
#include <string.h>		       // for strcmp/strlen/stpcpy/strdup
#include <stdlib.h>		       // for calloc
#include <stdio.h>
#ifdef MEMWATCH
#include "memwatch.h"
#endif

static char lastchar = 0;
static char operator = 0;
static char append = 1;
static Number cur_number;
static Number prev_number;

void simpleCalcInit();

void simpleEval()
{
    simpleCalcInit();
    Dprintf("operator: %c\n", operator);
    switch (operator) {
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

char *simpleCalc(unsigned char input, char *expStr)
{
    simpleCalcInit();
    Dprintf("simpleCalc: %c, %s\n", input, expStr);
    Dprintf(" ~ cur: %f, prev: %f\n", num_get_d(cur_number),
	    num_get_d(prev_number));
    if (input == '+' || input == '-' || input == '*' || input == '/') {
	// if the input is one of the operators
	append = 0;
	if (lastchar != input) {
	    parseme(expStr);
	    num_set(cur_number, last_answer);
	}
	simpleEval();
	operator = input;
    } else if (input == '=') {
	simpleEval();
	set_prettyanswer(prev_number);
	num_set(cur_number, prev_number);
	num_set(last_answer, prev_number);
	operator = 0;
	append = 0;
	return NULL;
    } else if (input == conf.dec_delimiter || isdigit(input)) {
	// if the input is part of a number
	char *newStr;

	if (append && input != conf.dec_delimiter && !strcmp(expStr, "0")) {
	    append = 0;
	}
	if (append) {
	    char *tail;

	    newStr = (char *)calloc(sizeof(char), strlen(expStr) + 2);
	    tail = (char *)stpcpy(newStr, expStr);
	    *tail = input;
	} else {
	    newStr = (char *)calloc(sizeof(char), 2);
	    *newStr = input;
	}
	append = 1;
	parseme(newStr);
	num_set(cur_number, last_answer);
	return newStr;
    } else {
	// if the input is not an acceptable character... do nothing

	return strdup(expStr);
    }
    lastchar = input;
    set_prettyanswer(prev_number);
    return NULL;
}

void simpleClearEntry()
{
    simpleCalcInit();
    num_set_ui(cur_number, 0);
//  operator = 0;
    lastchar = 0;
}

void simpleCalcInit()
{
    static int initialized = 0;

    if (!initialized) {
	num_init(cur_number);
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
    operator = 0;
    lastchar = 0;
}
