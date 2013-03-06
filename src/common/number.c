#include "number.h"
#include "calculator.h"
#include <sys/time.h> /* for gettimeofday() */
#include <stdio.h> /* for perror() */
#include <string.h> /* for strlen() */
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
    srandom(time(NULL));
#endif
    atexit(numbers_cleanup);
}

int is_int(const Number potential_int)
{
    Number temp;
    int ret;
    num_init(temp);
    num_trunc(temp, potential_int);
    ret = num_is_equal(temp, potential_int);
    num_free(temp);
    return ret;
}

#ifndef HAVE_LIBMPFR
#include <errno.h>
#include <math.h> /* for HUGE_VAL, according to C89 */

void num_init(Number n)
{
    n->nan = 1;
}
void num_init_set(Number n1, const Number n2)
{
    num_init(n1);
    memcpy(n1, n2, sizeof(struct numberstruct));
}
void num_init_set_d(Number n, const double d)
{
    num_init(n);
    n->value = d;
    n->nan = 0;
}
void num_init_set_ui(Number n, const unsigned int ui)
{
    num_init(n);
    n->value = (double)ui;
    n->nan = 0;
}
int num_init_set_str(Number n, const char * str, const int base)
{
    num_init(n);
    return num_set_str(n, str, base);
}
void num_set(Number n1, const Number n2)
{
    memcpy(n1, n2, sizeof(struct numberstruct));
}
void num_set_d(Number n, const double d)
{
    n->value = d;
    n->nan = 0;
}
void num_set_ui(Number n, const unsigned int ui)
{
    n->value = (double)ui;
    n->nan = 0;
}
int num_set_str(Number n, const char * str, const int base)
{
    char * endptr;

    Dprintf("base = %i, %c%c\n", base, str[0], str[1]);
    errno = 0;
    n->value = strtod(str, &endptr);
    if (endptr == str) {
	return -1;
    }
    if (errno == ERANGE && (n->value == HUGE_VAL || n->value == 0.0)) {
	n->nan = 1;
	return -1;
    }
    n->nan = 0;
    return 0;
}
void num_set_nan(Number n)
{
    n->value = 0.0;
    n->nan = 1;
}
void num_mul(Number ret, const Number n1, const Number n2)
{
    if (n1->nan == 0 && n2->nan == 0) {
	num_set_d(ret, n1->value * n2->value);
    } else {
	num_set_nan(ret);
    }
}
void num_mul_si(Number ret, const Number n, const int si)
{
    if (n->nan == 0) {
	num_set_d(ret, n->value * si);
    } else {
	num_set_nan(ret);
    }
}
void num_mul_ui(Number ret, const Number n, const unsigned int ui)
{
    if (n->nan == 0) {
	num_set_d(ret, n->value * ui);
    } else {
	num_set_nan(ret);
    }
}
void num_sqr(Number ret, const Number n)
{
    num_mul(ret, n, n);
}
void num_sqrt(Number ret, const Number n)
{
    if (n->nan == 0 && n->value >= 0.0) {
	num_set_d(ret, sqrt(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_cbrt(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, cbrt(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_pow(Number ret, const Number n, const Number exp)
{
    if (n->nan == 0 && exp->nan == 0) {
	errno = 0;
	num_set_d(ret, pow(n->value, exp->value));
	if (errno == EDOM) {
	    num_set_nan(ret);
	}
    } else {
	num_set_nan(ret);
    }
}
void num_pow_si(Number ret, const Number n, const int exp)
{
    if (n->nan == 0) {
	errno = 0;
	num_set_d(ret, pow(n->value, exp));
	if (errno == EDOM) {
	    num_set_nan(ret);
	}
    } else {
	num_set_nan(ret);
    }
}
void num_exp(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, exp(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_factorial(Number ret, unsigned int n)
{
    ret->value = (double)n;
    n--;
    while (n > 1 && ret->value < DBL_MAX) {
	ret->value *= n;
	n--;
    }
    if (ret->value < DBL_MAX) {
	ret->nan = 0;
    } else {
	num_set_nan(ret);
    }
}
void num_log10(Number ret, const Number n)
{
    if (n->nan == 0 && n->value > 0.0) {
	num_set_d(ret, log10(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_log2(Number ret, const Number n)
{
    if (n->nan == 0 && n->value > 0.0) {
	num_set_d(ret, log10(n->value)/log10(2.0));
    } else {
	num_set_nan(ret);
    }
}
void num_log(Number ret, const Number n)
{
    if (n->nan == 0 && n->value > 0.0) {
	num_set_d(ret, log(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_sin(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, sin(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_cos(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, cos(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_tan(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, tan(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_asin(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, asin(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_acos(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, acos(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_atan(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, atan(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_sinh(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, sinh(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_cosh(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, cosh(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_tanh(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, tanh(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_asinh(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, asinh(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_acosh(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, acosh(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_atanh(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, atanh(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_acoth(Number ret, const Number n)
{
    if (n->nan == 0 && !num_is_zero(n)) {
	num_set_d(ret, atanh(1.0/n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_asech(Number ret, const Number n)
{
    if (n->nan == 0 && !num_is_zero(n)) {
	num_set_d(ret, acosh(1.0/n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_acsch(Number ret, const Number n)
{
    if (n->nan == 0 && !num_is_zero(n)) {
	num_set_d(ret, asinh(1.0/n->value));
    } else {
	num_set_nan(ret);
    }
}
double num_get_d(const Number n)
{
    return n->value;
}
unsigned int num_get_ui(const Number n)
{
    return (unsigned int) n->value;
}
char * num_get_str(char *str, num_exp_t *expptr, const int b, const size_t n, const Number op)
{
    char * dec;
    if (!str) str = malloc(1024);
    sprintf(str, "%.1000g",op->value);
    printf("the str: %s\n", str);
    dec = strchr(str, conf.dec_delimiter);
    if (dec == NULL) {
	*expptr = strlen(str);
    }
    if (str[0] == -1) {
	*expptr -= 1;
    }
    strstrip(conf.dec_delimiter, str);
    printf("the str (2): %s\n", str);
    return str;
}
num_prec_t num_get_default_prec()
{
    return DBL_MANT_DIG;
}
void num_add(Number ret, const Number n1, const Number n2)
{
    if (n1->nan == 0 && n2->nan == 0) {
	num_set_d(ret, n1->value + n2->value);
    } else {
	num_set_nan(ret);
    }
}
void num_sub(Number ret, const Number n1, const Number n2)
{
    if (n1->nan == 0 && n2->nan == 0) {
	num_set_d(ret, n1->value - n2->value);
    } else {
	num_set_nan(ret);
    }
}
void num_add_ui(Number ret, const Number n, const unsigned int ui)
{
    if (n->nan == 0) {
	num_set_d(ret, n->value + ui);
    } else {
	num_set_nan(ret);
    }
}
void num_sub_ui(Number ret, const Number n, const unsigned int ui)
{
    if (n->nan == 0) {
	num_set_d(ret, n->value - ui);
    } else {
	num_set_nan(ret);
    }
}
void num_div(Number ret, const Number n1, const Number n2)
{
    if (n1->nan == 0 && n2->nan == 0 && n2->value != 0.0) {
	num_set_d(ret, n1->value / n2->value);
    } else {
	num_set_nan(ret);
    }
}
void num_div_ui(Number ret, const Number n, const unsigned int ui)
{
    if (n->nan == 0 && ui != 0) {
	num_set_d(ret, n->value / ui);
    } else {
	num_set_nan(ret);
    }
}
void num_rint(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, rint(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_rintz(Number ret, const Number n)
{
    if (n->nan == 0) {
	if (n->value < 0.0) {
	    num_set_d(ret, ceil(n->value));
	} else {
	    num_set_d(ret, floor(n->value));
	}
    } else {
	num_set_nan(ret);
    }
}
void num_floor(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, floor(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_ceil(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, ceil(n->value));
    } else {
	num_set_nan(ret);
    }
}
int num_random(Number n)
{
    num_set_d(n, random()/(double)RAND_MAX);
    return 0;
}
void num_sinc(Number ret, const Number n)
{
    if (n->nan == 0) {
	if (num_is_zero(n)) {
	    num_set_d(ret, 1);
	} else {
	    num_sin(ret, n);
	    num_div(ret, ret, n);
	}
    } else {
	num_set_nan(ret);
    }
}
void num_const_pi(Number n)
{
    num_set_d(n, atan(1)*4.0); // as accurate as the underlying math library can do
}
void num_const_euler(Number n)
{
    num_set_d(n, 2.718281828459045235360287471352662497757247093699959574966967627724076630353547594571382178525166427427466391932003059921817413596629043572900334295260595630);
}
void num_const_catalan(Number n)
{
    num_set_d(n, 0.91596559417721901505460351493238411077414937428167213426649811962176301977625476947935651292611510624857442261919619957903589880332585905943159473748115840);
}
void num_out_str(FILE *fd, const int base, const Number n)
{
    if (n->nan == 1) {
	fprintf(fd,"@NaN@");
	return;
    }
    switch (base) {
	case 10:
	    fprintf(fd, "%g", n->value); break;
	case 16:
	    fprintf(fd, "%#x", (unsigned int)(n->value)); break;
	case 8:
	    fprintf(fd, "%#o", (unsigned int)(n->value)); break;
	default:
	    fprintf(fd, "-->oops!<--");
    }
}
int num_is_nan(const Number n)
{
    return n->nan;
}
int num_is_inf(const Number n)
{
    return (n->nan == 0 && isinf(n->value));
}
int num_is_zero(const Number n)
{
    return (n->nan == 0 && n->value == 0.0);
}
int num_is_equal(const Number n1, const Number n2)
{
    return (n1->nan == 0 && n2->nan == 0 && n1->value == n2->value);
}
int num_is_greater(const Number n1, const Number n2)
{
    return (n1->nan == 0 && n2->nan == 0 && n1->value > n2->value);
}
int num_is_greaterequal(const Number n1, const Number n2)
{
    return (n1->nan == 0 && n2->nan == 0 && n1->value >= n2->value);
}
int num_is_less(const Number n1, const Number n2)
{
    return (n1->nan == 0 && n2->nan == 0 && n1->value < n2->value);
}
int num_is_lessequal(const Number n1, const Number n2)
{
    return (n1->nan == 0 && n2->nan == 0 && n1->value <= n2->value);
}
int num_sign(const Number n)
{
    if (n->value > 0.0) {
	return 1;
    } else if (n->value < 0.0) {
	return -1;
    } else {
	return 0;
    }
}
void num_neg(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, n->value*-1);
    } else {
	num_set_nan(ret);
    }
}
void num_abs(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, fabs(n->value));
    } else {
	num_set_nan(ret);
    }
}
int num_cmp_si(const Number n, const int si)
{
    if (n->value > si) {
	return 1;
    } else if (n->value < si) {
	return -1;
    } else {
	return 0;
    }
}
void num_free(Number n)
{
}
void num_cot(Number ret, const Number n)
{
    num_tan(ret, n);
    if (ret->nan == 0 && !num_is_zero(ret)) {
	num_set_d(ret, 1.0/ret->value);
    }
}
void num_sec(Number ret, const Number n)
{
    num_cos(ret, n);
    if (ret->nan == 0 && !num_is_zero(ret)) {
	num_set_d(ret, 1.0/ret->value);
    }
}
void num_csc(Number ret, const Number n)
{
    num_sin(ret, n);
    if (ret->nan == 0 && !num_is_zero(ret)) {
	num_set_d(ret, 1.0/ret->value);
    }
}
void num_coth(Number ret, const Number n)
{
    num_tanh(ret, n);
    if (ret->nan == 0 && !num_is_zero(ret)) {
	num_set_d(ret, 1.0/ret->value);
    }
}
void num_sech(Number ret, const Number n)
{
    num_cosh(ret, n);
    if (ret->nan == 0 && !num_is_zero(ret)) {
	num_set_d(ret, 1.0/ret->value);
    }
}
void num_csch(Number ret, const Number n)
{
    num_sinh(ret, n);
    if (ret->nan == 0 && !num_is_zero(ret)) {
	num_set_d(ret, 1.0/ret->value);
    }
}
void num_comp(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, (((int)(n->value))*-1)-1);
    } else {
	num_set_nan(ret);
    }
}
void num_bnot(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_ui(ret, ~num_get_ui(n));
    } else {
	num_set_nan(ret);
    }
}
void num_band(Number ret, const Number n1, const Number n2)
{
    if (n1->nan == 0 && n2->nan == 0) {
	num_set_ui(ret, num_get_ui(n1) & num_get_ui(n2));
    } else {
	num_set_nan(ret);
    }
}
void num_bxor(Number ret, const Number n1, const Number n2)
{
    if (n1->nan == 0 && n2->nan == 0) {
	num_set_ui(ret, num_get_ui(n1) ^ num_get_ui(n2));
    } else {
	num_set_nan(ret);
    }
}
void num_bor(Number ret, const Number n1, const Number n2)
{
    if (n1->nan == 0 && n2->nan == 0) {
	num_set_ui(ret, num_get_ui(n1) | num_get_ui(n2));
    } else {
	num_set_nan(ret);
    }
}
void num_trunc(Number ret, const Number n)
{
    if (n->nan == 0) {
	num_set_d(ret, trunc(n->value));
    } else {
	num_set_nan(ret);
    }
}
void num_unimplemented()
{
    fprintf(stderr,"This function is unimplemented when not using libmpfr.\n");
}
#endif
