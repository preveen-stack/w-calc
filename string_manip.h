/*
 *  string_manip.h
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Fri Mar 01 2002.
 *  Copyright (c) 2001 Kyle Wheeler. All rights reserved.
 *
 */

#ifndef KBW_STRING_MANIP
#define KBW_STRING_MANIP

void strstrip(char, char *);
void strswap(char, char, char *);
void strswap2(char, char, char *);
unsigned int count_digits(char *);
int justnumbers(char *);
void stripComments(char *);

#endif
