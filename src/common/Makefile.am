SUBDIRS = includes
AM_YFLAGS = -d
AM_CPPFLAGS = -I$(top_builddir)/src/common/includes \
			  -I$(top_builddir)/src/common \
			  @GMP_CPPFLAGS@ @MPFR_CPPFLAGS@
AM_LDFLAGS = @GMP_LDFLAGS@ @MPFR_LDFLAGS@

noinst_LIBRARIES = libwcalc.a

BUILT_SOURCES = parser.h

libwcalc_a_SOURCES = \
					 add_commas.c \
					 calculator.c \
					 conversion.c \
					 evalvar.c \
					 explain.c \
					 extract_vars.c \
					 files.c \
					 help.c \
					 historyManager.c \
					 iscmd.c \
					 isconst.c \
					 isfunc.c \
					 list.c \
					 number.c \
					 number_formatting.c \
					 parser.y \
					 scanner.l \
					 string_manip.c \
					 variables.c
