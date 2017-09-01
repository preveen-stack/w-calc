#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h> /* for NULL */
#include <string.h> /* for strcmp() */
#include <stdio.h> /* for fprintf() */

#include "extract_vars.h"
#include "list.h"
#include "CuTest.h"

static void before(void)
{
    lists_init();
}

static void after(void)
{
    lists_cleanup();
}

static void GIVEN_null_WHEN_extract_vars_THEN_emptyList(CuTest *tc)
{
    before();
    List result = extract_vars(NULL);
    CuAssertIntEquals(tc, 0, listLen(result));
    freeList(&result);
    after();
}

static void GIVEN_emptyString_WHEN_extract_vars_THEN_emptyList(CuTest *tc)
{
    before();
    char *input = strdup("");
    List result = extract_vars(input);
    CuAssertIntEquals(tc, 0, listLen(result));
    freeList(&result);
    free(input);
    after();
}

static void GIVEN_allNumbers_WHEN_extract_vars_THEN_emptyList(CuTest *tc)
{
    before();
    char *input = strdup("123 4 56 7890");
    List result = extract_vars(input);
    CuAssertIntEquals(tc, 0, listLen(result));
    freeList(&result);
    free(input);
    after();
}

static void GIVEN_oneLongWord_WHEN_extract_vars_THEN_oneElementList(CuTest *tc)
{
    before();
    const char *reference = "supercalifragilisticespialidocious";
    char *input = strdup(reference);
    List result = extract_vars(input);
    CuAssertIntEquals(tc, 1, listLen(result));
    char *foundVar = peekAheadInList(result);
    CuAssertStrEquals(tc, reference, foundVar);
    free(foundVar);
    freeList(&result);
    free(input);
    after();
}

static void GIVEN_twoWords_WHEN_extract_vars_THEN_twoElementList(CuTest *tc)
{
    before();
    char *input = strdup("one two");
    List result = extract_vars(input);
    CuAssertIntEquals(tc, 2, listLen(result));
    char *foundVar = getHeadOfList(result);
    CuAssertStrEquals(tc, "one", foundVar);
    free(foundVar);
    foundVar = getHeadOfList(result);
    CuAssertStrEquals(tc, "two", foundVar);
    free(foundVar);
    freeList(&result);
    free(input);
    after();
}

static void GIVEN_duplicateWords_WHEN_extract_vars_THEN_oneElementList(CuTest *tc)
{
    before();
    char *input = strdup("one one");
    List result = extract_vars(input);
    CuAssertIntEquals(tc, 1, listLen(result));
    char *foundVar = getHeadOfList(result);
    CuAssertStrEquals(tc, "one", foundVar);
    free(foundVar);
    freeList(&result);
    free(input);
    after();
}

static void GIVEN_twoWordsWithNumbers_WHEN_extract_vars_THEN_twoElementList(CuTest *tc)
{
    before();
    char *input = strdup("one + two + 3.145");
    List result = extract_vars(input);
    CuAssertIntEquals(tc, 2, listLen(result));
    char *foundVar = getHeadOfList(result);
    CuAssertStrEquals(tc, "one", foundVar);
    free(foundVar);
    foundVar = getHeadOfList(result);
    CuAssertStrEquals(tc, "two", foundVar);
    free(foundVar);
    freeList(&result);
    free(input);
    after();
}

static void GIVEN_functionOfWord_WHEN_extract_vars_THEN_oneElementList(CuTest *tc)
{
    before();
    char *input = strdup("sin(one)");
    List result = extract_vars(input);
    CuAssertIntEquals(tc, 1, listLen(result));
    char *foundVar = getHeadOfList(result);
    CuAssertStrEquals(tc, "one", foundVar);
    free(foundVar);
    freeList(&result);
    free(input);
    after();
}

static void GIVEN_wordPlusPi_WHEN_extract_vars_THEN_oneElementList(CuTest *tc)
{
    before();
    char *input = strdup("pi + one + pi");
    List result = extract_vars(input);
    CuAssertIntEquals(tc, 1, listLen(result));
    char *foundVar = getHeadOfList(result);
    CuAssertStrEquals(tc, "one", foundVar);
    free(foundVar);
    freeList(&result);
    free(input);
    after();
}

CuSuite* extract_vars_get_suite(void);
CuSuite* extract_vars_get_suite(void) {
    CuSuite  *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, GIVEN_null_WHEN_extract_vars_THEN_emptyList);
    SUITE_ADD_TEST(suite, GIVEN_emptyString_WHEN_extract_vars_THEN_emptyList);
    SUITE_ADD_TEST(suite, GIVEN_allNumbers_WHEN_extract_vars_THEN_emptyList);
    SUITE_ADD_TEST(suite, GIVEN_oneLongWord_WHEN_extract_vars_THEN_oneElementList);
    SUITE_ADD_TEST(suite, GIVEN_twoWords_WHEN_extract_vars_THEN_twoElementList);
    SUITE_ADD_TEST(suite, GIVEN_duplicateWords_WHEN_extract_vars_THEN_oneElementList);
    SUITE_ADD_TEST(suite, GIVEN_twoWordsWithNumbers_WHEN_extract_vars_THEN_twoElementList);
    SUITE_ADD_TEST(suite, GIVEN_functionOfWord_WHEN_extract_vars_THEN_oneElementList);
    SUITE_ADD_TEST(suite, GIVEN_wordPlusPi_WHEN_extract_vars_THEN_oneElementList);
    return suite;
}
