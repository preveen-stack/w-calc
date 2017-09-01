#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h> /* for NULL */
#include <string.h> /* for strcmp() */
#include <stdio.h> /* for fprintf() */

#include "add_commas.h"
#include "conf.h" /* for getConf(), so I can set configurations and test them */
#include "CuTest.h"

static conf_t *test_init() {
    conf_t *conf = getConf();
    memset(conf, 0, sizeof(conf_t));
    return conf;
}

static void GIVEN_nullIn_WHEN_add_commas_THEN_nullOut(CuTest *tc)
{
    test_init();
    char * result = add_commas(NULL, 0);
    CuAssertPtrEquals(tc, NULL, result);
}

static void GIVEN_emptyIn_WHEN_add_commas_THEN_emptyOut(CuTest *tc)
{
    test_init();
    char * result = add_commas("", 0);
    CuAssertPtrEquals(tc, NULL, result);
}

static void GIVEN_threeDigitsInBaseTenNoSepsConfigured_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    test_init();
    const char *input = "123";
    char * result = add_commas(input, DECIMAL_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_fourDigitsInBaseTenNoSepsConfigured_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    test_init();
    const char *input = "1234";
    char * result = add_commas(input, DECIMAL_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_threeNegativeDigitsInBaseTenNoSepsConfigured_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    test_init();
    const char *input = "-123";
    char * result = add_commas(input, DECIMAL_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_fourNegativeDigitsInBaseTenNoSepsConfigured_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    test_init();
    const char *input = "-1234";
    char * result = add_commas(input, DECIMAL_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_threeDigitsInBaseTen_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    const char *input = "123";
    conf_t *conf = test_init();
    memset(conf, 0, sizeof(conf_t));
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, DECIMAL_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_sevenDigitsInBaseTen_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "1234567";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, DECIMAL_FORMAT);
    CuAssertStrEquals(tc, "1,234,567", result);
}

static void GIVEN_threeNegativeDigitsInBaseTen_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    const char *input = "-123";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, DECIMAL_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_sevenNegativeDigitsInBaseTen_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "-1234567";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, DECIMAL_FORMAT);
    CuAssertStrEquals(tc, "-1,234,567", result);
}

static void GIVEN_eightHexDigitsInBaseTen_WHEN_add_commas_THEN_NULL(CuTest *tc)
{
    const char *input = "1deadbeef";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, DECIMAL_FORMAT);
    CuAssertPtrEquals(tc, NULL, result);
}

static void GIVEN_eightDigitsInBaseTwo_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    const char *input = "10110010";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, BINARY_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_twelveDigitsInBaseTwo_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "101100111000";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, BINARY_FORMAT);
    CuAssertStrEquals(tc, "1011,00111000", result);
}

static void GIVEN_eightNegativeDigitsInBaseTwo_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    const char *input = "-10110010";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, BINARY_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_twelveNegativeDigitsInBaseTwo_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "-101100111000";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, BINARY_FORMAT);
    CuAssertStrEquals(tc, "-1011,00111000", result);
}

static void GIVEN_eightHexDigitsInBaseTwo_WHEN_add_commas_THEN_NULL(CuTest *tc)
{
    const char *input = "1cafebab";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, BINARY_FORMAT);
    CuAssertPtrEquals(tc, NULL, result);
}

static void GIVEN_eightPrefixedDigitsInBaseTwo_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    const char *input = "0b10110010";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    conf->print_prefixes = 1;
    char * result = add_commas(input, BINARY_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_twelvePrefixedDigitsInBaseTwo_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "0b101100111000";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    conf->print_prefixes = 1;
    char * result = add_commas(input, BINARY_FORMAT);
    CuAssertStrEquals(tc, "0b1011,00111000", result);
}

static void GIVEN_eightNegativePrefixedDigitsInBaseTwo_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    const char *input = "-0b10110010";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    conf->print_prefixes = 1;
    char * result = add_commas(input, BINARY_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_twelveNegativePrefixedDigitsInBaseTwo_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "-0b101100111000";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    conf->print_prefixes = 1;
    char * result = add_commas(input, BINARY_FORMAT);
    CuAssertStrEquals(tc, "-0b1011,00111000", result);
}

static void GIVEN_eightPrefixedHexDigitsInBaseTwo_WHEN_add_commas_THEN_NULL(CuTest *tc)
{
    const char *input = "0xdeadbeef";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    conf->print_prefixes = 1;
    char * result = add_commas(input, BINARY_FORMAT);
    CuAssertPtrEquals(tc, NULL, result);
}

static void GIVEN_fourDigitsInBaseEight_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    const char *input = "7654";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, OCTAL_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_nineDigitsInBaseEight_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "765432107";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, OCTAL_FORMAT);
    CuAssertStrEquals(tc, "7,6543,2107", result);
}

static void GIVEN_fourNegativeDigitsInBaseEight_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    const char *input = "-7654";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, OCTAL_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_nineNegativeDigitsInBaseEight_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "-765432107";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, OCTAL_FORMAT);
    CuAssertStrEquals(tc, "-7,6543,2107", result);
}

static void GIVEN_nineDecDigitsInBaseEight_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "765432187";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, OCTAL_FORMAT);
    CuAssertPtrEquals(tc, NULL, result);
}

static void GIVEN_fourPrefixedDigitsInBaseEight_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    const char *input = "07654";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    conf->print_prefixes = 1;
    char * result = add_commas(input, OCTAL_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_ninePrefixedDigitsInBaseEight_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "0765432107";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    conf->print_prefixes = 1;
    char * result = add_commas(input, OCTAL_FORMAT);
    CuAssertStrEquals(tc, "07,6543,2107", result);
}

static void GIVEN_fourNegativePrefixedDigitsInBaseEight_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    const char *input = "07654";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    conf->print_prefixes = 1;
    char * result = add_commas(input, OCTAL_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_nineNegativePrefixedDigitsInBaseEight_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "0765432107";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    conf->print_prefixes = 1;
    char * result = add_commas(input, OCTAL_FORMAT);
    CuAssertStrEquals(tc, "07,6543,2107", result);
}

static void GIVEN_ninePrefixedDecDigitsInBaseEight_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "065432187";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    conf->print_prefixes = 1;
    char * result = add_commas(input, OCTAL_FORMAT);
    CuAssertPtrEquals(tc, NULL, result);
}

static void GIVEN_twoDigitsInBaseSixteen_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    const char *input = "ab";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, HEXADECIMAL_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_fiveDigitsInBaseSixteen_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "cafeb";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, HEXADECIMAL_FORMAT);
    CuAssertStrEquals(tc, "c af eb", result);
}

static void GIVEN_twoPrefixedDigitsInBaseSixteen_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    const char *input = "0xab";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    conf->print_prefixes = 1;
    char * result = add_commas(input, HEXADECIMAL_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_fivePrefixedDigitsInBaseSixteen_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "0xcafeb";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    conf->print_prefixes = 1;
    char * result = add_commas(input, HEXADECIMAL_FORMAT);
    CuAssertStrEquals(tc, "0xc af eb", result);
}

static void GIVEN_twoNegativeDigitsInBaseSixteen_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    const char *input = "-ab";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, HEXADECIMAL_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_fiveNegativeDigitsInBaseSixteen_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "-cafeb";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    char * result = add_commas(input, HEXADECIMAL_FORMAT);
    CuAssertStrEquals(tc, "-c af eb", result);
}

static void GIVEN_twoNegativePrefixedDigitsInBaseSixteen_WHEN_add_commas_THEN_sameOut(CuTest *tc)
{
    const char *input = "-0xab";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    conf->print_prefixes = 1;
    char * result = add_commas(input, HEXADECIMAL_FORMAT);
    CuAssertStrEquals(tc, input, result);
}

static void GIVEN_fiveNegativePrefixedDigitsInBaseSixteen_WHEN_add_commas_THEN_commas(CuTest *tc)
{
    const char *input = "-0xcafeb";
    conf_t *conf = test_init();
    conf->thou_delimiter = ',';
    conf->dec_delimiter  = '.';
    conf->print_prefixes = 1;
    char * result = add_commas(input, HEXADECIMAL_FORMAT);
    CuAssertStrEquals(tc, "-0xc af eb", result);
}

// Need to test:
// exponents
// already-has-commas
CuSuite* add_commas_get_suite(void);
CuSuite* add_commas_get_suite(void) {
    CuSuite  *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, GIVEN_nullIn_WHEN_add_commas_THEN_nullOut);
    SUITE_ADD_TEST(suite, GIVEN_emptyIn_WHEN_add_commas_THEN_emptyOut);
    SUITE_ADD_TEST(suite, GIVEN_threeDigitsInBaseTenNoSepsConfigured_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_fourDigitsInBaseTenNoSepsConfigured_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_threeNegativeDigitsInBaseTenNoSepsConfigured_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_fourNegativeDigitsInBaseTenNoSepsConfigured_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_threeDigitsInBaseTen_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_sevenDigitsInBaseTen_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_threeNegativeDigitsInBaseTen_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_sevenNegativeDigitsInBaseTen_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_eightHexDigitsInBaseTen_WHEN_add_commas_THEN_NULL);
    SUITE_ADD_TEST(suite, GIVEN_eightDigitsInBaseTwo_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_twelveDigitsInBaseTwo_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_eightNegativeDigitsInBaseTwo_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_twelveNegativeDigitsInBaseTwo_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_eightHexDigitsInBaseTwo_WHEN_add_commas_THEN_NULL);
    SUITE_ADD_TEST(suite, GIVEN_eightPrefixedDigitsInBaseTwo_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_twelvePrefixedDigitsInBaseTwo_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_eightNegativePrefixedDigitsInBaseTwo_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_twelveNegativePrefixedDigitsInBaseTwo_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_eightPrefixedHexDigitsInBaseTwo_WHEN_add_commas_THEN_NULL);
    SUITE_ADD_TEST(suite, GIVEN_fourDigitsInBaseEight_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_nineDigitsInBaseEight_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_fourNegativeDigitsInBaseEight_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_nineNegativeDigitsInBaseEight_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_nineDecDigitsInBaseEight_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_fourPrefixedDigitsInBaseEight_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_ninePrefixedDigitsInBaseEight_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_fourNegativePrefixedDigitsInBaseEight_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_nineNegativePrefixedDigitsInBaseEight_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_ninePrefixedDecDigitsInBaseEight_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_twoDigitsInBaseSixteen_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_fiveDigitsInBaseSixteen_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_twoPrefixedDigitsInBaseSixteen_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_fivePrefixedDigitsInBaseSixteen_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_twoNegativeDigitsInBaseSixteen_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_fiveNegativeDigitsInBaseSixteen_WHEN_add_commas_THEN_commas);
    SUITE_ADD_TEST(suite, GIVEN_twoNegativePrefixedDigitsInBaseSixteen_WHEN_add_commas_THEN_sameOut);
    SUITE_ADD_TEST(suite, GIVEN_fiveNegativePrefixedDigitsInBaseSixteen_WHEN_add_commas_THEN_commas);
    return suite;
}
