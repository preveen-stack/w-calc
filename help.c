/*
 *  help.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Wed Feb 12 2003.
 *  Copyright (c) 2003 Kyle Wheeler. All rights reserved.
 *
 */

#include <stdio.h>
#include "help.h"

#ifndef GUI
#include "config.h"
#endif
#include "definitions.h"

void print_interactive_help (void)
{
	printf("Wcalc %s by Kyle Wheeler.\nVersion 0.1.x written in C++ for CS240C at Ohio University, Jan 14 2000\n",VERSION);
	printf("Version 0.2.x rewritten for lex/yacc in C, Nov 25 2001\n");
	printf( "This calculator recognized the following symbols:\n"
		 SUPPORTED_SYMBOLS
		 "The ! is factorial or boolean, depending on use.\n"
		 "Spaces are ignored (unknown symbols cause errors or are treated as variables)\n"
		 "This calculator accepts input in decimal (1.00), scientific notation (1.0e5),\n"
		 "octal (088), hexadecimal (0xff), and binary (0b11).\n\n"
		 "\\pX is the way to set the precision, where X is the desired precision.\n"
		 "    This setting only affects output.\n"
		 "\\e  toggles the formatting of output between decimal and scientific notation.\n"
		 "    This setting only applies when precision is specified.\n"
		 "a   in an expression represents the previous answer.\n"
		 "\\q  exits the program.\n"
		 "?   displays this help screen.\n"
		 "\\radians toggles radian mode.\n"
		 "\\commas toggles output delineation between commas and periods.\n"
		 "\\p or \\picky toggles stricter variable parsing rules.\n"
		 "\\b or \\bin or \\binary prints numbers in binary.\n"
		 "\\d or \\dec or \\decimal prints numbers in decimal notation.\n"
		 "\\h or \\x or \\hex or \\hexadecimal prints numbers in hex notation.\n"
		 "\\o or \\oct or \\octal prints numbers in octal notation.\n"
		 "\\r or \\radians toggles radian mode for sin-related functions.\n"
		 "\\s or \\strict toggles stricter number parsing rules.\n"
		 "\\dsepX sets the decimal separator character to X.\n"
		 "\\tsepX sets the thousands-place separator character to X.\n"
		 "\\cons or \\conservative toggles precision guards.\n"
		 "\\hlimitX sets the limit on the length of the history.\n"
		 "\\li or \\list or \\listvars prints out the currently defined variables.\n\n"
		 "Variables are supported and may be assigned using the = operator.\n"
		 "To assign a variable, use the form:\n"
		 "foo = anylegalexpression\n"
		 "Thereafter, that variable name is the same as the literal value it represents.\n"
		 );
}


void print_command_help (void)
{
	printf("Wcalc %s by Kyle Wheeler.\nVersion 0.1.x written in C++ for CS240C at Ohio University, Jan 14 2000\n",VERSION);
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

