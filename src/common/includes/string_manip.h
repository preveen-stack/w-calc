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

void strstrip(const char,
              char *);
/* this replaces all instances of sw in str with ap */
void strswap(const char sw,
             const char ap,
             char      *str);
/* this replaces all instances of sw in str with ap, and all instances of ap in str with sw */
void strswap2(const char sw,
              const char ap,
              char      *str);
unsigned int count_digits(const char *);
int          justnumbers(const char *);
void         stripComments(char *);

#endif // ifndef KBW_STRING_MANIP
/* vim:set expandtab: */
