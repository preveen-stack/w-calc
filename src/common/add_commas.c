#if HAVE_CONFIG_H
# include "config.h"
#endif

/* System Headers */
#if !defined(HAVE_CONFIG_H) || HAVE_STRING_H
# include <string.h>
#endif
#include <stdlib.h>                    /* for calloc() */
#include <wctype.h>                    /* for iswdigit() */

/* Internal Headers */
#include "add_commas.h"
#include "conf.h"

static int isvaliddigit(const wint_t digit,
                        const int base)
{
    switch (base) {
        default:
        case DECIMAL_FORMAT:
            return iswdigit(digit);
        case HEXADECIMAL_FORMAT:
            return iswxdigit(digit);
        case OCTAL_FORMAT:
            switch(digit) {
                case '0': case '1': case '2': case '3':
                case '4': case '5': case '6': case '7':
                    return 1;
                default:
                    return 0;
            }
        case BINARY_FORMAT:
            switch(digit) {
                case '0': case '1':
                    return 1;
                default:
                    return 0;
            }
    }
}

/* this function returns a copy of the input string with delimiters
 * appropriate for the specified base. */
/*@null@*/
char *add_commas(const char *input,
                 const int   base)
{
    struct _conf *conf = getConf();
    char        *tmpstring, *delimiter;
    unsigned int skip, prefix, ctr;
    char         separator;
    char         dec_delim = conf->dec_delimiter;
    size_t       preflen;
    size_t       fromcurs, tocurs;
    char        *exponent_chars;

    Dprintf("add_commas: %s, %i\n", input, base);
    if (NULL == input) {
        Dprintf("input was null, returning null\n");
        return NULL;
    }

    const size_t input_len = strlen(input);
    delimiter = strchr(input, dec_delim);
    if (NULL == delimiter) {
        dec_delim = 0;
        delimiter = strrchr(input, 0);
    }
    Dprintf("add_commas: input: %s ... delimiter: %s\n", input, delimiter);
    switch (base) {
        default:
        case DECIMAL_FORMAT:
            skip           = 3;
            prefix         = 0;
            separator      = conf->thou_delimiter;
            exponent_chars = "eE";
            break;
        case HEXADECIMAL_FORMAT:
            skip           = 2;
            prefix         = 2;
            separator      = ' ';
            exponent_chars = "@";
            break;
        case OCTAL_FORMAT:
            skip           = 4;
            prefix         = 1;
            separator      = conf->thou_delimiter;
            exponent_chars = "eE";
            break;
        case BINARY_FORMAT:
            skip           = 8;
            prefix         = 2;
            separator      = conf->thou_delimiter;
            exponent_chars = "eE";
            break;
    }
    if (0 == conf->print_prefixes) {
        prefix = 0;
    }
    if (*input == '-') {
        prefix++;
    }
    // the meat of the function
    preflen = (size_t)(delimiter - input);
    if (preflen < (size_t)(skip + prefix)) {
        return NULL;
    }
    Dprintf("tmpstring is alloc'd to be %lu long\n", preflen + strlen(input));
    tmpstring = calloc(preflen + strlen(input), sizeof(char));
    if (tmpstring == NULL) {
        return NULL;
    }
    ctr = (unsigned int)((delimiter - (input + prefix)) % skip);
    if (ctr == 0) {
        ctr = skip;
    }
    fromcurs = 0;
    tocurs   = 0;
    while (fromcurs < input_len && input[fromcurs] != dec_delim &&
           strchr(exponent_chars, input[fromcurs]) == NULL) {
        if (prefix == 0 && !isvaliddigit(input[fromcurs], base)) {
            Dprintf("caught a bad digit: %c returning null...\n", input[fromcurs]);
            free(tmpstring);
            return NULL;
        }
        Dprintf("from: %s to: %s \n", input + fromcurs, tmpstring);
        tmpstring[tocurs++] = input[fromcurs++];
        Dprintf("after copy from: %s to: %s \n", input + fromcurs, tmpstring);
        if (prefix != 0) {
            prefix--;
            Dprintf("skipping prefix...\n");
            continue;
        }
        if ((--ctr == 0) && (separator != '\0')) {
            tmpstring[tocurs++] = separator;
        }
        if (ctr == 0) {
            ctr = skip;
        }
    }
    Dprintf("tmpstring[tocurs-1] == %c\n", tmpstring[tocurs - 1]);
    Dprintf("  tmpstring[tocurs] == %c\n", tmpstring[tocurs]);
    Dprintf("  input[fromcurs-1] == %c\n", input[fromcurs - 1]);
    Dprintf("    input[fromcurs] == %c\n", input[fromcurs]);
    Dprintf("tmpstring = %s (ctr = %i)\n", tmpstring, ctr);
    if (ctr == skip) {
        if (separator == '\0') {
            tmpstring[tocurs++] = dec_delim;
        } else {
            tmpstring[tocurs - 1] = dec_delim;
        }
    }
    Dprintf("tmpstring = %s\n", tmpstring);
    if ((input[fromcurs] == 'e') || (input[fromcurs] == 'E')) {
        tmpstring[tocurs++] = input[fromcurs];
    }
    Dprintf("tmpstring = %s\n", tmpstring);
    if (fromcurs < input_len) {
        fromcurs++;
        while (fromcurs < input_len) {
            tmpstring[tocurs++] = input[fromcurs++];
        }
    }
    Dprintf("tmpstring = %s\n", tmpstring);
    return tmpstring;
}

/* vim:set expandtab: */
