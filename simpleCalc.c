#include "simpleCalc.h"
#include "calculator.h"
#include <ctype.h> // for isdigit
#include <string.h> // for strcmp/strlen/stpcpy/strdup
#include <stdlib.h> // for calloc
#include <stdio.h>

static char lastchar = 0;
static char operator = 0;
static char append = 1;
static double cur_number = 0.0;
static double prev_number = 0.0;

void simpleEval()
{
	Dprintf("operator: %c\n",operator);
	switch (operator) {
		case '+':
			prev_number = simple_exp(prev_number, wplus, cur_number);
			break;
		case '-':
			prev_number = simple_exp(prev_number, wminus, cur_number);
			break;
		case '*':
			prev_number = simple_exp(prev_number, wmult, cur_number);
			break;
		case '/':
			prev_number = simple_exp(prev_number, wdiv, cur_number);
			break;
		default:
			prev_number = cur_number;
			break;
	}
}

char *simpleCalc(unsigned char input, char *expStr)
{
	Dprintf("simpleCalc: %c, %s\n",input,expStr);
	Dprintf(" ~ cur: %f, prev: %f\n",cur_number, prev_number);
	if (input == '+' || input == '-' || input == '*' || input == '/') {
		// if the input is one of the operators
		append = 0;
		if (lastchar != input) {
			cur_number = parseme(expStr);
		}
		simpleEval();
		operator = input;
	} else if (input == '=') {
		simpleEval();
		set_prettyanswer(prev_number);
		last_answer = cur_number = prev_number;
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
			tail = stpcpy(newStr, expStr);
			*tail = input;
		} else {
			newStr = (char *)calloc(sizeof(char), 2);
			*newStr = input;
		}
		append = 1;
		cur_number = parseme(newStr);
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
	cur_number = 0.0;
//	operator = 0;
	lastchar = 0;
}

void simpleClearAll()
{
	cur_number = 0.0;
	last_answer = 0.0;
	prev_number = 0.0;
	operator = 0;
	lastchar = 0;
}
