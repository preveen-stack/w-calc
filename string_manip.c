/*
 *  string_manip.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Fri Mar 01 2002.
 *  Copyright (c) 2001 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef GUI
#include "config.h"
#endif
#include <ctype.h>
#include "string_manip.h"

void strstrip(char strip, char *str)
{
	char *left, *right;

	for (left = right = str; *right; ++right) {
		if (right && *right && *right != strip) {
			*left = *right;
			++left;
		}
	}
	while (left != right) {
		*left = 0;
		++left;
	}
}

void strswap(char sw, char ap, char *str)
{
	while (str && *str) {
		if (*str == sw)
			*str = ap;
		++str;
	}
}

void strswap2(char sw, char ap, char *str)
{
	while (str && *str) {
		if (*str == sw)
			*str = ap;
		else if (*str == ap)
			*str = sw;
		++str;
	}
}

unsigned int count_digits(char *curs)
{
	unsigned int counter = 0;

	while (curs && *curs && *curs != 'e' && *curs != 'E') {
		counter += isdigit((int)(*curs));
		++curs;
	}
	return counter;
}

int justnumbers(char *curs)
{
	while (curs && *curs && (isdigit((int)(*curs)) || ispunct((int)(*curs))))
		curs++;
	if (curs && !*curs)				   // if we reached the end of the string
		return 1;
	else
		return 0;
}

void stripComments(char *curs)
{
	char *follower = curs;
	char update_follower = 1;

	while (curs && *curs) {
		if (*curs == '#') {
			*curs = 0;
			return;
		} else if (*curs == '/' && *(curs + 1) == '/') {
			*curs = 0;
			return;
		} else if (*curs == '/' && *(curs + 1) == '*') {
			update_follower = 0;
			curs++;
		} else if (*curs == '*' && *(curs + 1) == '/') {
			update_follower = 1;
			curs += 2;
		}
		if (update_follower) {
			*follower = *curs;
			++follower;
		}
		curs++;
	}
	*follower = 0;
}
