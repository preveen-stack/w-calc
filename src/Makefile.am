AM_YFLAGS = -d
AM_CPPFLAGS = -I$(top_srcdir)/src/common/includes \
	      -I$(top_builddir)/src/common/includes \
	      -I$(top_builddir)/src/common \
	      -I$(top_srcdir)/src/common \
	      @GMP_CPPFLAGS@ @MPFR_CPPFLAGS@
AM_LDFLAGS = @GMP_LDFLAGS@ @MPFR_LDFLAGS@

UNIT_TESTS =

include common/Makefile.inc
include gui/Makefile.inc

bin_PROGRAMS = wcalc
wcalc_LDADD = libwcalc.a @LEXLIB@
wcalc_SOURCES = cli/main.c cli/help.c

TESTS = AllUnitTests

check_PROGRAMS = $(TESTS)

AllUnitTests_SOURCES = AllUnitTests.c \
					   $(UNIT_TESTS) \
					   common/CuTest.c
AllUnitTests_LDADD = libwcalc.a
