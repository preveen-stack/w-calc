#if HAVE_CONFIG_H
# include "config.h"
#endif

#if ! defined(HAVE_CONFIG_H) || HAVE_STRING_H
# include <string.h>
#endif
#include <stdio.h>					   /* for snprintf() */
#include <stdlib.h>					   /* for calloc() */

#include <gmp.h>
#include <mpfr.h>

#include "calculator.h"
#include "string_manip.h"
#include "number_formatting.h"

static size_t zero_strip(char *num);
static void add_prefix(char *num, size_t length, int base);
static char *engineering_formatted_number(char *digits, mp_exp_t exp,
										  int precision, int base,
										  int prefix);
static char *automatically_formatted_number(char *digits, mp_exp_t exp,
											int base, int prefix);
static char *precision_formatted_number(char *digits, mp_exp_t exp,
										int precision, int base, int prefix);

/* this function takes a number (mpfr_t) and prints it.
 * This is a blatant ripoff of mpfr's mpfr_out_str(), because it formats things
 * (sorta) the way I want them formatted, though this prints things out to a
 * string, and does all the fancy presentation stuff we've come to expect from
 * wcalc.
 */
char *num_to_str_complex(mpfr_t num, int base, int engr, int prec, int prefix)
{
	char *s, *retstr;
	mp_exp_t e;

	Dprintf("num_to_str_complex: base: %i, engr: %i, prec: %i, prefix: %i\n",
			base, engr, prec, prefix);
	if (mpfr_nan_p(num)) {
		return (char *)strdup("@NaN@");
	}
	if (mpfr_inf_p(num)) {
		if (mpfr_sgn(num) > 0) {
			return (char *)strdup("@Inf@");
		} else {
			return (char *)strdup("-@Inf@");
		}
	}
	if (mpfr_zero_p(num)) {
		if (mpfr_sgn(num) > 0) {
			return (char *)strdup("0");
		} else {
			return (char *)strdup("-0");
		}
	}

	s = mpfr_get_str(NULL, &e, base, 0, num, GMP_RNDN);
	/* s is the string, allocated big enough to hold the whole thing
	 * e is the number of integers (the exponent) if positive
	 *
	 * Now, if there's odd formatting involved, make mpfr do the rounding,
	 * so we know it's "correct":
	 */
	if (prec > -1) {
		if (engr == 0) {
			size_t significant_figures = 0;

			Dprintf("prec > -1 && engr == 0\n");
			/*printf("e: %li\n", (long)e);
			 * printf("s: %s\n", s);
			 * printf("prec: %i\n", prec); */
			significant_figures = ((e > 0) ? e : 0) + prec;
			if (significant_figures < 2) {	/* why is this the minimum? */
				s = mpfr_get_str(s, &e, base, 2, num, GMP_RNDN);
				if (s[1] > '4') {	   /* XXX: LAME! */
					unsigned foo;

					foo = s[0] - '0';
					foo++;
					snprintf(s, 3, "%u", foo);
					e++;
				}
			} else {
				s = mpfr_get_str(s, &e, base, significant_figures, num,
								 GMP_RNDN);
			}
		} else {
			s = mpfr_get_str(NULL, &e, base, 1 + prec, num, GMP_RNDN);
		}
	}
	Dprintf("post-mpfr e: %li s: %s\n", (long int)e, s);
	if (engr != 0) {
		retstr = engineering_formatted_number(s, e, prec, base, prefix);
	} else if (-1 == prec) {
		retstr = automatically_formatted_number(s, e, base, prefix);
	} else {
		retstr = precision_formatted_number(s, e, prec, base, prefix);
	}
	mpfr_free_str(s);
	return retstr;
}

char *precision_formatted_number(char *digits, mp_exp_t exp, int precision,
								 int base, int prefix)
{
	size_t length = strlen(digits) + 2;	// the null and the decimal
	size_t full_length;
	size_t decimal_count = 0;
	size_t print_limit;
	char *retstring, *curs, *dcurs = digits;

	if (length < precision + 3) {	   // leading zero, decimal, and null
		length = precision + 3;
	}
	Dprintf("Precision Formatted Number\n");
	Dprintf("digits: %s(%u), exp: %i, base: %i, prefix: %i, precision: %i\n",
			digits, (unsigned)length, (int)exp, base, prefix, precision);

	// ten extra, 'cuz of the *possible* exponent
	full_length = length + 10;
	curs = retstring = (char *)calloc(sizeof(char), full_length);
	Dprintf("length: %lu, full_length: %lu\n", length, full_length);
	// now, copy the digits into the output string, carefully

	// copy over the negative sign
	if (*dcurs == '-') {
		snprintf(curs++, length--, "%c", *dcurs++);
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
	if (exp > 0) {
		snprintf(curs++, length--, "%c", *dcurs++);
		exp--;						   // leading digit
		while (exp > 0) {
			snprintf(curs++, length--, "%c", *dcurs++);
			exp--;
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
		while (exp < 0 && decimal_count <= precision) {
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
		snprintf(curs, print_limit, "%s", dcurs);
	}

	return retstring;
}

char *automatically_formatted_number(char *digits, mp_exp_t exp, int base,
									 int prefix)
{
	size_t length = strlen(digits) + 2;	// the null and the decimal
	size_t full_length;
	size_t decimal_count = 0;
	size_t printed;
	char *retstring, *curs, *dcurs = digits;

	Dprintf("Automatically Formatted Number\n");
	Dprintf("digits: %s(%u), exp: %i, base: %i, prefix: %i\n", digits,
			(unsigned)length, (int)exp, base, prefix);

	// ten extra, 'cuz of the *possible* exponent
	full_length = length + 10;
	curs = retstring = (char *)calloc(sizeof(char), full_length);
	Dprintf("length: %lu, full_length: %lu\n", length, full_length);

	// now, copy the digits into the output string, carefully

	// copy over the negative sign
	if (*dcurs == '-') {
		snprintf(curs++, length--, "%c", *dcurs++);
	}
	// copy in a prefix
	if (prefix) {
		char *nc;

		add_prefix(curs, length, base);
		nc = strchr(curs, '\0');
		length -= nc - curs;
		curs = nc;
	}
	// copy over the integers
	if (exp > 0) {
		snprintf(curs++, length--, "%c", *dcurs++);
		exp--;						   // leading digit
		while (exp > 0) {
			snprintf(curs++, length--, "%c", *dcurs++);
			exp--;
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
	while (exp < 0) {
		snprintf(curs++, length--, "0");
		exp++;
		decimal_count++;
	}
	// the rest of the mantissa (the decimals)

	// this variable exists because snprintf's return value is unreliable.
	// and can be larger than the number of digits printed
	printed = ((length - 1 < strlen(dcurs)) ? length - 1 : strlen(dcurs));
	snprintf(curs, length, "%s", dcurs);
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

char *engineering_formatted_number(char *digits, mp_exp_t exp, int precision,
								   int base, int prefix)
{
	size_t length = strlen(digits) + 2;	// the null and the decimal
	size_t full_length;
	char *retstring, *curs, *dcurs = digits;

	Dprintf("Engineering Formatted Number\n");
	Dprintf("digits: %s(%u), exp: %i, prec: %i, prefix: %i\n", digits,
			(unsigned)length, (int)exp, precision, prefix);

	// ten extra, 'cuz of the exponent
	full_length = length + 10;
	curs = retstring = (char *)calloc(sizeof(char), full_length);
	Dprintf("length: %lu, full_length: %lu\n", length, full_length);

	// now, copy the digits into the output string, carefully

	// copy over the negative sign
	if (*dcurs == '-') {
		snprintf(curs++, length--, "%c", *dcurs++);
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
	snprintf(curs++, length--, "%c", *dcurs++);
	exp--;
	// the decimal
	snprintf(curs++, length--, ".");
	Dprintf("the integers: %s\n", retstring);
	Dprintf("length: %lu, full_length: %lu\n", length, full_length);

	// now, add in the rest of the digits
	// note that the digits are already correctly rounded and I've already
	// allocated enough space, because of how I asked mpfr for the original
	// digit string.
	snprintf(curs, length, "%s", dcurs);
	Dprintf("the decimals: %s\n", retstring);

	// strip off the trailing 0's
	if (-1 == precision) {
		zero_strip(retstring);
	}
	// copy in an exponent
	curs = strchr(retstring, '\0');
	Dprintf("space left: %lu\n", full_length - (curs - retstring));
	snprintf(curs, full_length - (curs - retstring),
			 (base <= 10 ? "e%ld" : "@%ld"), (long)exp);

	return retstring;
}

/* this function removes zeros and decimals from the back end of a number,
 * and returns how many it stripped */
size_t zero_strip(char *num)
{
	char *curs = strchr(num, '\0');
	size_t count = 0;

	curs--;
	while ('0' == *curs) {
		*curs-- = '\0';
		count++;
	}
	if ('.' == *curs) {
		*curs = '\0';
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
