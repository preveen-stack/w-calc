#include "CuTest.h"
#include <stdio.h> /* for printf() */

CuSuite* add_commas_get_suite();
CuSuite* evalvar_get_suite();

int main(void) {
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    CuSuiteAddSuite(suite, add_commas_get_suite());
    CuSuiteAddSuite(suite, evalvar_get_suite());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    int status = 0;
    for (size_t i=0; i<output->length-2; i++) {
        if (output->buffer[i] != '.') {
            status = 1;
            break;
        }
    }
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    return status;
}

