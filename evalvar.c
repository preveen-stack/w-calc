#include <gmp.h>
#include <mpfr.h>
#include "variables.h"
#include "calculator.h"
#include "number_formatting.h"

char *evalvar(char *varname)
{
    struct answer a;
    char *varvalue, junk;
    mpfr_t f;

    a = getvar_full(varname);
    if (!a.err) {
	mpfr_init(f);
	if (a.exp) {		       // it is an expression
	    parseme(a.exp);
	    mpfr_set(f, last_answer, GMP_RNDN);
	} else {		       // it is a value
	    mpfr_set(f, a.val, GMP_RNDN);
	    mpfr_clear(a.val);
	}
	varvalue = num_to_str_complex(f, 10, 0, -1, 1, &junk);
	mpfr_clear(f);
	return varvalue;
    } else {
	return NULL;
    }
}
