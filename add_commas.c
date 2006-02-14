#if HAVE_CONFIG_H
# include "config.h"
#endif

#if ! defined(HAVE_CONFIG_H) || HAVE_STRING_H
# include <string.h>
#endif
#include <stdlib.h>		       /* for calloc() */

#include "calculator.h"
#include "add_commas.h"

/* this function returns a copy of the input string with delimiters
 * appropriate for the specified base. */
char * add_commas(char *input, int base)
{
    char *copyto, *copyfrom, *tmpstring, *delimiter;
    unsigned int skip, prefix;
    unsigned char ctr;
    char separator;
    char dec_delim = conf.dec_delimiter;

    Dprintf("add_commas: %s, %i\n",input, base);
    if (NULL == input) { return NULL; }
    delimiter = strchr(input,dec_delim);
    if (NULL == delimiter) {
	dec_delim = 0;
	delimiter = strrchr(input,0);
    }
    switch (base) {
	default:
	case DECIMAL_FORMAT:
	    skip = 3;
	    prefix = 0;
	    separator = conf.thou_delimiter;
	    break;
	case HEXADECIMAL_FORMAT:
	    skip = 2;
	    prefix = 2;
	    separator = ' ';
	    break;
	case OCTAL_FORMAT:
	    skip = 4;
	    prefix = 1;
	    separator = conf.thou_delimiter;
	    break;
	case BINARY_FORMAT:
	    skip = 8;
	    prefix = 2;
	    separator = conf.thou_delimiter;
	    break;
    }
    if (! conf.print_prefixes) {
	prefix = 0;
    }
    if (*input == '-') {
	prefix++;
    }

    // the meat of the function
    if (delimiter - input < (skip+prefix)) {
	return NULL;
    }
    tmpstring = calloc(sizeof(char),(delimiter-input)+strlen(input));
    ctr = (delimiter-(input+prefix))%skip;
    if (ctr == 0) {
	ctr = skip;
    }
    copyfrom = input;
    copyto = tmpstring;
    while (*copyfrom && *copyfrom != dec_delim) {
	Dprintf("from: %s to: %s \n",copyfrom,tmpstring);
	*copyto++ = *copyfrom++;
	if (prefix != 0) {
	    prefix --;
	    continue;
	}
	if (--ctr == 0) {
	    *copyto++ = separator;
	}
	if (ctr == 0) {
	    ctr = skip;
	}
    }
    if (*(copyto-1) == separator) {
	*(copyto-1) = dec_delim;
    }
    copyfrom++;
    while (*copyfrom) {
	*copyto++ = *copyfrom++;
    }
    return tmpstring;
}

