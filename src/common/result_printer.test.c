#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h> /* for NULL */
#include <string.h> /* for strcmp() */
#include <stdio.h> /* for fprintf() */
#include <conf.h>

#include "result_printer.h"
#include "CuTest.h"

static int show_answer_called = 0;
Number last_answer;

static void
show_answer_callback(char *err, bool uncertain, char *answer)
{
    printf("err:'%s' uncertain:%i answer:'%s'\n", err, uncertain, answer);
    show_answer_called ++;
}

static void init_test_conf()
{
    conf_t *conf = getConf();

    /* initialize the preferences */
    memset(conf, 0, sizeof(struct _conf));
    conf->precision           = -1;
    conf->scientific          = automatic;
    conf->picky_variables     = 1;
    conf->print_prefixes      = 1;
    conf->precision_guard     = 1;
    conf->thou_delimiter      = ',';
    conf->dec_delimiter       = '.';
    conf->print_equal         = 1;
    conf->c_style_mod         = 1;
    conf->print_greeting      = 1;
    conf->rounding_indication = SIG_FIG_ROUNDING_INDICATION;
    Dprintf("---------------------------------------------------------NEW TEST\n");
}

static void GIVEN_badArgs_WHEN_init_resultprinter_THEN_nocrash(CuTest *tc)
{
    init_test_conf();
    init_resultprinter(NULL, NULL);

    void *lastr = (void*)get_last_answer_str();
    CuAssertPtrEquals_Msg(tc, "last_answer_str", NULL, lastr);

    reset_last_answer_str();

    lastr = (void*)get_last_answer_str();
    CuAssertPtrEquals_Msg(tc, "last_answer_str after reset", NULL, lastr);

    show_answer("errstr", 0, "display");
    CuAssertIntEquals_Msg(tc, "show_answer called", 0, show_answer_called);

    term_resultprinter();
}

static void GIVEN_noDisplayCallback_WHEN_init_resultprinter_THEN_NaN(CuTest *tc)
{
    init_test_conf();
    num_init(last_answer);
    init_resultprinter(NULL, &last_answer);

    void *laptr = (void*)get_last_answer_str();
    CuAssertPtrEquals_Msg(tc, "last_answer_str", NULL, laptr);

    reset_last_answer_str();

    const char *lastr = get_last_answer_str();
    CuAssertStrEquals_Msg(tc, "last_answer_str after reset", "@NaN@", lastr);

    show_answer("errstr", 0, "display");
    CuAssertIntEquals_Msg(tc, "show_answer called", 0, show_answer_called);

    term_resultprinter();
    num_free(last_answer);
}

static void GIVEN_happyPath_WHEN_init_resultprinter_THEN_correctString(CuTest *tc)
{
    init_test_conf();
    num_init(last_answer);
    init_resultprinter(show_answer_callback, &last_answer);
    CuAssertIntEquals_Msg(tc, "show_answer called", 0, show_answer_called);

    void *laptr = (void*)get_last_answer_str();
    CuAssertPtrEquals_Msg(tc, "last_answer_str", NULL, laptr);
    CuAssertIntEquals_Msg(tc, "show_answer called", 0, show_answer_called);

    reset_last_answer_str();
    CuAssertIntEquals_Msg(tc, "show_answer called", 1, show_answer_called);

    const char *lastr = get_last_answer_str();
    CuAssertStrEquals_Msg(tc, "last_answer_str after reset", "@NaN@", lastr);

    show_answer("errstr", 0, "display");
    CuAssertIntEquals_Msg(tc, "show_answer called", 2, show_answer_called);
    show_answer_called = 0;

    term_resultprinter();
    num_free(last_answer);
}

static void GIVEN_one_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    bool nad_out = true;
    char *es_out = NULL;

    init_test_conf();
    num_init_set_d(last_answer, 1.0);
    init_resultprinter(show_answer_callback, &last_answer);

    // WHEN
    const char * ret = print_this_result(last_answer, false, 1, &nad_out, &es_out);

    // THEN
    CuAssertStrEquals_Msg(tc, "output string", "1", ret);
    CuAssertPtrEquals_Msg(tc, "error string", NULL, (void*)es_out);
    CuAssertTrue(tc, !nad_out);
    num_free(last_answer);
}

static void GIVEN_oneDotOne_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    bool nad_out = true;
    char *es_out = NULL;

    init_test_conf();
    num_init_set_d(last_answer, 1.1);
    init_resultprinter(show_answer_callback, &last_answer);

    // WHEN
    const char * ret = print_this_result(last_answer, false, 2, &nad_out, &es_out);

    // THEN
    CuAssertStrEquals_Msg(tc, "output string", "1.1", ret);
    CuAssertPtrEquals_Msg(tc, "error string", NULL, (void*)es_out);
    CuAssertTrue(tc, !nad_out);
    num_free(last_answer);
}

static void GIVEN_nan_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    bool nad_out = true;
    char *es_out = NULL;

    init_test_conf();
    num_init(last_answer);
    num_set_nan(last_answer);
    init_resultprinter(show_answer_callback, &last_answer);

    // WHEN
    const char * ret = print_this_result(last_answer, false, 1, &nad_out, &es_out);

    // THEN
    CuAssertStrEquals_Msg(tc, "output string", "@NaN@", ret);
    CuAssertPtrEquals_Msg(tc, "error string", NULL, (void*)es_out);
    CuAssertTrue(tc, !nad_out);
    num_free(last_answer);
}

static void GIVEN_inf_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    bool nad_out = true;
    char *es_out = NULL;

    init_test_conf();
    num_init(last_answer);
    num_set_inf(last_answer, 1);
    init_resultprinter(show_answer_callback, &last_answer);

    // WHEN
    const char * ret = print_this_result(last_answer, false, 1, &nad_out, &es_out);

    // THEN
    CuAssertStrEquals_Msg(tc, "output string", "@Inf@", ret);
    CuAssertPtrEquals_Msg(tc, "error string", NULL, (void*)es_out);
    CuAssertTrue(tc, !nad_out);
    num_free(last_answer);
}

static void GIVEN_negInf_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    bool nad_out = true;
    char *es_out = NULL;

    init_test_conf();
    num_init(last_answer);
    num_set_inf(last_answer, -1);
    init_resultprinter(show_answer_callback, &last_answer);

    // WHEN
    const char * ret = print_this_result(last_answer, false, 1, &nad_out, &es_out);

    // THEN
    CuAssertStrEquals_Msg(tc, "output string", "-@Inf@", ret);
    CuAssertPtrEquals_Msg(tc, "error string", NULL, (void*)es_out);
    CuAssertTrue(tc, !nad_out);
    num_free(last_answer);
}

static void GIVEN_zero_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    bool nad_out = true;
    char *es_out = NULL;

    init_test_conf();
    num_init_set_d(last_answer, 0);
    init_resultprinter(show_answer_callback, &last_answer);

    // WHEN
    const char * ret = print_this_result(last_answer, false, 0, &nad_out, &es_out);

    // THEN
    CuAssertStrEquals_Msg(tc, "output string", "0", ret);
    CuAssertPtrEquals_Msg(tc, "error string", NULL, (void*)es_out);
    CuAssertTrue(tc, !nad_out);
    num_free(last_answer);
}

static void GIVEN_negZero_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    bool nad_out = true;
    char *es_out = NULL;

    init_test_conf();
    num_init_set_d(last_answer, -0);
    init_resultprinter(show_answer_callback, &last_answer);

    // WHEN
    const char * ret = print_this_result(last_answer, false, 0, &nad_out, &es_out);

    // THEN
    CuAssertStrEquals_Msg(tc, "output string", "0", ret);
    CuAssertPtrEquals_Msg(tc, "error string", NULL, (void*)es_out);
    CuAssertTrue(tc, !nad_out);
    num_free(last_answer);
}

static void GIVEN_oneThird_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    bool nad_out = true;
    char *es_out = NULL;

    init_test_conf();
    num_init_set_d(last_answer, 1.0/3.0);
    init_resultprinter(show_answer_callback, &last_answer);

    // WHEN
    const char * ret = print_this_result(last_answer, false, 1, &nad_out, &es_out);

    // THEN
    CuAssertStrEquals_Msg(tc, "output string", "0.333333", ret);
    CuAssertPtrEquals_Msg(tc, "error string", NULL, (void*)es_out);
    CuAssertTrue(tc, !nad_out);
    num_free(last_answer);
}

CuSuite* result_printer_get_suite(void);
CuSuite* result_printer_get_suite(void) {
    CuSuite  *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, GIVEN_badArgs_WHEN_init_resultprinter_THEN_nocrash);
    SUITE_ADD_TEST(suite, GIVEN_noDisplayCallback_WHEN_init_resultprinter_THEN_NaN);
    SUITE_ADD_TEST(suite, GIVEN_happyPath_WHEN_init_resultprinter_THEN_correctString);
    SUITE_ADD_TEST(suite, GIVEN_one_WHEN_print_this_result_THEN_correctStringWithNadUnset);
    SUITE_ADD_TEST(suite, GIVEN_oneDotOne_WHEN_print_this_result_THEN_correctStringWithNadUnset);
    SUITE_ADD_TEST(suite, GIVEN_nan_WHEN_print_this_result_THEN_correctStringWithNadUnset);
    SUITE_ADD_TEST(suite, GIVEN_inf_WHEN_print_this_result_THEN_correctStringWithNadUnset);
    SUITE_ADD_TEST(suite, GIVEN_negInf_WHEN_print_this_result_THEN_correctStringWithNadUnset);
    SUITE_ADD_TEST(suite, GIVEN_zero_WHEN_print_this_result_THEN_correctStringWithNadUnset);
    SUITE_ADD_TEST(suite, GIVEN_negZero_WHEN_print_this_result_THEN_correctStringWithNadUnset);
    SUITE_ADD_TEST(suite, GIVEN_oneThird_WHEN_print_this_result_THEN_correctStringWithNadUnset);
    return suite;
}
