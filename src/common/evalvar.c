#include "number.h"
#include "variables.h"
#include "calculator.h"
#include "number_formatting.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif

char *evalvar(const char *varname)
{
    struct answer a;
    char *varvalue, junk;
    Number f;

    a = getvar_full(varname);
    if (!a.err) {
	num_init(f);
	if (a.exp) {		       // it is an expression
	    parseme(a.exp);
	    num_set(f, last_answer);
	} else {		       // it is a value
	    num_set(f, a.val);
	    num_free(a.val);
	}
	varvalue = num_to_str_complex(f, 10, 0, -1, 1, &junk);
	num_free(f);
	return varvalue;
    } else {
	return NULL;
    }
}