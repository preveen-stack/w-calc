#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h> /* for NULL */
#include <string.h> /* for strcmp() */
#include <stdio.h> /* for fprintf() */

#include "isfunc.h"
#include "CuTest.h"

static void GIVEN_null_WHEN_isfunc_THEN_false(CuTest *tc)
{
    int result = isfunc(NULL);
    CuAssertIntEquals(tc, 0, result);
}
static void GIVEN_emptyString_WHEN_isfunc_THEN_false(CuTest *tc)
{
    int result = isfunc("");
    CuAssertIntEquals(tc, 0, result);
}
static void GIVEN_notAFunc_WHEN_isfunc_THEN_false(CuTest *tc)
{
    int result = isfunc("notAFunc");
    CuAssertIntEquals(tc, 0, result);
}
static void GIVEN_func_WHEN_isfunc_THEN_true(CuTest *tc)
{
    int result = isfunc("sin");
    CuAssertIntEquals(tc, 1, result);
}
static void GIVEN_capitalizedFunc_WHEN_isfunc_THEN_true(CuTest *tc)
{
    int result = isfunc("sIn");
    CuAssertIntEquals(tc, 1, result);
}

CuSuite* isfunc_get_suite(void);
CuSuite* isfunc_get_suite(void) {
    CuSuite  *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, GIVEN_null_WHEN_isfunc_THEN_false);
    SUITE_ADD_TEST(suite, GIVEN_emptyString_WHEN_isfunc_THEN_false);
    SUITE_ADD_TEST(suite, GIVEN_notAFunc_WHEN_isfunc_THEN_false);
    SUITE_ADD_TEST(suite, GIVEN_func_WHEN_isfunc_THEN_true);
    SUITE_ADD_TEST(suite, GIVEN_capitalizedFunc_WHEN_isfunc_THEN_true);
    return suite;
}
