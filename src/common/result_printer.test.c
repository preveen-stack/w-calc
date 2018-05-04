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

static conf_t* init_test_conf()
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
    return conf;
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

static void basicPTRtest(CuTest *tc, const unsigned int sigfigs, const char * expectedOutput,
                         const bool expectedNad)
{
    bool nad_out = true;
    char *es_out = NULL;
    char desc[100];
    const char *format;
    const char *rounding;
    conf_t *conf = getConf();

    switch (conf->output_format) {
        default:
        case DECIMAL_FORMAT: format = "decimal"; break;
        case OCTAL_FORMAT: format = "octal"; break;
        case HEXADECIMAL_FORMAT: format = "hex"; break;
        case BINARY_FORMAT: format = "binary"; break;
    }
    switch (conf->rounding_indication) {
        default:
        case NO_ROUNDING_INDICATION: rounding = "no rounding"; break;
        case SIMPLE_ROUNDING_INDICATION: rounding = "simple rounding"; break;
        case SIG_FIG_ROUNDING_INDICATION: rounding = "sig fig rounding"; break;
    }
    snprintf(desc, 100, "%s, %s (prec:%i)", format, rounding, conf->precision);

    // WHEN
    Dprintf("---------------------------------------------------------print_this_result\n");
    const char * ret = print_this_result(last_answer, false, sigfigs, &nad_out, &es_out);

    // THEN
    snprintf(desc, 100, "%s, %s (prec:%i): output string", format, rounding, conf->precision);
    CuAssertStrEquals_Msg(tc, desc, expectedOutput, ret);
    snprintf(desc, 100, "%s, %s (prec:%i): error string", format, rounding, conf->precision);
    CuAssertPtrEquals_Msg(tc, desc, NULL, (void*)es_out);
    snprintf(desc, 100, "%s, %s (prec:%i): not all displayed (%s)", format, rounding,
             conf->precision, ret);
    CuAssertIntEquals_Msg(tc, desc, expectedNad, nad_out);
}

static void basicPrecisionTest(CuTest *tc, const unsigned sigfigs, const char **answers,
                               const bool *nad, const int testCount)
{
    conf_t *conf = getConf();

    for (int i = 0; i < testCount; i++) {
        conf->precision = i-1;
        basicPTRtest(tc, sigfigs, answers[i], nad?nad[i]:false);
    }
}

static void GIVEN_one_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    conf_t *conf = init_test_conf();
    num_init_set_d(last_answer, 1.0);
    init_resultprinter(show_answer_callback, &last_answer);

    const char *answers[FORMAT_COUNT][5] = {
          {"1", "1", "1.0", "1.00", "1.000"},
          {"01", "01", "01.0", "01.00", "01.000"},
          {"0x1", "0x1", "0x1.0", "0x1.00", "0x1.000"},
          {"0b1", "0b1", "0b1.0", "0b1.00", "0b1.000"},
    };
    const int testCount[FORMAT_COUNT] = {5, 5, 5, 5};

    // WHEN
    for (enum rounding_style round = NO_ROUNDING_INDICATION; round < ROUNDING_INDICATION_COUNT;
         round++) {
        conf->rounding_indication = round;
        for (enum output_formats format = DECIMAL_FORMAT; format < FORMAT_COUNT; format++) {
            conf->output_format = format;
            basicPrecisionTest(tc, 1, answers[format], NULL, testCount[format]);
        }
    }
    num_free(last_answer);
}

/*
conf->precision           = -1;
conf->scientific          = automatic;
conf->print_prefixes      = 1;
conf->precision_guard     = 1;
conf->thou_delimiter      = ',';
conf->dec_delimiter       = '.';
conf->print_equal         = 1;
conf->c_style_mod         = 1;
conf->print_greeting      = 1;
conf->rounding_indication = SIG_FIG_ROUNDING_INDICATION;
 */

static void GIVEN_oneDotOne_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    conf_t *conf = init_test_conf();
    num_init_set_d(last_answer, 1.1);
    init_resultprinter(show_answer_callback, &last_answer);

    const char *answers[FORMAT_COUNT][5] = {
          {"1.1", "1", "1.1", "1.10", "1.100"},
          {"01.06314631463146315", "01", "01.1", "01.06", "01.063"},
          {"0x1.199999999999a", "0x1", "0x1.2", "0x1.1a", "0x1.19a"},
          {"0b1.00011001100110011001100110011", "0b1", "0b1.0", "0b1.00", "0b1.001"},
    };
    const bool expectedNad[FORMAT_COUNT][ROUNDING_INDICATION_COUNT][5] = {
          {{false, false, false, false, false}, // decimal, no round
           {false, true, true, true, true}, // decimal simple
           {false, true, false, false, false}}, // decimal sig fig
          {{false, false, false, false, false}, // octal no round
           {true, true, true, true, true}, // octal simple
           {true, true, false, false, false}}, // octal sig fig
          {{false, false, false, false, false}, // hex no round
           {true, true, true, true, true}, // hex simple
           {true, true, false, false, false}}, // hex sig fig
          {{false, false, false, false, false}, // binary no round
           {true, true, true, true, true}, // binary simple
           {true, true, false, false, false}}, // binary sig fig
    };
    const int testCount[FORMAT_COUNT] = {5, 5, 5, 5};

    // WHEN
    for (enum rounding_style round = NO_ROUNDING_INDICATION; round < ROUNDING_INDICATION_COUNT;
         round++) {
        conf->rounding_indication = round;
        for (enum output_formats format = DECIMAL_FORMAT; format < FORMAT_COUNT; format++) {
            conf->output_format = format;
            basicPrecisionTest(tc, 2, answers[format], expectedNad[format][round],
                               testCount[format]);
        }
    }
    num_free(last_answer);
}

static void GIVEN_nan_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    conf_t *conf = init_test_conf();
    num_init(last_answer);
    num_set_nan(last_answer);
    init_resultprinter(show_answer_callback, &last_answer);

    const char *answers[FORMAT_COUNT][5] = {
          {"@NaN@", "@NaN@", "@NaN@", "@NaN@", "@NaN@"},
          {"@NaN@", "@NaN@", "@NaN@", "@NaN@", "@NaN@"},
          {"@NaN@", "@NaN@", "@NaN@", "@NaN@", "@NaN@"},
          {"@NaN@", "@NaN@", "@NaN@", "@NaN@", "@NaN@"},
    };
    const int testCount[FORMAT_COUNT] = {5, 5, 5, 5};

    // WHEN
    for (enum rounding_style round = NO_ROUNDING_INDICATION; round < ROUNDING_INDICATION_COUNT;
         round++) {
        conf->rounding_indication = round;
        for (enum output_formats format = DECIMAL_FORMAT; format < FORMAT_COUNT; format++) {
            conf->output_format = format;
            basicPrecisionTest(tc, 2, answers[format], NULL, testCount[format]);
        }
    }
    num_free(last_answer);
}

static void GIVEN_inf_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    conf_t *conf = init_test_conf();
    num_init(last_answer);
    num_set_inf(last_answer, 1);
    init_resultprinter(show_answer_callback, &last_answer);

    const char *answers[FORMAT_COUNT][5] = {
          {"@Inf@", "@Inf@", "@Inf@", "@Inf@", "@Inf@"},
          {"@Inf@", "@Inf@", "@Inf@", "@Inf@", "@Inf@"},
          {"@Inf@", "@Inf@", "@Inf@", "@Inf@", "@Inf@"},
          {"@Inf@", "@Inf@", "@Inf@", "@Inf@", "@Inf@"},
    };
    const int testCount[FORMAT_COUNT] = {5, 5, 5, 5};

    // WHEN
    for (enum rounding_style round = NO_ROUNDING_INDICATION; round < ROUNDING_INDICATION_COUNT;
         round++) {
        conf->rounding_indication = round;
        for (enum output_formats format = DECIMAL_FORMAT; format < FORMAT_COUNT; format++) {
            conf->output_format = format;
            basicPrecisionTest(tc, 2, answers[format], NULL, testCount[format]);
        }
    }
    num_free(last_answer);
}

static void GIVEN_negInf_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    conf_t *conf = init_test_conf();
    num_init(last_answer);
    num_set_inf(last_answer, -1);
    init_resultprinter(show_answer_callback, &last_answer);

    const char *answers[FORMAT_COUNT][5] = {
          {"-@Inf@", "-@Inf@", "-@Inf@", "-@Inf@", "-@Inf@"},
          {"-@Inf@", "-@Inf@", "-@Inf@", "-@Inf@", "-@Inf@"},
          {"-@Inf@", "-@Inf@", "-@Inf@", "-@Inf@", "-@Inf@"},
          {"-@Inf@", "-@Inf@", "-@Inf@", "-@Inf@", "-@Inf@"},
    };
    const int testCount[FORMAT_COUNT] = {5, 5, 5, 5};

    // WHEN
    for (enum rounding_style round = NO_ROUNDING_INDICATION; round < ROUNDING_INDICATION_COUNT;
         round++) {
        conf->rounding_indication = round;
        for (enum output_formats format = DECIMAL_FORMAT; format < FORMAT_COUNT; format++) {
            conf->output_format = format;
            basicPrecisionTest(tc, 2, answers[format], NULL, testCount[format]);
        }
    }
    num_free(last_answer);
}

static void GIVEN_zero_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    conf_t *conf = init_test_conf();
    num_init_set_d(last_answer, 0);
    init_resultprinter(show_answer_callback, &last_answer);

    const char *answers[FORMAT_COUNT][5] = {
          {"0", "0", "0", "0", "0"},
          {"0", "0", "0", "0", "0"},
          {"0", "0", "0", "0", "0"},
          {"0", "0", "0", "0", "0"},
    };
    const int testCount[FORMAT_COUNT] = {5, 5, 5, 5};

    // WHEN
    for (enum rounding_style round = NO_ROUNDING_INDICATION; round < ROUNDING_INDICATION_COUNT;
         round++) {
        conf->rounding_indication = round;
        for (enum output_formats format = DECIMAL_FORMAT; format < FORMAT_COUNT; format++) {
            conf->output_format = format;
            basicPrecisionTest(tc, 0, answers[format], NULL, testCount[format]);
        }
    }
    num_free(last_answer);
}

static void GIVEN_negZero_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    conf_t *conf = init_test_conf();
    num_init_set_d(last_answer, -0);
    init_resultprinter(show_answer_callback, &last_answer);

    const char *answers[FORMAT_COUNT][5] = {
          {"0", "0", "0", "0", "0"},
          {"0", "0", "0", "0", "0"},
          {"0", "0", "0", "0", "0"},
          {"0", "0", "0", "0", "0"},
    };
    const int testCount[FORMAT_COUNT] = {5, 5, 5, 5};

    // WHEN
    for (enum rounding_style round = NO_ROUNDING_INDICATION; round < ROUNDING_INDICATION_COUNT;
         round++) {
        conf->rounding_indication = round;
        for (enum output_formats format = DECIMAL_FORMAT; format < FORMAT_COUNT; format++) {
            conf->output_format = format;
            basicPrecisionTest(tc, 0, answers[format], NULL, testCount[format]);
        }
    }
    num_free(last_answer);
}

static void GIVEN_oneThird_WHEN_print_this_result_THEN_correctStringWithNadUnset(CuTest *tc)
{
    // GIVEN
    conf_t *conf = init_test_conf();
    num_init_set_d(last_answer, 1.0/3.0);
    init_resultprinter(show_answer_callback, &last_answer);

    const char *answers[FORMAT_COUNT][5] = {
          {"0.333333", "0", "0.3", "0.33", "0.333"},
          {"00.252525252525252525", "00", "00.2", "00.25", "00.252"},
          {"0x0.5555555555555", "0x0", "0x0.5", "0x0.55", "0x0.555"},
          {"0b0.0101010101010101010101010101", "0b0", "0b0.0", "0b0.01", "0b0.010"},
    };
    const bool expectedNad[FORMAT_COUNT][ROUNDING_INDICATION_COUNT][5] = {
          {{false, false, false, false, false}, // decimal, no round
           {true, true, true, true, true}, // decimal simple
           {false, true, true, true, true}}, // decimal sig fig
          {{false, false, false, false, false}, // octal no round
           {true, true, true, true, true}, // octal simple
           {true, true, true, true, true}}, // octal sig fig
          {{false, false, false, false, false}, // hex no round
           {true, true, true, true, true}, // hex simple
           {true, true, false, false, false}}, // hex sig fig
          {{false, false, false, false, false}, // binary no round
           {true, true, true, true, true}, // binary simple
           {true, true, true, true, true}}, // binary sig fig
    };
    const int testCount[FORMAT_COUNT] = {5, 5, 5, 2};

    // WHEN
    for (enum rounding_style round = NO_ROUNDING_INDICATION; round < ROUNDING_INDICATION_COUNT;
         round++) {
        conf->rounding_indication = round;
        for (enum output_formats format = DECIMAL_FORMAT; format < FORMAT_COUNT; format++) {
            conf->output_format = format;
            basicPrecisionTest(tc, 1, answers[format], expectedNad[format][round], testCount[format]);
        }
    }
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
