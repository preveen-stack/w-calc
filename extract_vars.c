#include <stdlib.h>		       /* for NULL */
#include <ctype.h>		       /* for isalpha() and isdigit() */
#include <string.h>		       /* for strdup() */
#include "list.h"		       /* for List */
#include "isfunc.h"
#include "isconst.h"
#include "extract_vars.h"

List extract_vars(char *str)
{
    char *curs, *eov, save_char;
    List variables = NULL;
    char *varname;

    curs = str;
    while (curs && *curs) {
	// search for the first letter of a possible variable
	while (curs && *curs && !isalpha((int)(*curs))) {
	    if (*curs == '\'' || *curs == '#') {
		break;
	    }
	    curs++;
	    // skip hex numbers
	    if ((*curs == 'x' || *curs == 'X') && curs != str &&
		*(curs - 1) == '0') {
		curs++;
		while (curs && *curs &&
		       (isdigit((int)(*curs)) ||
			(*curs >= 'a' && *curs <= 'f') || (*curs >= 'A' &&
							   *curs <= 'F'))) {
		    curs++;
		}
	    }
	    // skip binary numbers
	    if (*curs == 'b' && curs != str && *(curs - 1) == '0') {
		curs++;
	    }
	}

	// if we didn't find a first letter, we're done looking
	if (*curs == 0 || *curs == '#' || *curs == '\'') {
	    break;
	}
	// if we did find something, pull out the variable name
	eov = curs;
	while (eov && *eov &&
	       (isalpha((int)(*eov)) || *eov == '_' || *eov == ':' ||
		isdigit((int)(*eov)))) {
	    eov++;
	}
	save_char = *eov;
	*eov = 0;
	varname = (char *)strdup(curs);
	*eov = save_char;
	curs = eov;

	// add it to the set of known variables
	if (!isfunc(varname) && !isconst(varname)) {
	    size_t i = 0;
	    char exists = 0;

	    while (i < listLen(variables)) {
		char *str = peekListElement(variables, i);

		if (!strcmp(str, varname)) {
		    exists = 1;
		    break;
		}
		i++;
	    }
	    if (!exists) {
		addToList(&variables, (void *)strdup(varname));
	    }
	}
	free(varname);
    }
    return variables;
}
