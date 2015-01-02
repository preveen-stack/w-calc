AUTOMAKE_OPTIONS = parallel-tests

TEST_EXTENSIONS = .test

POSITIVE_TESTS = \
				 basic_ints.test \
				 basic_float.test \
				 basic_vars.test \
				 basic_math.test \
				 implicit_mult.test \
				 precision_guard.test \
				 conversion.test \
				 website_math.test \
				 factorial.test \
				 formatting.test \
				 active_variable.test \
				 vars.test

XFAIL_TESTS = \
			  err.assert.test \
			  err.question.test \
			  err.assign.test \
			  err.conv_category1.test \
			  err.conv_category2.test \
			  err.recursion1.test \
			  err.recursion2.test \
			  err.recursion3.test \
			  err.recursion4.test

TESTS = $(POSITIVE_TESTS) $(XFAIL_TESTS)

EXTRA_DIST = checktest.sh $(POSITIVE_TESTS) $(XFAIL_TESTS)

TEST_LOG_COMPILER = $(top_srcdir)/test/checktest.sh
AM_TEST_LOG_FLAGS = "$(top_builddir)/src/wcalc"
