#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h> /* for NULL */
#include <string.h> /* for strcmp() */
#include <stdio.h> /* for fprintf() */

#include "result_printer.h"
#include "CuTest.h"

static int show_answer_called = 0;
Number last_answer;

static void
show_answer_callback(char *err, int  uncertain, char *answer)
{
    printf("err:'%s' uncertain:%i answer:'%s'\n", err, uncertain, answer);
    show_answer_called ++;
}

static void GIVEN_badArgs_WHEN_init_resultprinter_THEN_nocrash(CuTest *tc)
{
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

CuSuite* result_printer_get_suite(void);
CuSuite* result_printer_get_suite(void) {
    CuSuite  *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, GIVEN_badArgs_WHEN_init_resultprinter_THEN_nocrash);
    SUITE_ADD_TEST(suite, GIVEN_noDisplayCallback_WHEN_init_resultprinter_THEN_NaN);
    SUITE_ADD_TEST(suite, GIVEN_happyPath_WHEN_init_resultprinter_THEN_correctString);
    return suite;
}
