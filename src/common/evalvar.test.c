#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h> /* for NULL */
#include <string.h> /* for strcmp() */
#include <stdio.h> /* for fprintf() */
#include <calculator.h>

#include "evalvar.h" /* SUT */

#include "variables.h" /* for init_var() */
#include "conf.h" /* for getConf(), so I can ensure a consistent configuration behavior */
#include "CuTest.h"

extern char* errstring;

static unsigned dv_called = 0;
static unsigned sa_called = 0;

static void display_var(variable_t *v,
                        unsigned count,
                        unsigned digits)
{
    dv_called ++;
}

static void show_answer_test(char *err, bool uncertain, char *answer)
{
    sa_called ++;
}

static void test_init() {
    static int initstate = 0;
    // Reset parser state
    extern int synerrors;
    extern char compute;
    synerrors = 0;
    compute = 1;
    if (errstring) {
        free(errstring);
        errstring = NULL;
    }

    conf_t *conf = getConf();
    memset(conf, 0, sizeof(conf_t));
    dv_called = 0;
    sa_called = 0;
    init_calculator(show_answer_test);
    if (initstate == 0) {
        init_var(display_var);
        initstate = 1;
    } else {
        cleanupvar();
        init_var(display_var);
    }
}

static void GIVEN_null_WHEN_evalvar_noparse_THEN_nullOut(CuTest *tc)
{
    test_init();
    char * result = evalvar_noparse(NULL);
    CuAssertPtrEquals(tc, NULL, result);
    CuAssertPtrEquals(tc, NULL, errstring);
    CuAssertIntEquals(tc, 0, dv_called);
    CuAssertIntEquals(tc, 0, sa_called);
}

static void GIVEN_noSuchVar_WHEN_evalvar_noparse_THEN_nullOut(CuTest *tc)
{
    test_init();
    char * result = evalvar_noparse("testval");
    CuAssertPtrEquals(tc, NULL, result);
    CuAssertPtrEquals(tc, NULL, errstring);
    CuAssertIntEquals(tc, 0, dv_called);
    CuAssertIntEquals(tc, 0, sa_called);
}

static void GIVEN_validNumericalVar_WHEN_evalvar_noparse_THEN_valStr(CuTest *tc)
{
    test_init();
    Number n;
    num_init_set_d(n, 1.0);
    putval("testval", n, "test description");
    char * result = evalvar_noparse("testval");
    CuAssertStrEquals(tc, "1e+0", result);
    CuAssertPtrEquals(tc, NULL, errstring);
    CuAssertIntEquals(tc, 0, dv_called);
    CuAssertIntEquals(tc, 0, sa_called);
}

static void GIVEN_validExpressionVar_WHEN_evalvar_noparse_THEN_expStr(CuTest *tc)
{
    test_init();
    const char *expression = "some string";
    putexp("testval", expression, "test description");
    char * result = evalvar_noparse("testval");
    CuAssertStrEquals(tc, expression, result);
    CuAssertPtrEquals(tc, NULL, errstring);
    CuAssertIntEquals(tc, 0, dv_called);
    CuAssertIntEquals(tc, 0, sa_called);
}

static void GIVEN_null_WHEN_evalvar_THEN_nullOut(CuTest *tc)
{
    test_init();
    char * result = evalvar(NULL);
    CuAssertPtrEquals(tc, NULL, result);
    CuAssertPtrEquals(tc, NULL, errstring);
    CuAssertIntEquals(tc, 0, dv_called);
    CuAssertIntEquals(tc, 0, sa_called);
}

static void GIVEN_noSuchVar_WHEN_evalvar_THEN_nullOut(CuTest *tc)
{
    test_init();
    char * result = evalvar("testval");
    CuAssertPtrEquals(tc, NULL, result);
    CuAssertIntEquals(tc, 0, dv_called);
    CuAssertIntEquals(tc, 0, sa_called);
}

static void GIVEN_validNumericalVar_WHEN_evalvar_THEN_valStr(CuTest *tc)
{
    test_init();
    Number n;
    num_init_set_d(n, 1.0);
    putval("testval", n, "test description");
    char * result = evalvar("testval");
    CuAssertStrEquals(tc, "1e+0", result);
    CuAssertPtrEquals(tc, NULL, errstring);
    CuAssertIntEquals(tc, 0, dv_called);
    CuAssertIntEquals(tc, 0, sa_called);
}

static void GIVEN_bogusExpressionVar_WHEN_evalvar_THEN_valStr(CuTest *tc)
{
    test_init();
    const char *expression = "some string";
    putexp("testval", expression, "test description");
    char * result = evalvar("testval");
    CuAssertStrEquals(tc, "@NaN@", result);
    CuAssertStrEquals(tc, "Undefined variable: some\nUndefined variable: string", errstring);
    CuAssertIntEquals(tc, 0, dv_called);
    CuAssertIntEquals(tc, 0, sa_called);
}

static void GIVEN_validExpressionVar_WHEN_evalvar_THEN_valStr(CuTest *tc)
{
    test_init();
    const char *expression = "1+1";
    putexp("testval", expression, "test description");
    char * result = evalvar("testval");
    CuAssertStrEquals(tc, "2e+0", result);
    CuAssertPtrEquals(tc, NULL, errstring);
    CuAssertIntEquals(tc, 0, dv_called);
    CuAssertIntEquals(tc, 1, sa_called);
}


CuSuite* evalvar_get_suite(void);
CuSuite* evalvar_get_suite(void) {
    CuSuite  *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, GIVEN_null_WHEN_evalvar_noparse_THEN_nullOut);
    SUITE_ADD_TEST(suite, GIVEN_noSuchVar_WHEN_evalvar_noparse_THEN_nullOut);
    SUITE_ADD_TEST(suite, GIVEN_validNumericalVar_WHEN_evalvar_noparse_THEN_valStr);
    SUITE_ADD_TEST(suite, GIVEN_validExpressionVar_WHEN_evalvar_noparse_THEN_expStr);

    SUITE_ADD_TEST(suite, GIVEN_null_WHEN_evalvar_THEN_nullOut);
    SUITE_ADD_TEST(suite, GIVEN_noSuchVar_WHEN_evalvar_THEN_nullOut);
    SUITE_ADD_TEST(suite, GIVEN_validNumericalVar_WHEN_evalvar_THEN_valStr);
    SUITE_ADD_TEST(suite, GIVEN_bogusExpressionVar_WHEN_evalvar_THEN_valStr);
    SUITE_ADD_TEST(suite, GIVEN_validExpressionVar_WHEN_evalvar_THEN_valStr);

    return suite;
}
