#ifndef WCALC_NUMBER_FORMATTING_H
#define WCALC_NUMBER_FORMATTING_H

char *num_to_str_complex(mpfr_t num, int base, int engr, int prec, int prefix,
			 char *truncated_flag);

#endif
