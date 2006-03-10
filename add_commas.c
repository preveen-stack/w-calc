#if HAVE_CONFIG_H
# include "config.h"
#endif

#if ! defined(HAVE_CONFIG_H) || HAVE_STRING_H
# include <string.h>
#endif
#include <stdlib.h>		       /* for calloc() */
#include <ctype.h>

#include "calculator.h"
#include "add_commas.h"

/* this function returns a copy of the input string with delimiters
 * appropriate for the specified base. */
char *add_commas(char *input, int base)
{
    char *copyto, *copyfrom, *tmpstring, *delimiter;
    unsigned int skip, prefix;
    unsigned char ctr;
    char separator;
    char dec_delim = conf.dec_delimiter;
    size_t preflen;

    Dprintf("add_commas: %s, %i\n", input, base);
    if (NULL == input) {
	return NULL;
    }
    if (!isdigit(*input)) {
	return NULL;
    }
    delimiter = strchr(input, dec_delim);
    if (NULL == delimiter) {
	dec_delim = 0;
	delimiter = strrchr(input, 0);
    }
    Dprintf("add_commas: input: %s\n", input);
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
    if (!conf.print_prefixes) {
	prefix = 0;
    }
    if (*input == '-') {
	prefix++;
    }
    // the meat of the function
    preflen = delimiter - input;
    if (preflen < (skip + prefix)) {
	return NULL;
    }
    Dprintf("tmpstring is alloc'd to be %lu long\n", preflen + strlen(input));
    tmpstring = calloc(preflen + strlen(input), sizeof(char));
    ctr = (delimiter - (input + prefix)) % skip;
    if (ctr == 0) {
	ctr = skip;
    }
    copyfrom = input;
    copyto = tmpstring;
    while (*copyfrom && *copyfrom != dec_delim && *copyfrom != 'E' && *copyfrom != 'e') {
	Dprintf("from: %s to: %s \n", copyfrom, tmpstring);
	*copyto++ = *copyfrom++;
	if (prefix != 0) {
	    prefix--;
	    continue;
	}
	if (--ctr == 0) {
	    *copyto++ = separator;
	}
	if (ctr == 0) {
	    ctr = skip;
	}
    }
    Dprintf("*(copyto - 1) == %c\n",*(copyto-1));
    Dprintf("*(copyfrom - 1) == %c\n",*(copyfrom-1));
    if (*(copyto - 1) == separator) {
	*(copyto - 1) = dec_delim;
    }
    if (*copyfrom == 'e' || *copyfrom == 'E') {
	*copyto++ = *copyfrom;
    }
    if (*copyfrom) {
	copyfrom++;
	while (*copyfrom) {
	    *copyto++ = *copyfrom++;
	}
    }
    return tmpstring;
}
