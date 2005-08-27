/*
 *  help.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Wed Feb 12 2003.
 *  Copyright (c) 2003 Kyle Wheeler. All rights reserved.
 *
 */

#include <stdio.h>
#include <gmp.h>
#include <mpfr.h>
#include "help.h"

#ifndef GUI
#include "config.h"
#endif
#include "definitions.h"

void print_interactive_help (void)
{
	printf("Wcalc %s by Kyle Wheeler.\nVersion 0.1.x written in C++ for CS240C at Ohio University, Jan 14 2000\n",VERSION);
	printf("Version 0.2.x rewritten for lex/yacc in C, Nov 25 2001\n");
	printf("Version 0.3.x released under GPL, Jan 2003\n");
	printf( "This calculator recognizes the following symbols:\n"
		 SUPPORTED_SYMBOLS
		 "\nThe ! is factorial or boolean, depending on use.\n"
		 "Spaces are ignored (unknown symbols cause errors or are treated as variables)\n"
		 "This calculator accepts input in decimal (1.00), scientific notation (1.0e5),\n"
		 "octal (088), hexadecimal (0xff), and binary (0b11).\n\n"
		 "a   is a reserved variable that represents the previous answer.\n"
		 "\\pX :\n"
		 "    Sets the precision, where X is the desired precision.\n"
		 "    Precision here is in digits. This setting only affects output.\n"
		 "\\bitsX :\n"
		 "    Sets the number of bits used internally to represent numbers.\n"
		 "    This number must be above %li and below %li.\n"
		 "\\e or \\eng or \\engineering or \\eX or \\engX or \\engineeringX :\n"
		 "    Toggles the formatting of output between decimal and scientific\n"
		 "    notation.  If X is used, 0 turns it off, anything else turns it\n"
		 "    on.  This setting only applies when precision is specified.\n"
		 "\\q or q :\n"
		 "    Exits the program.\n"
		 "? or \\help or help :\n"
		 "    Displays this help screen.\n"
		 "\\prefs :\n"
		 "    Prints out the current preference settings.\n"
		 "\\cons or \\conservative :\n"
		 "    Toggles precision guards.\n"
		 "\\p or \\picky or \\l or \\lenient :\n"
		 "    Toggles stricter variable parsing rules.\n"
		 "\\b or \\bin or \\binary :\n"
		 "    Prints numbers in binary.\n"
		 "\\d or \\dec or \\decimal :\n"
		 "    Prints numbers in decimal notation.\n"
		 "\\h or \\x or \\hex or \\hexadecimal :\n"
		 "    Prints numbers in hex notation.\n"
		 "\\o or \\oct or \\octal :\n"
		 "    Prints numbers in octal notation.\n"
		 "\\r or \\radians :\n"
		 "    Toggles radian mode for trigonometric functions.\n"
		 "\\re or \\remember or \\remember_errors :\n"
		 "    Toggles whether errors are remembered in the history.\n"
		 "\\round none|simple|sig_fig :\n"
		 "    Sets the rounding indication.\n"
		 "\\dsepX :\n"
		 "    Sets the decimal separator character to X.\n"
		 "\\tsepX :\n"
		 "    Sets the thousands-place separator character to X.\n"
		 "\\hlimitX :\n"
		 "    Sets the limit on the length of the history.\n"
		 "\\openXXXXX :\n"
		 "    Loads a saved file\n"
		 "\\saveXXXXX :\n"
		 "    Saves the history and variable list to a file\n"
		 "\\ints :\n"
		 "    Toggles whether long integers will be abbreviated or not\n"
		 "    (conflicts with engineering notation for large numbers, but not\n"
		 "    for decimals).\n"
		 "\\convert unit1 to unit2 :\n"
		 "    Converts the previous answer from the first unit to the second.\n"
		 "\\li or \\list or \\listvars :\n"
		 "    Prints out the currently defined variables.\n\n"
		 "Variables are supported and may be assigned using the = operator.\n"
		 "To assign a variable, use the form:\n"
		 "foo = anylegalexpression\n"
		 "Thereafter, that variable name is the same as the literal value it represents.\n\n"
		 "Comments (ignored expressions) are either between /* and */ or\n"
		 "after a // or #\n",
		(long int) MPFR_PREC_MIN, (long int) MPFR_PREC_MAX
		 );
}


void print_command_help (void)
{
	printf("Wcalc %s by Kyle Wheeler.\n",VERSION);
	printf("Version 0.1.x written in C++ for CS240C at Ohio University, Jan 14 2000\n");
	printf("Version 0.2.x rewritten for lex/yacc in C, Nov 25 2001\n\n");
	printf( "This calculator recognized the following symbols:\n"
		 SUPPORTED_SYMBOLS
		 "\nThe ! is factorial or boolean, depending on use.\n"
		 "Spaces are ignored (unknown symbols cause errors or are treated as variables)\n"
		 "This calculator accepts input in decimal (1.00), scientific notation (1.0e5),\n"
		 "octal (088), hexadecimal (0xff), and in binary (0b11).\n\n"
		 "-v, --version            Prints the version number.\n"
		 "-Px                      Sets the precision, where x is the desired precision.\n"
		 "                         This setting only affects output.\n"
		 "-E                       Toggles the formatting of output between decimal\n"
		 "                         and scientific notation.\n"
		 "                         This setting only applies when precision is specified.\n"
		 "-d, -dec, --decimal      Prints the output in decimal form.\n"
		 "-h, -hex, --hexadecimal  Prints the output in hexadeximal form.\n"
		 "-o, -oct, --octal        Prints the output in octal form.\n"
		 "-b, -bin, --binary       Prints the output in binary form.\n"
		 "-p                       Prints the prefixes for hexadecimal, octal and\n"
		 "                         binary forms.\n"
		 "-l, --lenient            Makes the parser assume that uninitialized variables\n"
		 "                         have a value of zero.\n"
		 "-r, --radians            This makes all trigonometric functions work in radians.\n\n"
		 "Variables are supported and may be assigned using the = operator.\n"
		 "To assign a variable, use the form:\n"
		 "foo = anylegalexpression\n"
		 "Thereafter, that variable name is the same as the literal value it represents.\n\n"
		 "Executing this program without any arguments enters an 'interactive' mode\n"
		 "with a few more features and options.\n\n"
		 );
}

