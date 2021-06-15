#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* System Headers */
#include <stdio.h>
#include <string.h>  // for strlen

/* Internal Headers */
#include "display.h"
#include "output.h"
#include "isconst.h"

void
display_consts(const struct name_with_exp *consts)
{/*{{{*/
    size_t linelen = 0;

    for (size_t i = 0; consts[i].explanation; i++) {
        const char *const *const names = consts[i].names;
        for (size_t j = 0; names[j]; j++) {
            if (linelen + strlen(names[j]) + 2 > 70) {
                printf(",\n");
                linelen = 0;
            }
            if (linelen == 0) {
                printf("%s", names[j]);
                linelen = strlen(names[j]);
            } else {
                printf(", %s", names[j]);
                linelen += strlen(names[j]) + 2;
            }
        }
    }
    printf("\n");
}/*}}}*/


