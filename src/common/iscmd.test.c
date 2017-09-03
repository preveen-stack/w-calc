#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h> /* for NULL */
#include <string.h> /* for strcmp() */
#include <stdio.h> /* for fprintf() */

#include "iscmd.h"
#include "CuTest.h"

static void GIVEN_null_WHEN_iscmd_THEN_false(CuTest *tc)
{
    int result = iscmd(NULL);
    CuAssertIntEquals(tc, 0, result);
}
static void GIVEN_emptyString_WHEN_iscmd_THEN_false(CuTest *tc)
{
    int result = iscmd("");
    CuAssertIntEquals(tc, 0, result);
}
static void GIVEN_notACmd_WHEN_iscmd_THEN_false(CuTest *tc)
{
    int result = iscmd("notACmd");
    CuAssertIntEquals(tc, 0, result);
}
static void GIVEN_func_WHEN_iscmd_THEN_false(CuTest *tc)
{
    int result = iscmd("sin");
    CuAssertIntEquals(tc, 0, result);
}
static void GIVEN_cmd_WHEN_iscmd_THEN_true(CuTest *tc)
{
    int result = iscmd("verbose");
    CuAssertIntEquals(tc, 1, result);
}
static void GIVEN_capitalizedCmd_WHEN_iscmd_THEN_false(CuTest *tc)
{
    int result = iscmd("Verbose");
    CuAssertIntEquals(tc, 0, result);
}

CuSuite* iscmd_get_suite(void);
CuSuite* iscmd_get_suite(void) {
    CuSuite  *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, GIVEN_null_WHEN_iscmd_THEN_false);
    SUITE_ADD_TEST(suite, GIVEN_emptyString_WHEN_iscmd_THEN_false);
    SUITE_ADD_TEST(suite, GIVEN_notACmd_WHEN_iscmd_THEN_false);
    SUITE_ADD_TEST(suite, GIVEN_func_WHEN_iscmd_THEN_false);
    SUITE_ADD_TEST(suite, GIVEN_cmd_WHEN_iscmd_THEN_true);
    SUITE_ADD_TEST(suite, GIVEN_capitalizedCmd_WHEN_iscmd_THEN_false);
    return suite;
}
