#ifndef WCALC_NUMBER_FORMATTING_H
#define WCALC_NUMBER_FORMATTING_H

char *num_to_str_complex(const mpfr_t num, const int base, const int engr,
			 const int prec, const int prefix,
			 char *truncated_flag);

#endif
