#ifndef NUMBER_H
#define NUMBER_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#else
# ifndef HAVE_LIBMPFR
#  define HAVE_LIBMPFR
# endif
#endif

#ifdef HAVE_LIBMPFR

# if HAVE_LIMITS_H
#  include <limits.h> /* MPFR uses ULONG_MAX on some systems */
# endif

# ifndef S_SPLINT_S /* splint barfs on the gmp.h header */
#  include <gmp.h>
#  include <mpfr.h>
# endif

# define Number       mpfr_t
# define NUM_PREC_MIN MPFR_PREC_MIN
# define NUM_PREC_MAX MPFR_PREC_MAX

# define num_init(n)                    mpfr_init(n)
# define num_init_set(n1, n2)           mpfr_init_set((n1), (n2), GMP_RNDN)
# define num_init_set_d(n, d)           mpfr_init_set_d((n), (d), GMP_RNDN)
# define num_init_set_ui(n, ui)         mpfr_init_set_ui((n), (ui), GMP_RNDN)
# define num_init_set_str(n, str, base) mpfr_init_set_str((n), (str), (base), GMP_RNDN)

# define num_free(n) mpfr_clear(n)

typedef mp_exp_t num_exp_t;
# define num_get_d(n)               mpfr_get_d((n), GMP_RNDN)
# define num_get_ui(n)              mpfr_get_ui((n), GMP_RNDN)
# define num_get_str(a, b, c, d, e) mpfr_get_str(a, b, c, d, e, GMP_RNDN)
# define num_free_str(a)            mpfr_free_str(a)

# define num_snprintf(s, n, fmt, ...) mpfr_snprintf((s), (n), (fmt), ## __VA_ARGS__)
# define num_fprintf(f, fmt, ...)     mpfr_fprintf((f), (fmt), ## __VA_ARGS__)

# define num_set(n1, n2)           mpfr_set((n1), (n2), GMP_RNDN)
# define num_set_d(n, d)           mpfr_set_d((n), (d), GMP_RNDN)
# define num_set_ui(n, ui)         mpfr_set_ui((n), (ui), GMP_RNDN)
# define num_set_str(n, str, base) mpfr_set_str((n), (str), (base), GMP_RNDN)
# define num_set_nan(n)            mpfr_set_nan(n)
# define num_set_inf(n, s)         mpfr_set_inf(n, s)

# define num_add(ret, n1, n2)   mpfr_add((ret), (n1), (n2), GMP_RNDN)
# define num_sub(ret, n1, n2)   mpfr_sub((ret), (n1), (n2), GMP_RNDN)
# define num_add_ui(ret, n, ui) mpfr_add_ui((ret), (n), (ui), GMP_RNDN)
# define num_sub_ui(ret, n, ui) mpfr_sub_ui((ret), (n), (ui), GMP_RNDN)

# define num_mul(ret, n1, n2)   mpfr_mul((ret), (n1), (n2), GMP_RNDN)
# define num_mul_si(ret, n, si) mpfr_mul_si((ret), (n), (si), GMP_RNDN)
# define num_mul_ui(ret, n, ui) mpfr_mul_ui((ret), (n), (ui), GMP_RNDN)
# define num_mul_d(ret, n, d)   mpfr_mul_d((ret), (n), (d), GMP_RNDN)
# define num_sqr(ret, n)        mpfr_sqr((ret), (n), GMP_RNDN)
# define num_sqrt(ret, n)       mpfr_sqrt((ret), (n), GMP_RNDN)
# define num_cbrt(ret, n)       mpfr_cbrt((ret), (n), GMP_RNDN)

# define num_div(ret, n1, n2)   mpfr_div((ret), (n1), (n2), GMP_RNDN)
# define num_div_ui(ret, n, ui) mpfr_div_ui((ret), (n), (ui), GMP_RNDN)

# define num_modf(ipart, fpart, n) mpfr_modf((ipart), (fpart), (n), GMP_RNDN)
# define num_rint(ret, n)  mpfr_rint((ret), (n), GMP_RNDN)
# define num_rintz(ret, n) mpfr_rint((ret), (n), GMP_RNDZ)
# define num_floor(ret, n) mpfr_floor((ret), (n))
# define num_ceil(ret, n)  mpfr_ceil((ret), (n))

# define num_pow(ret, n, exp)    mpfr_pow((ret), (n), (exp), GMP_RNDN)
# define num_pow_si(ret, n, exp) mpfr_pow_si((ret), (n), (exp), GMP_RNDN)
# define num_exp(ret, n)         mpfr_exp((ret), (n), GMP_RNDN)
# define num_factorial(ret, op)  mpfr_fac_ui((ret), (op), GMP_RNDN)
# define num_log10(ret, n)       mpfr_log10((ret), (n), GMP_RNDN)
# define num_log2(ret, n)        mpfr_log2((ret), (n), GMP_RNDN)
# define num_log(ret, n)         mpfr_log((ret), (n), GMP_RNDN)

# define num_sin(ret, n)   mpfr_sin((ret), (n), GMP_RNDN)
# define num_cos(ret, n)   mpfr_cos((ret), (n), GMP_RNDN)
# define num_tan(ret, n)   mpfr_tan((ret), (n), GMP_RNDN)
# define num_asin(ret, n)  mpfr_asin((ret), (n), GMP_RNDN)
# define num_acos(ret, n)  mpfr_acos((ret), (n), GMP_RNDN)
# define num_atan(ret, n)  mpfr_atan((ret), (n), GMP_RNDN)
# define num_sinh(ret, n)  mpfr_sinh((ret), (n), GMP_RNDN)
# define num_cosh(ret, n)  mpfr_cosh((ret), (n), GMP_RNDN)
# define num_tanh(ret, n)  mpfr_tanh((ret), (n), GMP_RNDN)
# define num_asinh(ret, n) mpfr_asinh((ret), (n), GMP_RNDN)
# define num_acosh(ret, n) mpfr_acosh((ret), (n), GMP_RNDN)
# define num_atanh(ret, n) mpfr_atanh((ret), (n), GMP_RNDN)
# define num_acoth(ret, n) do {              \
        mpfr_pow_si((n), (n), -1, GMP_RNDN); \
        mpfr_atanh((ret), (n), GMP_RNDN);    \
} while (0)
# define num_asech(ret, n) do {              \
        mpfr_pow_si((n), (n), -1, GMP_RNDN); \
        mpfr_acosh((ret), (n), GMP_RNDN);    \
} while (0)
# define num_acsch(ret, n) do {              \
        mpfr_pow_si((n), (n), -1, GMP_RNDN); \
        mpfr_asinh((ret), (n), GMP_RNDN);    \
} while (0)

extern gmp_randstate_t randstate;
# define num_random(n)    mpfr_urandomb((n), randstate)
# define num_zeta(ret, n) mpfr_zeta((ret), (n), GMP_RNDN);
# define num_sinc(ret, n) do {                     \
        if(mpfr_zero_p((n))) {                     \
            mpfr_set_ui((ret), 1, GMP_RNDN);       \
        } else {                                   \
            mpfr_sin((ret), (n), GMP_RNDN);        \
            mpfr_div((ret), (ret), (n), GMP_RNDN); \
        }                                          \
} while (0)

# define num_const_pi(n)    mpfr_const_pi((n), GMP_RNDN)
# define num_const_euler(n) mpfr_const_euler((n), GMP_RNDN)

# define num_out_str(fd, base, n)    mpfr_out_str((fd), (base), 0, (n), GMP_RNDN)
# define num_is_nan(n)               mpfr_nan_p(n)
# define num_is_inf(n)               mpfr_inf_p(n)
# define num_is_zero(n)              mpfr_zero_p(n)
# define num_is_equal(n1, n2)        mpfr_equal_p((n1), (n2))
# define num_is_greater(n1, n2)      mpfr_greater_p((n1), (n2))
# define num_is_greaterequal(n1, n2) mpfr_greaterequal_p((n1), (n2))
# define num_is_less(n1, n2)         mpfr_less_p((n1), (n2))
# define num_is_lessequal(n1, n2)    mpfr_lessequal_p((n1), (n2))
# define num_sign(n)                 mpfr_sgn(n)
# define num_neg(ret, n)             mpfr_neg((ret), (n), GMP_RNDN)
# define num_abs(ret, n)             mpfr_abs((ret), (n), GMP_RNDN)

# define num_cmp_si(n, si) mpfr_cmp_si((n), (si))
# define num_cmp_d(n, d)   mpfr_cmp_d((n), (d))

typedef mp_prec_t num_prec_t;
# define num_get_default_prec()     mpfr_get_default_prec()
# define num_set_default_prec(prec) mpfr_set_default_prec(prec)

# define num_const_catalan(n) mpfr_const_catalan((n), GMP_RNDN)
# define num_cot(ret, n)      mpfr_cot((ret), (n), GMP_RNDN)
# define num_sec(ret, n)      mpfr_sec((ret), (n), GMP_RNDN)
# define num_csc(ret, n)      mpfr_csc((ret), (n), GMP_RNDN)
# define num_coth(ret, n)     mpfr_coth((ret), (n), GMP_RNDN)
# define num_sech(ret, n)     mpfr_sech((ret), (n), GMP_RNDN)
# define num_csch(ret, n)     mpfr_csch((ret), (n), GMP_RNDN)
# define num_eint(ret, n)     mpfr_eint((ret), (n), GMP_RNDN)
# define num_gamma(ret, n)    mpfr_gamma((ret), (n), GMP_RNDN)
# define num_lngamma(ret, n)  mpfr_lngamma((ret), (n), GMP_RNDN)

# define num_bor(ret, n1, n2)  do {              \
        mpz_t intfirst, intsecond, intoutput;    \
        mpz_init(intfirst);                      \
        mpz_init(intsecond);                     \
        mpz_init(intoutput);                     \
        mpfr_get_z(intfirst, (n1), GMP_RNDZ);    \
        mpfr_get_z(intsecond, (n2), GMP_RNDZ);   \
        mpz_ior(intoutput, intfirst, intsecond); \
        mpfr_set_z((ret), intoutput, GMP_RNDN);  \
        mpz_clear(intfirst);                     \
        mpz_clear(intsecond);                    \
        mpz_clear(intoutput);                    \
} while (0)
# define num_band(ret, n1, n2) do {              \
        mpz_t intfirst, intsecond, intoutput;    \
        mpz_init(intfirst);                      \
        mpz_init(intsecond);                     \
        mpz_init(intoutput);                     \
        mpfr_get_z(intfirst, (n1), GMP_RNDZ);    \
        mpfr_get_z(intsecond, (n2), GMP_RNDZ);   \
        mpz_and(intoutput, intfirst, intsecond); \
        mpfr_set_z((ret), intoutput, GMP_RNDN);  \
        mpz_clear(intfirst);                     \
        mpz_clear(intsecond);                    \
        mpz_clear(intoutput);                    \
} while (0)
# define num_bxor(ret, n1, n2) do {              \
        mpz_t intfirst, intsecond, intoutput;    \
        mpz_init(intfirst);                      \
        mpz_init(intsecond);                     \
        mpz_init(intoutput);                     \
        mpfr_get_z(intfirst, (n1), GMP_RNDZ);    \
        mpfr_get_z(intsecond, (n2), GMP_RNDZ);   \
        mpz_xor(intoutput, intfirst, intsecond); \
        mpfr_set_z((ret), intoutput, GMP_RNDN);  \
        mpz_clear(intfirst);                     \
        mpz_clear(intsecond);                    \
        mpz_clear(intoutput);                    \
} while (0)
# define num_trunc(ret, n)     mpfr_trunc((ret), (n))
# define num_comp(ret, n)      do {             \
        mpz_t integer, intoutput;               \
        mpz_init(integer);                      \
        mpz_init(intoutput);                    \
        mpfr_get_z(integer, (n), GMP_RNDZ);     \
        mpz_com(intoutput, integer);            \
        mpfr_set_z((ret), intoutput, GMP_RNDN); \
        mpz_clear(integer);                     \
        mpz_clear(intoutput);                   \
} while (0)
/* this is because binary not doesn't make sense when your integer can be
 * arbitrarily big */
# ifdef _MPFR_H_HAVE_INTMAX_T
#  define num_bnot(ret, n) mpfr_set_uj((ret), ~mpfr_get_uj((n), GMP_RNDN), GMP_RNDN)
# else
#  define num_bnot(ret, n) mpfr_set_ui((ret), ~mpfr_get_ui((n), GMP_RNDN), GMP_RNDN)
# endif

#else /* this is to reimplement mpfr using doubles */
# include <float.h> /* for DBL_MANT_DIG, according to C89 */
# include <stddef.h> /* for size_t, according to C89 */
# include <stdio.h>  /* for FILE* */
struct numberstruct {
    double       value;
    unsigned int nan;
};
typedef struct numberstruct Number[1];

# define NUM_PREC_MIN DBL_MANT_DIG
# define NUM_PREC_MAX DBL_MANT_DIG

typedef int num_exp_t;
typedef long int num_prec_t;

void num_init(Number n);
void num_init_set(Number       n1,
                  const Number n2);
void num_init_set_d(Number       n,
                    const double d);
void num_init_set_ui(Number             n,
                     const unsigned int ui);
int num_init_set_str(Number      n,
                     const char *str,
                     const int   base);

void num_set(Number       n1,
             const Number n2);
void num_set_d(Number       n,
               const double d);
void num_set_ui(Number             n,
                const unsigned int ui);
int num_set_str(Number      n,
                const char *str,
                const int   base);
void num_set_nan(Number n);

void num_mul(Number       ret,
             const Number n1,
             const Number n2);
void num_mul_si(Number       ret,
                const Number n,
                const int    si);
void num_mul_ui(Number             ret,
                const Number       n,
                const unsigned int ui);
void num_sqr(Number       ret,
             const Number n);
void num_sqrt(Number       ret,
              const Number n);
void num_cbrt(Number       ret,
              const Number n);

void num_pow(Number       ret,
             const Number n,
             const Number exp);
void num_pow_si(Number       ret,
                const Number n,
                const int    exp);
void num_exp(Number       ret,
             const Number n);
void num_factorial(Number       ret,
                   unsigned int op);
void num_log10(Number       ret,
               const Number n);
void num_log2(Number       ret,
              const Number n);
void num_log(Number       ret,
             const Number n);

void num_sin(Number       ret,
             const Number n);
void num_cos(Number       ret,
             const Number n);
void num_tan(Number       ret,
             const Number n);
void num_asin(Number       ret,
              const Number n);
void num_acos(Number       ret,
              const Number n);
void num_atan(Number       ret,
              const Number n);
void num_sinh(Number       ret,
              const Number n);
void num_cosh(Number       ret,
              const Number n);
void num_tanh(Number       ret,
              const Number n);
void num_asinh(Number       ret,
               const Number n);
void num_acosh(Number       ret,
               const Number n);
void num_atanh(Number       ret,
               const Number n);
void num_acoth(Number       ret,
               const Number n);
void num_asech(Number       ret,
               const Number n);
void num_acsch(Number       ret,
               const Number n);

void num_add(Number       ret,
             const Number n1,
             const Number n2);
void num_sub(Number       ret,
             const Number n1,
             const Number n2);
void num_add_ui(Number             ret,
                const Number       n,
                const unsigned int ui);
void num_sub_ui(Number             ret,
                const Number       n,
                const unsigned int ui);

void num_div(Number       ret,
             const Number n1,
             const Number n2);
void num_div_ui(Number             ret,
                const Number       n,
                const unsigned int ui);

# define num_modf(ipart, fpart, n) (fpart) = modf((ipart), (n))
void num_rint(Number       ret,
              const Number n);
void num_rintz(Number       ret,
               const Number n);
void num_floor(Number       ret,
               const Number n);
void num_ceil(Number       ret,
              const Number n);

int num_random(Number n);
# define num_zeta(ret, n) num_unimplemented()
void num_sinc(Number       ret,
              const Number n);

void num_const_pi(Number n);
void num_const_euler(Number n);
void num_const_catalan(Number n);

void num_out_str(FILE        *fd,
                 const int    base,
                 const Number n);
int num_is_nan(const Number n);
int num_is_inf(const Number n);
int num_is_zero(const Number n);
int num_is_equal(const Number n1,
                 const Number n2);
int num_is_greater(const Number n1,
                   const Number n2);
int num_is_greaterequal(const Number n1,
                        const Number n2);
int num_is_less(const Number n1,
                const Number n2);
int num_is_lessequal(const Number n1,
                     const Number n2);
int  num_sign(const Number n);
void num_neg(Number       ret,
             const Number n);
void num_abs(Number       ret,
             const Number n);
int num_cmp_si(const Number n,
               const int    si);

void num_free(Number);
double       num_get_d(const Number);
unsigned int num_get_ui(const Number);
char        *num_get_str(char        *str,
                         num_exp_t   *expptr,
                         const int    b,
                         const size_t n,
                         const Number op);
# define num_free_str(a) free(a);

# define num_snprintf(s, n, fmt, ...) snprintf((s), (n), (fmt), ## __VA_ARGS__)
# define num_fprintf(f, fmt, ...)     fprintf((f), (fmt), ## __VA_ARGS__)

num_prec_t num_get_default_prec();
# define num_set_default_prec(prec) /* num_set_default_prec doesn't exist */
void num_cot(Number       ret,
             const Number n);
void num_sec(Number       ret,
             const Number n);
void num_csc(Number       ret,
             const Number n);
void num_coth(Number       ret,
              const Number n);
void num_sech(Number       ret,
              const Number n);
void num_csch(Number       ret,
              const Number n);
# define num_lngamma(ret, n) num_unimplemented()
# define num_gamma(ret, n)   num_unimplemented()
# define num_eint(ret, n)    num_unimplemented()
void num_comp(Number       ret,
              const Number n);
void num_bnot(Number       ret,
              const Number n);
void num_band(Number       ret,
              const Number n1,
              const Number n2);
void num_bxor(Number       ret,
              const Number n1,
              const Number n2);
void num_bor(Number       ret,
             const Number n1,
             const Number n2);
#endif // ifdef HAVE_LIBMPFR

void init_numbers(void);
int  is_int(const Number);
#endif /* NUMBER_H */
/* vim:set expandtab: */
