#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h> /* for NULL */
#include <string.h> /* for strcmp() */
#include <stdio.h> /* for fprintf() */

#include "string_manip.h"
#include "CuTest.h"

static void GIVEN_null_WHEN_strstrip_THEN_null(CuTest *tc)
{
    char *input = NULL;
    strstrip('\0', input);
    CuAssertPtrEquals(tc, NULL, input); // mostly to ensure the test executes without crashing
}

static void GIVEN_EOB_WHEN_strstrip_THEN_null(CuTest *tc)
{
    char *input = strdup("string");
    strstrip('\0', input);
    CuAssertStrEquals(tc, "string", input);
    free(input);
}

static void GIVEN_emptyString_WHEN_strstrip_THEN_emptyString(CuTest *tc)
{
    char *input = strdup("");
    strstrip('1', input);
    CuAssertStrEquals(tc, "", input);
    free(input);
}

static void GIVEN_notPresentChar_WHEN_strstrip_THEN_unchanged(CuTest *tc)
{
    char *input = strdup("123456789");
    strstrip('a', input);
    CuAssertStrEquals(tc, "123456789", input);
    free(input);
}

static void GIVEN_oneCharToStrip_WHEN_strstrip_THEN_oneStripped(CuTest *tc)
{
    char *input = strdup("123456789");
    strstrip('6', input);
    CuAssertStrEquals(tc, "12345789", input);
    free(input);
}

static void GIVEN_firstCharToStrip_WHEN_strstrip_THEN_oneStripped(CuTest *tc)
{
    char *input = strdup("123456789");
    strstrip('1', input);
    CuAssertStrEquals(tc, "23456789", input);
    free(input);
}

static void GIVEN_lastCharToStrip_WHEN_strstrip_THEN_oneStripped(CuTest *tc)
{
    char *input = strdup("123456789");
    strstrip('9', input);
    CuAssertStrEquals(tc, "12345678", input);
    free(input);
}

static void GIVEN_twoCharsToStrip_WHEN_strstrip_THEN_twoStripped(CuTest *tc)
{
    char *input = strdup("1234561789");
    strstrip('1', input);
    CuAssertStrEquals(tc, "23456789", input);
    free(input);
}

static void GIVEN_allCharsToStrip_WHEN_strstrip_THEN_emptyString(CuTest *tc)
{
    char *input = strdup("bbbbbbbbbb");
    strstrip('b', input);
    CuAssertStrEquals(tc, "", input);
    free(input);
}

static void GIVEN_null_WHEN_strswap_THEN_null(CuTest *tc)
{
    char *input = NULL;
    strswap('1', '2', input);
    CuAssertPtrEquals(tc, NULL, input); // mostly to ensure the test executes without crashing
}

static void GIVEN_emptyString_WHEN_strswap_THEN_emptyString(CuTest *tc)
{
    char *input = strdup("");
    strswap('1', '2', input);
    CuAssertStrEquals(tc, "", input);
    free(input);
}

static void GIVEN_EOB_WHEN_strswap_THEN_unchanged(CuTest *tc)
{
    char *input = strdup("");
    strswap('\0', '2', input);
    CuAssertStrEquals(tc, "", input);
    free(input);
}

static void GIVEN_swNotPresent_WHEN_strswap_THEN_unchanged(CuTest *tc)
{
    char *input = strdup("3456789");
    strswap('1', '2', input);
    CuAssertStrEquals(tc, "3456789", input);
    free(input);
}

static void GIVEN_identicalIO_WHEN_strswap_THEN_unchanged(CuTest *tc)
{
    char *input = strdup("3456789");
    strswap('5', '5', input);
    CuAssertStrEquals(tc, "3456789", input);
    free(input);
}

static void GIVEN_differentIO_WHEN_strswap_THEN_swapped(CuTest *tc)
{
    char *input = strdup("3456789");
    strswap('5', 'M', input);
    CuAssertStrEquals(tc, "34M6789", input);
    free(input);
}

static void GIVEN_null_WHEN_strswap2_THEN_null(CuTest *tc)
{
    char *input = NULL;
    strswap2('1', '2', input);
    CuAssertPtrEquals(tc, NULL, input); // mostly to ensure the test executes without crashing
}

static void GIVEN_emptyString_WHEN_strswap2_THEN_emptyString(CuTest *tc)
{
    char *input = strdup("");
    strswap2('1', '2', input);
    CuAssertStrEquals(tc, "", input);
    free(input);
}

static void GIVEN_EOB_WHEN_strswap2_THEN_unchanged(CuTest *tc)
{
    char *input = strdup("");
    strswap2('\0', '2', input);
    CuAssertStrEquals(tc, "", input);
    free(input);
}

static void GIVEN_swNotPresent_WHEN_strswap2_THEN_unchanged(CuTest *tc)
{
    char *input = strdup("3456789");
    strswap2('1', '2', input);
    CuAssertStrEquals(tc, "3456789", input);
    free(input);
}

static void GIVEN_identicalIO_WHEN_strswap2_THEN_unchanged(CuTest *tc)
{
    char *input = strdup("3456789");
    strswap2('5', '5', input);
    CuAssertStrEquals(tc, "3456789", input);
    free(input);
}

static void GIVEN_differentIO_WHEN_strswap2_THEN_swapped(CuTest *tc)
{
    char *input = strdup("3456789");
    strswap2('5', '8', input);
    CuAssertStrEquals(tc, "3486759", input);
    free(input);
}

static void GIVEN_null_WHEN_count_digits_THEN_zero(CuTest *tc)
{
    unsigned int result = count_digits(NULL);
    CuAssertIntEquals(tc, 0, result);
}

static void GIVEN_emptyString_WHEN_count_digits_THEN_zero(CuTest *tc)
{
    unsigned int result = count_digits("");
    CuAssertIntEquals(tc, 0, result);
}

static void GIVEN_noDigits_WHEN_count_digits_THEN_zero(CuTest *tc)
{
    unsigned int result = count_digits("zyxw");
    CuAssertIntEquals(tc, 0, result);
}

static void GIVEN_tenDecDigitsPlusExtra_WHEN_count_digits_THEN_ten(CuTest *tc)
{
    unsigned int result = count_digits("1234567890zyxw");
    CuAssertIntEquals(tc, 10, result);
}

static void GIVEN_tenDecDigits_WHEN_count_digits_THEN_ten(CuTest *tc)
{
    unsigned int result = count_digits("1234567890");
    CuAssertIntEquals(tc, 10, result);
}

static void GIVEN_tenHexDigitsPlusExtra_WHEN_count_digits_THEN_ten(CuTest *tc)
{
    unsigned int result = count_digits("0xcafebabe9azzz");
    CuAssertIntEquals(tc, 10, result);
}

static void GIVEN_tenHexDigits_WHEN_count_digits_THEN_ten(CuTest *tc)
{
    unsigned int result = count_digits("0xcafebabe9a");
    CuAssertIntEquals(tc, 10, result);
}

static void GIVEN_tenOctDigitsPlusExtra_WHEN_count_digits_THEN_ten(CuTest *tc)
{
    unsigned int result = count_digits("01234567012999");
    CuAssertIntEquals(tc, 10, result);
}

static void GIVEN_tenOctDigits_WHEN_count_digits_THEN_ten(CuTest *tc)
{
    unsigned int result = count_digits("01234567012");
    CuAssertIntEquals(tc, 10, result);
}

static void GIVEN_tenBinDigitsPlusExtra_WHEN_count_digits_THEN_ten(CuTest *tc)
{
    unsigned int result = count_digits("0b10110011102234");
    CuAssertIntEquals(tc, 10, result);
}

static void GIVEN_tenBinDigits_WHEN_count_digits_THEN_ten(CuTest *tc)
{
    unsigned int result = count_digits("0b1011001110");
    CuAssertIntEquals(tc, 10, result);
}

static void GIVEN_null_WHEN_justnumbers_THEN_zero(CuTest *tc)
{
    int result = justnumbers(NULL);
    CuAssertIntEquals(tc, 0, result);
}

static void GIVEN_emptyString_WHEN_justnumbers_THEN_zero(CuTest *tc)
{
    int result = justnumbers("");
    CuAssertIntEquals(tc, 0, result);
}

static void GIVEN_noDigits_WHEN_justnumbers_THEN_zero(CuTest *tc)
{
    int result = justnumbers("zzzzz");
    CuAssertIntEquals(tc, 0, result);
}

static void GIVEN_mixedAlphaNumeric_WHEN_justnumbers_THEN_zero(CuTest *tc)
{
    int result = justnumbers("a1b2c3d4e5f6g!");
    CuAssertIntEquals(tc, 0, result);
}

static void GIVEN_tenDigits_WHEN_justnumbers_THEN_one(CuTest *tc)
{
    int result = justnumbers("a1b2c3d4e5");
    CuAssertIntEquals(tc, 1, result);
}

static void GIVEN_tenCommaSeparatedDigits_WHEN_justnumbers_THEN_one(CuTest *tc)
{
    int result = justnumbers("1,000,000,000");
    CuAssertIntEquals(tc, 1, result);
}

static void GIVEN_tenDecimalDigits_WHEN_justnumbers_THEN_one(CuTest *tc)
{
    int result = justnumbers("1,000,000.777");
    CuAssertIntEquals(tc, 1, result);
}

static void GIVEN_null_WHEN_stripComments_THEN_null(CuTest *tc)
{
    char *input = NULL;
    stripComments(input);
    CuAssertPtrEquals(tc, NULL, input); // mostly to ensure the test executes without crashing
}

static void GIVEN_emptyString_WHEN_stripComments_THEN_null(CuTest *tc)
{
    char *input = strdup("");
    stripComments(input);
    CuAssertStrEquals(tc, "", input);
    free(input);
}

static void GIVEN_hashComment_WHEN_stripComments_THEN_emptyString(CuTest *tc)
{
    char *input = strdup("# this is a comment");
    stripComments(input);
    CuAssertStrEquals(tc, "", input);
    free(input);
}

static void GIVEN_hashCommentSuffix_WHEN_stripComments_THEN_prefix(CuTest *tc)
{
    char *input = strdup("not a comment # this is a comment");
    stripComments(input);
    CuAssertStrEquals(tc, "not a comment ", input);
    free(input);
}

static void GIVEN_slashComment_WHEN_stripComments_THEN_emptyString(CuTest *tc)
{
    char *input = strdup("// this is a comment");
    stripComments(input);
    CuAssertStrEquals(tc, "", input);
    free(input);
}

static void GIVEN_slashCommentSuffix_WHEN_stripComments_THEN_prefix(CuTest *tc)
{
    char *input = strdup("not a comment // this is a comment");
    stripComments(input);
    CuAssertStrEquals(tc, "not a comment ", input);
    free(input);
}

static void GIVEN_cComment_WHEN_stripComments_THEN_emptyString(CuTest *tc)
{
    char *input = strdup("/* this is a comment */");
    stripComments(input);
    CuAssertStrEquals(tc, "", input);
    free(input);
}

static void GIVEN_cCommentSuffix_WHEN_stripComments_THEN_prefix(CuTest *tc)
{
    char *input = strdup("not a comment /* this is a comment */");
    stripComments(input);
    CuAssertStrEquals(tc, "not a comment ", input);
    free(input);
}

static void GIVEN_cCommentPrefix_WHEN_stripComments_THEN_suffix(CuTest *tc)
{
    char *input = strdup("/* this is a comment */not a comment ");
    stripComments(input);
    CuAssertStrEquals(tc, "not a comment ", input);
    free(input);
}

static void GIVEN_cCommentInserted_WHEN_stripComments_THEN_concatenated(CuTest *tc)
{
    char *input = strdup("not a comment /* this is a comment */ not a comment either");
    stripComments(input);
    CuAssertStrEquals(tc, "not a comment  not a comment either", input);
    free(input);
}

static void GIVEN_twoCCommentsInserted_WHEN_stripComments_THEN_concatenated(CuTest *tc)
{
    char *input = strdup("not a comment /* this is a comment */ not a comment either /* more comment */");
    stripComments(input);
    CuAssertStrEquals(tc, "not a comment  not a comment either ", input);
    free(input);
}

CuSuite* string_manip_get_suite(void);
CuSuite* string_manip_get_suite(void) {
    CuSuite  *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, GIVEN_null_WHEN_strstrip_THEN_null);
    SUITE_ADD_TEST(suite, GIVEN_EOB_WHEN_strstrip_THEN_null);
    SUITE_ADD_TEST(suite, GIVEN_emptyString_WHEN_strstrip_THEN_emptyString);
    SUITE_ADD_TEST(suite, GIVEN_notPresentChar_WHEN_strstrip_THEN_unchanged);
    SUITE_ADD_TEST(suite, GIVEN_oneCharToStrip_WHEN_strstrip_THEN_oneStripped);
    SUITE_ADD_TEST(suite, GIVEN_firstCharToStrip_WHEN_strstrip_THEN_oneStripped);
    SUITE_ADD_TEST(suite, GIVEN_lastCharToStrip_WHEN_strstrip_THEN_oneStripped);
    SUITE_ADD_TEST(suite, GIVEN_twoCharsToStrip_WHEN_strstrip_THEN_twoStripped);
    SUITE_ADD_TEST(suite, GIVEN_allCharsToStrip_WHEN_strstrip_THEN_emptyString);
    SUITE_ADD_TEST(suite, GIVEN_null_WHEN_strswap_THEN_null);
    SUITE_ADD_TEST(suite, GIVEN_emptyString_WHEN_strswap_THEN_emptyString);
    SUITE_ADD_TEST(suite, GIVEN_EOB_WHEN_strswap_THEN_unchanged);
    SUITE_ADD_TEST(suite, GIVEN_swNotPresent_WHEN_strswap_THEN_unchanged);
    SUITE_ADD_TEST(suite, GIVEN_identicalIO_WHEN_strswap_THEN_unchanged);
    SUITE_ADD_TEST(suite, GIVEN_differentIO_WHEN_strswap_THEN_swapped);
    SUITE_ADD_TEST(suite, GIVEN_null_WHEN_strswap2_THEN_null);
    SUITE_ADD_TEST(suite, GIVEN_emptyString_WHEN_strswap2_THEN_emptyString);
    SUITE_ADD_TEST(suite, GIVEN_EOB_WHEN_strswap2_THEN_unchanged);
    SUITE_ADD_TEST(suite, GIVEN_swNotPresent_WHEN_strswap2_THEN_unchanged);
    SUITE_ADD_TEST(suite, GIVEN_identicalIO_WHEN_strswap2_THEN_unchanged);
    SUITE_ADD_TEST(suite, GIVEN_differentIO_WHEN_strswap2_THEN_swapped);
    SUITE_ADD_TEST(suite, GIVEN_null_WHEN_count_digits_THEN_zero);
    SUITE_ADD_TEST(suite, GIVEN_emptyString_WHEN_count_digits_THEN_zero);
    SUITE_ADD_TEST(suite, GIVEN_noDigits_WHEN_count_digits_THEN_zero);
    SUITE_ADD_TEST(suite, GIVEN_tenDecDigitsPlusExtra_WHEN_count_digits_THEN_ten);
    SUITE_ADD_TEST(suite, GIVEN_tenDecDigits_WHEN_count_digits_THEN_ten);
    SUITE_ADD_TEST(suite, GIVEN_tenHexDigitsPlusExtra_WHEN_count_digits_THEN_ten);
    SUITE_ADD_TEST(suite, GIVEN_tenHexDigits_WHEN_count_digits_THEN_ten);
    SUITE_ADD_TEST(suite, GIVEN_tenOctDigitsPlusExtra_WHEN_count_digits_THEN_ten);
    SUITE_ADD_TEST(suite, GIVEN_tenOctDigits_WHEN_count_digits_THEN_ten);
    SUITE_ADD_TEST(suite, GIVEN_tenBinDigitsPlusExtra_WHEN_count_digits_THEN_ten);
    SUITE_ADD_TEST(suite, GIVEN_tenBinDigits_WHEN_count_digits_THEN_ten);
    SUITE_ADD_TEST(suite, GIVEN_null_WHEN_justnumbers_THEN_zero);
    SUITE_ADD_TEST(suite, GIVEN_emptyString_WHEN_justnumbers_THEN_zero);
    SUITE_ADD_TEST(suite, GIVEN_noDigits_WHEN_justnumbers_THEN_zero);
    SUITE_ADD_TEST(suite, GIVEN_mixedAlphaNumeric_WHEN_justnumbers_THEN_zero);
    SUITE_ADD_TEST(suite, GIVEN_tenDigits_WHEN_justnumbers_THEN_one);
    SUITE_ADD_TEST(suite, GIVEN_tenCommaSeparatedDigits_WHEN_justnumbers_THEN_one);
    SUITE_ADD_TEST(suite, GIVEN_tenDecimalDigits_WHEN_justnumbers_THEN_one);
    SUITE_ADD_TEST(suite, GIVEN_null_WHEN_stripComments_THEN_null);
    SUITE_ADD_TEST(suite, GIVEN_emptyString_WHEN_stripComments_THEN_null);
    SUITE_ADD_TEST(suite, GIVEN_hashComment_WHEN_stripComments_THEN_emptyString);
    SUITE_ADD_TEST(suite, GIVEN_hashCommentSuffix_WHEN_stripComments_THEN_prefix);
    SUITE_ADD_TEST(suite, GIVEN_slashComment_WHEN_stripComments_THEN_emptyString);
    SUITE_ADD_TEST(suite, GIVEN_slashCommentSuffix_WHEN_stripComments_THEN_prefix);
    SUITE_ADD_TEST(suite, GIVEN_cComment_WHEN_stripComments_THEN_emptyString);
    SUITE_ADD_TEST(suite, GIVEN_cCommentSuffix_WHEN_stripComments_THEN_prefix);
    SUITE_ADD_TEST(suite, GIVEN_cCommentPrefix_WHEN_stripComments_THEN_suffix);
    SUITE_ADD_TEST(suite, GIVEN_cCommentInserted_WHEN_stripComments_THEN_concatenated);
    SUITE_ADD_TEST(suite, GIVEN_twoCCommentsInserted_WHEN_stripComments_THEN_concatenated);
    return suite;
}
