#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* Internal Headers */
#include "number.h"
#include "variables.h"
#include "calculator.h" // for parseme()
#include "number_formatting.h"

/* My Prototype */
#include "evalvar.h"

/* System Headers */
#include <string.h> /* for strdup() */

char *evalvar(const char *varname)
{
    struct answer a;
    Number        f;

    a = getvar_full(varname);
    if (!a.err) {
        num_init(f);
        if (a.exp) {                   // it is an expression
            extern char* errstring;
            parseme(a.exp);
            if (!errstring) {
                num_set(f, *get_last_answer());
            }
        } else {                       // it is a value
            num_set(f, a.val);
            num_free(a.val);
        }
        char *varvalue = num_to_str_complex(f, 10, 0, -1, 1, NULL);
        num_free(f);
        return varvalue;
    } else {
        return NULL;
    }
}

char *evalvar_noparse(const char *varname)
{
    struct answer a;
    Number        f;

    a = getvar_full(varname);
    if (!a.err) {
        if (a.exp) {                   // it is an expression
            return strdup(a.exp);
        }
        // it is a value
        num_init_set(f, a.val);
        num_free(a.val);
        char *varvalue = num_to_str_complex(f, 10, 0, -1, 1, NULL);
        num_free(f);
        return varvalue;
    } else {
        return NULL;
    }
}

/* vim:set expandtab: */
