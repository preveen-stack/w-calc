#include "number.h"
#include "calculator.h"
#include <stdlib.h> /* for atexit() */

#ifdef HAVE_LIBMPFR
gmp_randstate_t randstate;
#endif

void numbers_cleanup(void)
{
#ifdef HAVE_LIBMPFR
    mpfr_free_cache();
#endif
}

void init_numbers(void)
{
#ifdef HAVE_LIBMPFR
    struct timeval tp;
    /* seed the random number generator */
    if (gettimeofday(&tp, NULL) != 0) {
	perror("gettimeofday");
	exit(EXIT_FAILURE);
    }
    gmp_randinit_default(randstate);
    gmp_randseed_ui(randstate, (unsigned long)(tp.tv_usec));
    mpfr_set_default_prec(1024);
#else
    srandom(time(NULL))
#endif
    atexit(numbers_cleanup);
}

int is_int(Number potential_int)
{
#ifdef HAVE_LIBMPFR
    char *str, *curs;
    mp_exp_t eptr;
    int base;

    switch (conf.output_format) {
	case HEXADECIMAL_FORMAT:
	    base = 16;
	    break;
	default:
	case DECIMAL_FORMAT:
	    base = 10;
	    break;
	case OCTAL_FORMAT:
	    base = 8;
	    break;
	case BINARY_FORMAT:
	    base = 2;
	    break;
    }
    str = mpfr_get_str(NULL, &eptr, base, 0, potential_int, GMP_RNDN);
    if (eptr < 0)
	goto not_an_int;
    curs = str + eptr;
    while (curs && *curs) {
	if (*curs != '0')
	    goto not_an_int;
	curs++;
    }
    mpfr_free_str(str);
    return 1;
not_an_int:
    mpfr_free_str(str);
    return 0;
#else
#endif
}