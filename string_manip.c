/*
 *  string_manip.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Fri Mar 01 2002.
 *  Copyright (c) 2001 __MyCompanyName__. All rights reserved.
 *
 */

#include "string_manip.h"

void strstrip (char strip, char * str)
{
	char *left, *right;
	for (left=right=str; *right; ++right) {
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

void strswap (char sw, char ap, char * str)
{
	while (str && *str) {
		if (*str == sw)
			*str = ap;
		++str;
	}
}

void strswap2 (char sw, char ap, char * str)
{
	while (str && *str) {
		if (*str == sw)
			*str = ap;
		else if (*str == ap)
			*str = sw;
		++str;
	}
}