#ifndef WCALC_NUMBER_FORMATTING_H
#define WCALC_NUMBER_FORMATTING_H

char *num_to_str_complex(mpfr_t num, int base, int engr, int prec,
						 int prefix);
char *engineering_formatted_number(char *digits, mp_exp_t exp, int precision,
								   int base, int prefix);
char *automatically_formatted_number(char *digits, mp_exp_t exp, int base,
									 int prefix);
char *precision_formatted_number(char *digits, mp_exp_t exp, int precision, int base, int prefix);

#endif
