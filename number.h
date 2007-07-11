#ifndef NUMBER_H
#define NUMBER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define HAVE_LIBMPFR
#endif

#ifdef HAVE_LIBMPFR

#if HAVE_LIMITS_H
# include <limits.h> /* MPFR uses ULONG_MAX on some systems */
#endif

#include <gmp.h>
#include <mpfr.h>

#define Number mpfr_t
#define NUM_PREC_MIN MPFR_PREC_MIN
#define NUM_PREC_MAX MPFR_PREC_MAX

#define num_init(n) mpfr_init(n)
#define num_init_set(n1,n2) mpfr_init_set((n1), (n2), GMP_RNDN)
#define num_init_set_d(n,d) mpfr_init_set_d((n), (d), GMP_RNDN)
#define num_init_set_ui(n,ui) mpfr_init_set_ui((n), (ui), GMP_RNDN)
#define num_init_set_str(n,str,base) mpfr_init_set_str((n), (str), (base), GMP_RNDN)

#define num_free(n) mpfr_clear(n)

#define num_get_d(n) mpfr_get_d((n), GMP_RNDN)
#define num_get_ui(n) mpfr_get_ui((n), GMP_RNDN)

#define num_set(n1,n2) mpfr_set((n1), (n2), GMP_RNDN)
#define num_set_d(n,d) mpfr_set_d((n), (d), GMP_RNDN)
#define num_set_ui(n,ui) mpfr_set_ui((n), (ui), GMP_RNDN)
#define num_set_str(n,str,base) mpfr_set_str((n), (str), (base), GMP_RNDN)
#define num_set_nan(n) mpfr_set_nan(n)

#define num_add(ret,n1,n2) mpfr_add((ret), (n1), (n2), GMP_RNDN)
#define num_sub(ret,n1,n2) mpfr_sub((ret), (n1), (n2), GMP_RNDN)
#define num_add_ui(ret,n,ui) mpfr_add_ui((ret), (n), (ui), GMP_RNDN)
#define num_sub_ui(ret,n,ui) mpfr_sub_ui((ret), (n), (ui), GMP_RNDN)

#define num_mul(ret,n1,n2) mpfr_mul((ret), (n1), (n2), GMP_RNDN)
#define num_mul_si(ret,n,si) mpfr_mul_si((ret), (n), (si), GMP_RNDN)
#define num_mul_ui(ret,n,ui) mpfr_mul_ui((ret), (n), (ui), GMP_RNDN)
#define num_sqr(ret,n) mpfr_sqr((ret), (n), GMP_RNDN)
#define num_sqrt(ret,n) mpfr_sqrt((ret), (n), GMP_RNDN)
#define num_cbrt(ret,n) mpfr_cbrt((ret), (n), GMP_RNDN)

#define num_div(ret,n1,n2) mpfr_div((ret), (n1), (n2), GMP_RNDN)
#define num_div_ui(ret,n,ui) mpfr_div_ui((ret), (n), (ui), GMP_RNDN)

#define num_rint(ret,n) mpfr_rint((ret), (n), GMP_RNDN)
#define num_rintz(ret,n) mpfr_rint((ret), (n), GMP_RNDZ)
#define num_floor(ret,n) mpfr_floor((ret), (n))
#define num_ceil(ret,n) mpfr_ceil((ret), (n))

#define num_pow(ret,n,exp) mpfr_pow((ret), (n), (exp), GMP_RNDN)
#define num_pow_si(ret,n,exp) mpfr_pow_si((ret), (n), (exp), GMP_RNDN)
#define num_exp(ret,n) mpfr_exp((ret), (n), GMP_RNDN)
#define num_factorial(ret,op) mpfr_fac_ui((ret), (op), GMP_RNDN)
#define num_log10(ret,n) mpfr_log10((ret), (n), GMP_RNDN)
#define num_log2(ret,n) mpfr_log2((ret), (n), GMP_RNDN)
#define num_log(ret,n) mpfr_log((ret), (n), GMP_RNDN)

#define num_sin(ret,n) mpfr_sin((ret), (n), GMP_RNDN)
#define num_cos(ret,n) mpfr_cos((ret), (n), GMP_RNDN)
#define num_tan(ret,n) mpfr_tan((ret), (n), GMP_RNDN)
#define num_asin(ret,n) mpfr_asin((ret), (n), GMP_RNDN)
#define num_acos(ret,n) mpfr_acos((ret), (n), GMP_RNDN)
#define num_atan(ret,n) mpfr_atan((ret), (n), GMP_RNDN)
#define num_sinh(ret,n) mpfr_sinh((ret), (n), GMP_RNDN)
#define num_cosh(ret,n) mpfr_cosh((ret), (n), GMP_RNDN)
#define num_tanh(ret,n) mpfr_tanh((ret), (n), GMP_RNDN)
#define num_asinh(ret,n) mpfr_asinh((ret), (n), GMP_RNDN)
#define num_acosh(ret,n) mpfr_acosh((ret), (n), GMP_RNDN)
#define num_atanh(ret,n) mpfr_atanh((ret), (n), GMP_RNDN)
#define num_acoth(ret,n) do { \
    mpfr_pow_si((n), (n), -1, GMP_RNDN); \
    mpfr_atanh((ret), (n), GMP_RNDN); \
} while (0)
#define num_asech(ret,n) do { \
    mpfr_pow_si((n), (n), -1, GMP_RNDN); \
    mpfr_acosh((ret), (n), GMP_RNDN); \
} while (0)
#define num_acsch(ret,n) do { \
    mpfr_pow_si((n), (n), -1, GMP_RNDN); \
    mpfr_asinh((ret), (n), GMP_RNDN); \
} while (0)

#define num_random(n) mpfr_urandomb((n), randstate)
#define num_zeta(ret,n) mpfr_zeta((ret), (n), GMP_RNDN);
#define num_sinc(ret,n) do { \
    if(mpfr_zero_p((n))) { \
	mpfr_set_ui((ret), 1, GMP_RNDN); \
    } else { \
	mpfr_sin((ret), (n), GMP_RNDN); \
	mpfr_div((ret), (ret), (n), GMP_RNDN); \
    } \
} while (0)

#define num_const_pi(n) mpfr_const_pi((n), GMP_RNDN)
#define num_const_euler(n) mpfr_const_euler((n), GMP_RNDN)

#define num_out_str(fd,base,n) mpfr_out_str((fd), (base), 0, (n), GMP_RNDN)
#define num_is_nan(n) mpfr_nan_p(n)
#define num_is_inf(n) mpfr_inf_p(n)
#define num_is_zero(n) mpfr_zero_p(n)
#define num_is_equal(n1,n2) mpfr_equal_p((n1), (n2))
#define num_is_greater(n1,n2) mpfr_greater_p((n1), (n2))
#define num_is_greaterequal(n1,n2) mpfr_greaterequal_p((n1), (n2))
#define num_is_less(n1,n2) mpfr_less_p((n1), (n2))
#define num_is_lessequal(n1,n2) mpfr_lessequal_p((n1), (n2))
#define num_sign(n) mpfr_sgn(n)
#define num_neg(ret,n) mpfr_neg((ret), (n), GMP_RNDN)
#define num_abs(ret,n) mpfr_abs((ret), (n), GMP_RNDN)

#define num_cmp_si(n,si) mpfr_cmp_si((n), (si))

#define num_get_default_prec() mpfr_get_default_prec()
#define num_set_default_prec(prec) mpfr_set_default_prec(prec)

#ifdef HAVE_MPFR_22
# define num_const_catalan(n) mpfr_const_catalan((n), GMP_RNDN)
# define num_cot(ret,n) mpfr_cot((ret), (n), GMP_RNDN)
# define num_sec(ret,n) mpfr_sec((ret), (n), GMP_RNDN)
# define num_csc(ret,n) mpfr_csc((ret), (n), GMP_RNDN)
# define num_coth(ret,n) mpfr_coth((ret), (n), GMP_RNDN)
# define num_sech(ret,n) mpfr_sech((ret), (n), GMP_RNDN)
# define num_csch(ret,n) mpfr_csch((ret), (n), GMP_RNDN)
# define num_eint(ret,n) mpfr_eint((ret), (n), GMP_RNDN)
# define num_gamma(ret,n) mpfr_gamma((ret), (n), GMP_RNDN)
# define num_lngamma(ret,n) mpfr_lngamma((ret), (n), GMP_RNDN)
#else
# define num_const_catalan(n) num_set_str((n), W_CATALAN, 0)
# define num_cot(ret,n) do { \
    mpfr_tan((ret), (n), GMP_RNDN); \
    mpfr_pow_si((ret), (n), -1, GMP_RNDN); \
} while (0)
# define num_sec(ret,n) do { \
    mpfr_cos((ret), (n), GMP_RNDN); \
    mpfr_pow_si((ret), (n), -1, GMP_RNDN); \
} while (0)
# define num_csc(ret,n) do { \
    mpfr_sin((ret), (n), GMP_RNDN); \
    mpfr_pow_si((ret), (n), -1, GMP_RNDN); \
} while (0)
# define num_coth(ret,n) do { \
    mpfr_tanh((ret), (n), GMP_RNDN); \
    mpfr_pow_si((ret), (n), -1, GMP_RNDN); \
} while (0)
# define num_sech(ret,n) do { \
    mpfr_cosh((ret), (n), GMP_RNDN); \
    mpfr_pow_si((ret), (n), -1, GMP_RNDN); \
} while (0)
# define num_csch(ret,n) do { \
    mpfr_sinh((ret), (n), GMP_RNDN); \
    mpfr_pow_si((ret), (n), -1, GMP_RNDN); \
} while (0)
#endif

#define num_bor(ret,n1,n2) do { \
    mpz_t intfirst, intsecond, intoutput; \
    mpz_init(intfirst); \
    mpz_init(intsecond); \
    mpz_init(intoutput); \
    mpfr_get_z(intfirst, (n1), GMP_RNDN); \
    mpfr_get_z(intsecond, (n2), GMP_RNDN); \
    mpz_ior(intoutput, intfirst, intsecond); \
    mpfr_set_z((ret), intoutput, GMP_RNDN); \
    mpz_clear(intfirst); \
    mpz_clear(intsecond); \
    mpz_clear(intoutput); \
} while (0)
#define num_band(ret,n1,n2) do { \
    mpz_t intfirst, intsecond, intoutput; \
    mpz_init(intfirst); \
    mpz_init(intsecond); \
    mpz_init(intoutput); \
    mpfr_get_z(intfirst, (n1), GMP_RNDN); \
    mpfr_get_z(intsecond, (n2), GMP_RNDN); \
    mpz_and(intoutput, intfirst, intsecond); \
    mpfr_set_z((ret), intoutput, GMP_RNDN); \
    mpz_clear(intfirst); \
    mpz_clear(intsecond); \
    mpz_clear(intoutput); \
} while (0)
#define num_bxor(ret,n1,n2) do { \
    mpz_t intfirst, intsecond, intoutput; \
    mpz_init(intfirst); \
    mpz_init(intsecond); \
    mpz_init(intoutput); \
    mpfr_get_z(intfirst, (n1), GMP_RNDN); \
    mpfr_get_z(intsecond, (n2), GMP_RNDN); \
    mpz_xor(intoutput, intfirst, intsecond); \
    mpfr_set_z((ret), intoutput, GMP_RNDN); \
    mpz_clear(intfirst); \
    mpz_clear(intsecond); \
    mpz_clear(intoutput); \
} while (0)
#define num_comp(ret,n) do { \
    mpz_t integer, intoutput; \
    mpz_init(integer); \
    mpz_init(intoutput); \
    mpfr_get_z(integer, (n), GMP_RNDN); \
    mpz_com(intoutput, integer); \
    mpfr_set_z((ret), intoutput, GMP_RNDN); \
    mpz_clear(integer); \
    mpz_clear(intoutput); \
} while (0)
/* this is because binary not doesn't make sense when your integer can be
 * arbitrarily big */
#ifdef _MPFR_H_HAVE_INTMAX_T
# define num_bnot(ret,n) mpfr_set_uj((ret),~mpfr_get_uj((n),GMP_RNDN),GMP_RNDN)
#else
# define num_bnot(ret,n) mpfr_set_ui((ret),~mpfr_get_ui((n),GMP_RNDN),GMP_RNDN)
#endif

#else
typedef struct numberstruct Number;

void num_free(Number);
double num_get_d(Number);
#endif

void init_numbers(void);
int is_int(Number);

#endif /* NUMBER_H */
