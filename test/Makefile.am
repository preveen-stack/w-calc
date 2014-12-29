AUTOMAKE_OPTIONS = parallel-tests

TEST_EXTENSIONS = .test

TESTS = basic_ints.test \
		basic_float.test \
		basic_vars.test \
		basic_math.test \
		implicit_mult.test \
		precision_guard.test \
		conversion.test \
		website_math.test \
		factorial.test \
		formatting.test

EXTRA_DIST = checktest.sh

TEST_LOG_COMPILER = $(top_srcdir)/test/checktest.sh
AM_TEST_LOG_FLAGS = "$(top_builddir)/src/wcalc"
