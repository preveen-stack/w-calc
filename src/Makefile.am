SUBDIRS = common gui .

AM_CPPFLAGS = -I$(top_srcdir)/src/common/includes \
	      -I$(top_builddir)/src/common/includes \
	      -I$(top_builddir)/src/common \
	      -I$(top_srcdir)/src/common \
	      @GMP_CPPFLAGS@ @MPFR_CPPFLAGS@
AM_LDFLAGS = @GMP_LDFLAGS@ @MPFR_LDFLAGS@

bin_PROGRAMS = wcalc
wcalc_LDADD = common/libwcalc.a @LEXLIB@
wcalc_SOURCES = cli/main.c

common/libwcalc.a:
	$(MAKE) -C common libwcalc.a
