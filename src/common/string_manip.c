/*
 *  string_manip.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Fri Mar 01 2002.
 *  Copyright (c) 2001 Kyle Wheeler. All rights reserved.
 *
 */

#include <ctype.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "string_manip.h"
#if ! defined(HAVE_CONFIG_H) || HAVE_STRING_H
# include <string.h>
#else
# if !HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
char *strchr(), *strrchr();
#endif
#ifdef MEMWATCH
#include "memwatch.h"
#endif

void strstrip(const char strip, char *str)
{
    size_t left, right;

    for (left = right = 0; str[right] != 0; ++right) {
	if (str[right] != 0 && str[right] != strip) {
	    str[left++] = str[right];
	}
    }
    while (left != right) {
	str[left++] = 0;
    }
}

void strswap(const char sw, const char ap, char *str)
{
    size_t curs = 0;
    if (!str) return;
    while (str[curs] != 0) {
	if (str[curs] == sw)
	    str[curs] = ap;
	curs++;
    }
}

void strswap2(const char sw, const char ap, char *str)
{
    size_t curs = 0;
    if (!str) return;
    while (str[curs] != 0) {
	if (str[curs] == sw)
	    str[curs] = ap;
	else if (str[curs] == ap)
	    str[curs] = sw;
	++curs;
    }
}

unsigned int count_digits(const char *str)
{
    size_t curs = 0;
    unsigned int counter = 0;
    char base = 10;
    char * exponent_chars = "eE";
    char * base_chars = "1234567890";

    if (!str) return 0;
    if (str[0] == '0' && str[1] == 'x') {
	base = 16;
	curs += 2;
	exponent_chars = "@";
	base_chars = "1234567890abcdefABCDEF";
    } else if (str[0] == '0' && str[1] == 'b') {
	base = 2;
	curs += 2;
	exponent_chars = "eE";
	base_chars = "01";
    } else if (str[0] == '0') {
	base = 8;
	curs += 1;
	exponent_chars = "eE";
	base_chars = "12345670";
    }
    while (str[curs] != 0 && strchr(exponent_chars, str[curs]) == NULL) {
	if (strchr(base_chars, str[curs]) != NULL) {
	    counter ++;
	}
	++curs;
    }
    return counter;
}

int justnumbers(const char *str)
{
    size_t curs = 0;
    if (!str) return 0;
    while (str[curs] != 0 && (isdigit((int)(str[curs])) || ispunct((int)(str[curs]))))
	curs++;
    if (str[curs] == 0)		       // if we reached the end of the string
	return 1;
    else
	return 0;
}

void stripComments(char *str)
{
    size_t curs = 0;
    size_t follower = 0;
    int update_follower = 1;

    if (!str) return;
    while (str[curs] != '\0') {
	if (str[curs] == '#') {
	    str[curs] = 0;
	    return;
	} else if (str[curs] == '/' && str[curs + 1] == '/') {
	    str[curs] = 0;
	    return;
	} else if (str[curs] == '/' && str[curs + 1] == '*') {
	    update_follower = 0;
	    curs++;
	} else if (str[curs] == '*' && str[curs + 1] == '/') {
	    update_follower = 1;
	    curs += 2;
	}
	if (update_follower != 0) {
	    str[follower] = str[curs];
	    follower++;
	}
	curs++;
    }
    str[follower] = 0;
}
