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
#ifdef MEMWATCH
#include "memwatch.h"
#endif

/* this function returns a copy of the input string with delimiters
 * appropriate for the specified base. */
char *add_commas(const char *input, const int base)
{
    char *tmpstring, *delimiter;
    unsigned int skip, prefix;
    unsigned char ctr;
    char separator;
    char dec_delim = conf.dec_delimiter;
    size_t preflen;
    size_t fromcurs, tocurs;
    char * exponent_chars;
    const size_t input_len = strlen(input);

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
	    exponent_chars = "eE";
	    break;
	case HEXADECIMAL_FORMAT:
	    skip = 2;
	    prefix = 2;
	    separator = ' ';
	    exponent_chars = "@";
	    break;
	case OCTAL_FORMAT:
	    skip = 4;
	    prefix = 1;
	    separator = conf.thou_delimiter;
	    exponent_chars = "eE";
	    break;
	case BINARY_FORMAT:
	    skip = 8;
	    prefix = 2;
	    separator = conf.thou_delimiter;
	    exponent_chars = "eE";
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
    fromcurs = 0;
    tocurs = 0;
    while (fromcurs < input_len && input[fromcurs] != dec_delim &&
	    strchr(exponent_chars, input[fromcurs]) == NULL) {
	Dprintf("from: %s to: %s \n", input+fromcurs, tmpstring);
	tmpstring[tocurs++] = input[fromcurs++];
	if (prefix != 0) {
	    prefix--;
	    continue;
	}
	if (--ctr == 0) {
	    tmpstring[tocurs++] = separator;
	}
	if (ctr == 0) {
	    ctr = skip;
	}
    }
    Dprintf("tmpstring[tocurs-1] == %c\n",tmpstring[tocurs-1]);
    Dprintf("  input[fromcurs-1] == %c\n",input[fromcurs-1]);
    if (tmpstring[tocurs - 1] == separator) {
	tmpstring[tocurs - 1] = dec_delim;
    }
    if (input[fromcurs] == 'e' || input[fromcurs] == 'E') {
	tmpstring[tocurs++] = input[fromcurs];
    }
    if (fromcurs < input_len) {
	fromcurs++;
	while (fromcurs < input_len) {
	    tmpstring[tocurs++] = input[fromcurs++];
	}
    }
    return tmpstring;
}
