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
	printf( "This calculator recognizes the following symbols:\n"
		 SUPPORTED_SYMBOLS
		 "\nThe ! is factorial or boolean, depending on use.\n"
		 "Spaces are ignored (unknown symbols cause errors or are treated as variables)\n"
		 "This calculator accepts input in decimal (1.00), scientific notation (1.0e5),\n"
		 "octal (088), hexadecimal (0xff), and binary (0b11).\n\n"
		 "a   is a reserved variable that represents the previous answer.\n"
		 "\\pX is the way to set the precision, where X is the desired precision.\n"
		 "    This setting only affects output.\n"
		 "\\e or \\eng or \\engineering or \\eX or \\engX or \\engineeringX\n"
		 "    toggles the formatting of output between decimal and scientific notation.\n"
		 "    If X is used, 0 turns it off, anything else turns it on.\n"
		 "    This setting only applies when precision is specified.\n"
		 "\\q or q exits the program.\n"
		 "? or \\help or help  displays this help screen.\n\n"
		 "\\prefs prints out the current preference settings.\n"
		 "\\radians toggles radian mode.\n"
		 "\\cons or \\conservative toggles precision guards.\n"
		 "\\p or \\picky or \\l or \\lenient toggles stricter variable parsing rules.\n"
		 "\\b or \\bin or \\binary prints numbers in binary.\n"
		 "\\d or \\dec or \\decimal prints numbers in decimal notation.\n"
		 "\\h or \\x or \\hex or \\hexadecimal prints numbers in hex notation.\n"
		 "\\o or \\oct or \\octal prints numbers in octal notation.\n"
		 "\\r or \\radians toggles radian mode for sin-related functions.\n"
		 "\\re or \\remember or \\remember_errors toggles whether errors are\n"
		 "    remembered in the history.\n"
		 "\\round none|simple|sig_fig sets the rounding indication\n"
		 "\\dsepX sets the decimal separator character to X.\n"
		 "\\tsepX sets the thousands-place separator character to X.\n"
		 "\\hlimitX sets the limit on the length of the history.\n"
		 "\\openXXXXX loads a saved file\n"
		 "\\saveXXXXX saves the history and variable list to a file\n"
		 "\\li or \\list or \\listvars prints out the currently defined variables.\n\n"
		 "Variables are supported and may be assigned using the = operator.\n"
		 "To assign a variable, use the form:\n"
		 "foo = anylegalexpression\n"
		 "Thereafter, that variable name is the same as the literal value it represents.\n\n"
		 "Comments (ignored expressions) are either between /* and */ or\n"
		 "after a // or #\n"
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

