# Process this file with autoconf to produce a configure script.

## --------------------- ##
## Autoconf Requirements ##
## --------------------- ##
AC_PREREQ(2.59)

## ---------------------------------- ##
## Autoconf / Automake Initialization ##
## ---------------------------------- ##
AC_INIT([wcalc], [2.5.1], [kyle-wcalc@memoryhole.net])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([config])
AC_CONFIG_SRCDIR([src/cli/main.c])

dnl Require at least AM 1.7.  Prior versions require AM_CONFIG_HEADER
dnl and have no equivalent to AC_CONFIG_HEADERS (the plural version),
dnl which is a pain.  AM 1.7 also added support for putting target
dnl information in AM_CONDITIONAL blocks and using the += modifier.
dnl ... removed "check-news" because of automatic version generation
# Automake's silent rules were implemented in the same version that
# color-tests was implemented, so we can use one to detect the other.
# This nasty, dirty, unreliable trick is strongly discouraged by its author:
# http://blog.flameeyes.eu/trackbacks?article_id=5155
m4_ifdef([AM_SILENT_RULES],
		 [m4_define([wc_color_tests], [color-tests])],
		 [m4_define([wc_color_tests], [])])
AM_INIT_AUTOMAKE([foreign subdir-objects dist-bzip2 no-define 1.7 ]wc_color_tests)
# If Automake supports silent rules, enable them
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])


AC_ARG_ENABLE([rpath],
			  [AS_HELP_STRING([--enable-rpath],
							  [Add -rpath arguments to LDFLAGS])])
AC_ARG_ENABLE([picky],
              [AS_HELP_STRING([--enable-picky],
                              [turns on extra compiler warnings (for developers)])])
AS_IF([test "x$enable_picky" = x],
      [AS_IF([test -d "${srcdir}/.svn"],
             [AC_MSG_NOTICE([--> developer override: enable picky compiler by default <--])
             enable_picky=yes])])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_RANLIB
AC_PROG_YACC
AM_PROG_LEX
AS_IF([test "x$LEX" == x],
	  [AC_MSG_ERROR([A Lex/Flex program is required in order to compile wcalc.])],
	  [AS_IF([test "x$LEX" != xflex],
			 [LEX="$SHELL $missing_dir/missing flex"
			  AC_SUBST([LEX_OUTPUT_ROOT], [lex.yy])
			  AC_SUBST([LEXLIB], [''])]
			 )]
	  )
AC_PROG_CPP
AC_PROG_INSTALL

AS_IF([test "x$enable_picky" = xyes],
	  [CFLAGS="$CFLAGS -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wmissing-prototypes -Wstrict-prototypes"])

CPPFLAGS="$CPPFLAGS -D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=200112L"

AC_CHECK_HEADERS([limits.h])
AC_CHECK_DECL([PATH_MAX],
			  [AC_DEFINE([HAVE_PATH_MAX], [1], [Have PATH_MAX])],
			  [],
			  [#ifdef HAVE_LIMITS_H
			   #include <limits.h>
			   #endif
			   ])

# Checks for libraries.
AC_CHECK_LIB([m], [acos])
AC_ARG_WITH([readline],
			[AS_HELP_STRING([--without-readline],
							[do not try to find and use the readline library])],
			[wcalc_cv_readline=$withval],
			[wcalc_cv_readline=yes])
AC_ARG_WITH([editline],
            [AS_HELP_STRING([--with-editline],
                            [attempt to use the editline library rather than the readline library])],
            [wcalc_cv_editline=$withval],
            [wcalc_cv_editline=unspec])
AS_IF([test "x$wcalc_cv_readline" != xno -a "x$wcalc_cv_editline" != xyes],
	  [AX_LIB_READLINE])
case "$ac_cv_lib_readline" in
    -ledit*)
    wcalc_cv_editline=yes
    ;;
esac
AS_IF([test "x$wcalc_cv_editline" = xyes -o "x$ac_cv_lib_readline" = x],
      [WCALC_EDITLINE])

#==========================================================================================
# GMP
#==========================================================================================
AC_ARG_WITH([gmp],
			[AS_HELP_STRING([--with-gmp=[[PATH]]],
							[specify the path to the gmp library])],
			[AS_IF([test "x$enable_rpath" = xyes],
				   [GMP_CPPFLAGS="-I$with_gmp/include"
					GMP_LDFLAGS="-L$with_gmp/lib -Wl,-rpath,$with_gmp/lib"],
				   [GMP_CPPFLAGS="-I$with_gmp/include"
					GMP_LDFLAGS="-L$with_gmp/lib"])])
saved_CPPFLAGS="$CPPFLAGS"
saved_LDFLAGS="$LDFLAGS"
CPPFLAGS="$GMP_CPPFLAGS $CPPFLAGS"
LDFLAGS="$GMP_LDFLAGS $LDFLAGS"
AC_CHECK_LIB([gmp], [__gmpz_init], [],
			 [AC_CHECK_LIB([gmp], [mpz_init], [],
						   [AC_CHECK_LIB([gmp2], [mpz_init])])])
CPPFLAGS="$saved_CPPFLAGS"
LDFFLAGS="$saved_LDFLAGS"
AC_SUBST(GMP_CPPFLAGS)
AC_SUBST(GMP_LDFLAGS)

#==========================================================================================
# MPFR
#==========================================================================================
AC_ARG_WITH([mpfr],
			[AS_HELP_STRING([--with-mpfr=[[PATH]]],
							[specify the path to the mpfr library])],
			[AS_IF([test "x$enable_rpath" = xyes],
				   [MPFR_CPPFLAGS="-I$with_mpfr/include"
					MPFR_LDFLAGS="-L$with_mpfr/lib -Wl,-rpath,$with_mpfr/lib"],
				   [MPFR_CPPFLAGS="-I$with_mpfr/include"
					MPFR_LDFLAGS="-L$with_mpfr/lib"])])
saved_CPPFLAGS="$CPPFLAGS"
saved_LDFLAGS="$LDFLAGS"
CPPFLAGS="$MPFR_CPPFLAGS $GMP_CPPFLAGS $CPPFLAGS"
LDFLAGS="$MPFR_LDFLAGS $GMP_LDFLAGS $LDFLAGS"
AC_SEARCH_LIBS([mpfr_printf], [mpfr],
			   [AC_DEFINE([HAVE_LIBMPFR], [1], [Have libmpfr])],
			   [AC_MSG_ERROR([Need MPFR 2.4.x or better. see http://www.mpfr.org])])
AC_SUBST(MPFR_CPPFLAGS)
AC_SUBST(MPFR_LDFLAGS)
CPPFLAGS="$saved_CPPFLAGS"
LDFFLAGS="$saved_LDFLAGS"
# what version of mpfr
#==========================================================================================

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_CHECK_HEADERS([fcntl.h float.h inttypes.h limits.h stddef.h stdint.h stdlib.h string.h sys/time.h unistd.h stdio.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([gettimeofday memset modf pow stpcpy strchr strdup strerror strrchr strtoul])

AC_CONFIG_HEADER([src/common/includes/config.h])
AC_CONFIG_FILES([Makefile
				 src/Makefile
                 test/Makefile
				 English.lproj/Makefile])
AC_OUTPUT

