#if HAVE_CONFIG_H
# include "config.h"
#endif

#if ! defined(HAVE_CONFIG_H) || HAVE_STRING_H
# include <string.h>
#endif
#include <stdio.h>		       /* for snprintf() */
#include <stdlib.h>		       /* for calloc() */

#include "number.h"

#include "calculator.h"
#include "string_manip.h"
#include "number_formatting.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif

static size_t zero_strip(char *num);
static void add_prefix(char *num, size_t length, int base);
static char *engineering_formatted_number(const char *digits, num_exp_t exp,
					  const int precision, const int base,
					  const int prefix,
					  char *truncated_flag);
static char *full_precision_formatted_number(const char *digits, num_exp_t exp,
					     const int base,
					     const int prefix);
static char *automatically_formatted_number(const char *digits, num_exp_t exp,
					    const int precision,
					    const int base, const int prefix,
					    char *truncated_flag);
static char *precision_formatted_number(const char *digits, num_exp_t exp,
					const int precision, const int base,
					const int prefix);

/* this function takes a number (mpfr_t) and prints it.
 * This is a blatant ripoff of mpfr's mpfr_out_str(), because it formats things
 * (sorta) the way I want them formatted, though this prints things out to a
 * string, and does all the fancy presentation stuff we've come to expect from
 * wcalc.
 */
char *num_to_str_complex(const Number num, const int base,
			 const enum engineering_modes engr, const int prec,
			 const int prefix, char *truncated_flag)
{
    char *s, *retstr;
    num_exp_t e;

    Dprintf("num_to_str_complex: base: %i, engr: %i, prec: %i, prefix: %i\n",
	    base, engr, prec, prefix);
    if (num_is_nan(num)) {
	return (char *)strdup("@NaN@");
    }
    if (num_is_inf(num)) {
	if (num_sign(num) > 0) {
	    return (char *)strdup("@Inf@");
	} else {
	    return (char *)strdup("-@Inf@");
	}
    }
    if (num_is_zero(num)) {
	if (num_sign(num) >= 0) {
	    return (char *)strdup("0");
	} else {
	    return (char *)strdup("-0");
	}
    }

    s = num_get_str(NULL, &e, base, 0, num);
    /* s is the string
     * e is the number of integers (the exponent) if positive
     *
     * Now, if there's odd formatting involved, make mpfr do the rounding,
     * so we know it's "correct":
     */
    if (prec > -1) {
	if (engr == never) {
	    size_t significant_figures = 0;

	    Dprintf("prec > -1 && engr == never\n");
	    /*printf("e: %li\n", (long)e);
	     * printf("s: %s\n", s);
	     * printf("prec: %i\n", prec); */
	    significant_figures = ((e > 0) ? e : 0) + prec;
	    Dprintf("sig figs = %i\n", significant_figures);
	    if (significant_figures < 2) {	/* MPFR-defined minimum (why?) */
		s = num_get_str(s, &e, base, 2, num);
		Dprintf("s=%s, e = %i\n", s, (int)e);
		if (s[1] > '4') {      /* XXX: LAME! */
		    unsigned foo;

		    foo = s[0] - '0';
		    foo++;
		    snprintf(s, 3, "%u", foo);
		    Dprintf("s = %s\n", s);
		    e = 1;
		}
	    } else {
		num_free_str(s);
		s = num_get_str(NULL, &e, base, significant_figures, num);
	    }
	} else {
	    int left_digits = 0;
	    Number temp;

	    Dprintf("engr == auto || engr == always\n");
	    /* first, count how many figures to the left of the decimal */
	    num_init_set(temp, num);
	    while (num_get_d(temp) >= 1.0) {
		num_div_ui(temp, temp, base);
		left_digits++;
	    }
	    num_free(temp);
	    if (left_digits == 0) {
		left_digits = 1;
	    }
	    Dprintf("left_digits = %i, asking for %i\n", left_digits,
		    left_digits + prec);
	    s = num_get_str(NULL, &e, base, left_digits + prec, num);
	}
    }
    Dprintf("post-mpfr e: %li s: %s\n", (long int)e, s);
    *truncated_flag = 0;
    if (-2 == prec) {
	retstr = full_precision_formatted_number(s, e, base, prefix);
    } else {
	switch (engr) {
	    case always:
		Dprintf("ALWAYS print engineering\n");
		retstr =
		    engineering_formatted_number(s, e, prec, base, prefix,
						 truncated_flag);
		break;
	    case never:
		Dprintf("NEVER print engineering\n");
		retstr = precision_formatted_number(s, e, prec, base, prefix);
		break;
	    default:
	    case automatic:
		Dprintf("AUTOMATICALLY decide on engineering formatting\n");
		retstr =
		    automatically_formatted_number(s, e, prec, base, prefix,
						   truncated_flag);
	}
    }
    num_free_str(s);
    Dprintf("return string: %s\n", retstr);
    return retstr;
}

char *precision_formatted_number(const char *digits, num_exp_t exp,
				 const int precision, const int base,
				 const int prefix)
{
    size_t length;
    size_t full_length;
    size_t decimal_count = 0;
    size_t print_limit;
    char *retstring, *curs;
    size_t d_index = 0;

    length = strlen(digits);
    /* testing against both zero and length because length is unsigned */
    if (exp > 0 && (size_t) exp > length)
	length = exp;
    length += 3;

    if (length < (size_t) (precision + 3)) {	// leading zero, decimal, and null
	length = (size_t) (precision + 3);
    }
    Dprintf("Precision Formatted Number\n");
    Dprintf("digits: %s(%u), exp: %i, base: %i, prefix: %i, precision: %i\n",
	    digits, (unsigned)length, (int)exp, base, prefix, precision);

    // ten extra, 'cuz of the *possible* exponent
    full_length = length + 10;
    curs = retstring = (char *)calloc(full_length, sizeof(char));
    Dprintf("length: %lu, full_length: %lu\n", length, full_length);
    // now, copy the digits into the output string, carefully

    // copy over the negative sign
    if (digits[d_index] == '-') {
	snprintf(curs++, length--, "%c", digits[d_index++]);
    }
    // copy in a prefix
    if (prefix) {
	char *nc;

	add_prefix(curs, length, base);
	nc = strchr(curs, '\0');
	length -= nc - curs;
	curs = nc;
    }
    // copy in the integers
    Dprintf("exp = %i\n", exp);
    if (exp > 0) {
	snprintf(curs++, length--, "%c", digits[d_index++]);
	exp--;			       // leading digit
	while (exp > 0 && digits[d_index] && length > 0) {
	    snprintf(curs++, length--, "%c", digits[d_index++]);
	    exp--;
	}
	for (; exp > 0; exp--) {
	    snprintf(curs++, length--, "0");
	}
    } else {
	snprintf(curs++, length--, "0");
    }
    if (precision > 0) {
	// the decimal
	snprintf(curs++, length--, ".");
	Dprintf("the integers: %s\n", retstring);
	Dprintf("l: %lu, fl: %lu, dc: %u, p: %i, e: %i\n", length,
		full_length, (unsigned)decimal_count, (int)precision,
		(int)exp);
	// everything after this is affected by decimalcount
	// copy in the leading zeros
	while (exp < 0 && (ssize_t) decimal_count <= precision && length > 0) {
	    snprintf(curs++, length--, "0");
	    exp++;
	    decimal_count++;
	}
	Dprintf("l: %lu, fl: %lu, dc: %u, p: %i, e: %i\n", length,
		full_length, (unsigned)decimal_count, (int)precision,
		(int)exp);
	// copy in the rest of the mantissa (the decimals)
	Dprintf("leading zeros: %s\n", retstring);
	// this variable exists because snprintf's return value is unreliable,
	// and can be larger than the number of digits printed
	print_limit =
	    ((length <
	      (precision - decimal_count + 1)) ? length : (precision -
							   decimal_count +
							   1));
	snprintf(curs, print_limit, "%s", &digits[d_index]);
    }

    return retstring;
}

char *full_precision_formatted_number(const char *digits, num_exp_t exp,
				      const int base, const int prefix)
{
    size_t length;
    size_t full_length;
    size_t decimal_count = 0;
    size_t printed;
    char *retstring, *curs;
    size_t d_index = 0;

    length = strlen(digits);
    /* testing against both zero and length because length is unsigned */
    if (exp > 0 && (size_t) exp > length)
	length = exp;
    length += 3;		       /* the null, the (possible) sign, and the decimal */

    Dprintf("Full Precision Formatted Number\n");
    Dprintf("digits: %s(%u), exp: %i, base: %i, prefix: %i\n", digits,
	    (unsigned)length, (int)exp, base, prefix);
    Dprintf("strlen(digits): %u\n", (unsigned)strlen(digits));

    // ten extra, 'cuz of the *possible* exponent
    full_length = length + 10;
    curs = retstring = (char *)calloc(sizeof(char), full_length);
    Dprintf("length: %lu, full_length: %lu\n", length, full_length);

    // now, copy the digits into the output string, carefully

    // copy over the negative sign
    if (digits[d_index] == '-') {
	snprintf(curs++, length--, "%c", digits[d_index++]);
    }
    // copy in a prefix
    if (prefix) {
	char *nc;

	add_prefix(curs, length, base);
	nc = strchr(curs, '\0');
	length -= nc - curs;
	curs = nc;
    }
    Dprintf("ready for ints: %s\n", retstring);
    // copy over the integers
    if (exp > 0) {
	snprintf(curs++, length--, "%c", digits[d_index++]);
	exp--;			       // leading digit
	while (exp > 0 && digits[d_index] && length > 0) {
	    snprintf(curs++, length--, "%c", digits[d_index++]);
	    exp--;
	}
	for (; exp > 0; exp--) {
	    snprintf(curs++, length--, "0");
	}
    } else {
	snprintf(curs++, length--, "0");
    }
    // the decimal
    snprintf(curs++, length--, ".");
    Dprintf("the integers: %s\n", retstring);
    Dprintf("length: %lu, full_length: %lu\n", length, full_length);
    // XXX: Currently, this function is not used for decimals, so...

    // the leading decimal zeros
    while (exp < 0 && length > 0) {
	snprintf(curs++, length--, "0");
	exp++;
	decimal_count++;
    }
    // the rest of the mantissa (the decimals)

    // this variable exists because snprintf's return value is unreliable.
    // and can be larger than the number of digits printed
    printed = ((length - 1 < strlen(&digits[d_index])) ? length - 1 : strlen(&digits[d_index]));
    snprintf(curs, length, "%s", &digits[d_index]);
    length -= printed;
    decimal_count += printed;

    // strip off the trailing 0's
    zero_strip(retstring);

    // copy in an exponent if necessary
    if (exp != 0) {
	curs = strchr(retstring, '\0');
	Dprintf("space left: %lu\n", full_length - (curs - retstring));
	snprintf(curs, full_length - (curs - retstring),
		 (base <= 10 ? "e%ld" : "@%ld"), (long)exp);
    }

    return retstring;
}

char *automatically_formatted_number(const char *digits, num_exp_t exp,
				     const int precision, const int base,
				     const int prefix, char *truncated_flag)
{
    size_t length;
    size_t full_length;
    size_t decimal_count = 0;
    size_t printed;
    char *retstring, *curs;
    size_t d_index = 0;
    const long original_exp = exp;

    length = strlen(digits);
    /* testing against both zero and length because length is unsigned */
    if (exp > 0 && (size_t) exp > length)
	length = exp;
    length += 3;		       /* the null, the (possible) sign, and the decimal */

    Dprintf("Automatically Formatted Number\n");
    Dprintf("digits: %s(%u), exp: %i, base: %i, prefix: %i\n", digits,
	    (unsigned)length, (int)exp, base, prefix);
    Dprintf("strlen(digits): %u\n", (unsigned)strlen(digits));

    // ten extra, 'cuz of the *possible* exponent
    full_length = length + 10;
    curs = retstring = (char *)calloc(sizeof(char), full_length);
    Dprintf("length: %lu, full_length: %lu\n", length, full_length);

    // now, copy the digits into the output string, carefully

    // copy over the negative sign
    if (digits[d_index] == '-') {
	snprintf(curs++, length--, "%c", digits[d_index++]);
	Dprintf("copied negative sign: %s\n", retstring);
    }
    // copy in a prefix
    if (prefix) {
	char *nc;

	add_prefix(curs, length, base);
	nc = strchr(curs, '\0');
	length -= nc - curs;
	curs = nc;
	Dprintf("added prefix: %s\n", retstring);
    }
    Dprintf("ready for ints: %s\n", retstring);
    // copy over the integers
    if (exp > 0) {
	snprintf(curs++, length--, "%c", digits[d_index++]);
	exp--;			       // leading digit
	while (exp > 0 && digits[d_index] && length > 0) {
	    snprintf(curs++, length--, "%c", digits[d_index++]);
	    exp--;
	}
	for (; exp > 0; exp--) {
	    snprintf(curs++, length--, "0");
	}
    } else {
	snprintf(curs++, length--, "0");
    }
    // the decimal
    snprintf(curs++, length--, ".");
    Dprintf("the integers: %s\n", retstring);
    Dprintf("length: %lu, full_length: %lu\n", length, full_length);
    // XXX: Currently, this function is not used for decimals, so...

    // the leading decimal zeros
    while (exp < 0 && length > 0) {
	snprintf(curs++, length--, "0");
	exp++;
	decimal_count++;
    }
    // the rest of the mantissa (the decimals)

    // this variable exists because snprintf's return value is unreliable.
    // and can be larger than the number of digits printed
    printed = ((length - 1 < strlen(digits+d_index)) ? length - 1 : strlen(digits+d_index));
    snprintf(curs, length, "%s", digits+d_index);
    length -= printed;
    decimal_count += printed;

    if (precision == -1) {
	char *period;

	// strip off the trailing 0's
	zero_strip(retstring);
	/* XXX: This is a stupid hack; the idea is just to get the mpfr output
	 * to match the double output. */
	period = strchr(retstring, '.');
	Dprintf("retstring: %s\n", retstring);
	Dprintf("period: %s\n", period);
	if (period && strlen(period) > 10) {
	    period[10] = 0;
	    *truncated_flag = 1;
	    zero_strip(retstring);
	}
    } else if (precision >= 0) {
	char *period = strchr(retstring, '.');

	Dprintf("period: %s\n", period);
	if (period != NULL) {
	    if (precision == 0) { // this removes extraneous periods
		*period = 0;
	    }
	    period ++;
	    if (strlen(period) > (size_t) precision) {
		Dprintf("truncating down to precision...\n");
		period[precision] = 0;
		*truncated_flag = 1;
	    }
	}
    }
    // copy in an exponent if necessary
    if (exp != 0) {
	curs = strchr(retstring, '\0');
	Dprintf("space left: %lu\n", full_length - (curs - retstring));
	snprintf(curs, full_length - (curs - retstring),
		 (base <= 10 ? "e%ld" : "@%ld"), original_exp);
    }

    return retstring;
}

char *engineering_formatted_number(const char *digits, num_exp_t exp,
				   const int precision, const int base,
				   const int prefix, char *truncated_flag)
{
    size_t length;
    size_t full_length;
    char *retstring, *curs;
    size_t d_index = 0;

    length = strlen(digits);
    /* testing against both zero and length because length is unsigned */
    if (exp > 0 && (size_t) exp > length)
	length = exp;
    length += 3;		       /* the null, the (possible) sign, and the decimal */

    Dprintf("Engineering Formatted Number\n");
    Dprintf("digits: %s(%u), exp: %i, prec: %i, prefix: %i\n", digits,
	    (unsigned)length, (int)exp, precision, prefix);

    // ten extra, 'cuz of the exponent
    full_length = length + 10;
    curs = retstring = (char *)calloc(sizeof(char), full_length);
    Dprintf("length: %lu, full_length: %lu\n", length, full_length);

    // now, copy the digits into the output string, carefully

    // copy over the negative sign
    if (digits[d_index] == '-') {
	snprintf(curs++, length--, "%c", digits[d_index++]);
    }
    // copy in a prefix
    if (prefix) {
	char *nc;

	add_prefix(curs, length, base);
	nc = strchr(curs, '\0');
	length -= nc - curs;
	curs = nc;
    }
    // copy over the integer
    snprintf(curs++, length--, "%c", digits[d_index++]);
    exp--;
    // the decimal
    snprintf(curs++, length--, ".");
    Dprintf("the integers: %s\n", retstring);
    Dprintf("length: %lu, full_length: %lu\n", length, full_length);

    // now, add in the rest of the digits
    // note that the digits are already correctly rounded and I've already
    // allocated enough space, because of how I asked mpfr for the original
    // digit string.
    snprintf(curs, length + 1, "%s", &digits[d_index]);
    Dprintf("the decimals: %s\n", retstring);

    // strip off the trailing 0's
    if (-1 == precision) {
	char *period;

	zero_strip(retstring);
	/* XXX: This is a stupid hack; the idea is just to get the mpfr output
	 * to match (roughly) the double output. */
	period = strchr(retstring, '.');
	Dprintf("retstring: %s\n", retstring);
	Dprintf("period: %s\n", period);
	if (period && strlen(period) > 10) {
	    period[10] = 0;
	    *truncated_flag = 1;
	    zero_strip(retstring);
	}
    } else {
	char *period = strchr(retstring, '.') + 1;

	Dprintf("period: %s\n", period);
	if (period && (int)strlen(period) > precision) {
	    Dprintf("truncating down to precision...\n");
	    period[precision] = 0;
	    *truncated_flag = 1;
	}
    }
    // copy in an exponent
    curs = strchr(retstring, '\0');
    Dprintf("space left: %lu\n", full_length - (curs - retstring));
    snprintf(curs, full_length - (curs - retstring),
	     (base <= 10 ? "e%+ld" : "@%+ld"), (long)exp);

    return retstring;
}

/* this function removes zeros and decimals from the back end of a number,
 * and returns how many characters it stripped */
size_t zero_strip(char *num)
{
    size_t curs = strlen(num) - 1;
    size_t count = 0;

    while ('0' == num[curs]) {
	num[curs--] = '\0';
	count++;
    }
    if ('.' == num[curs]) {
	num[curs] = '\0';
	count++;
    }
    return count;
}

/* this function prints a prefix for the specified base into the specified
 * string */
void add_prefix(char *num, size_t length, int base)
{
    switch (base) {
	case 16:
	    snprintf(num, length, "0x");
	    return;
	case 10:
	    return;
	case 8:
	    snprintf(num, length, "0");
	    return;
	case 2:
	    snprintf(num, length, "0b");
	    return;
    }
}
