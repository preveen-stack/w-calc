AC_DEFUN([WCALC_EDITLINE], [
    AC_CHECK_HEADER([histedit.h],
                    [wcalc_cv_editline=yes],
                    [wcalc_cv_editline=no])
    AS_IF([test "x$wcalc_cv_editline" = xyes],
          [AC_SEARCH_LIBS([el_init],
                          [edit editline],
                          [],[wcalc_cv_editline=no])])
    AS_IF([test "x$wcalc_cv_editline" = xno],
          [$2])
    AS_IF([test "x$ac_cv_search_el_init" != "xnone required"],
          [AC_MSG_CHECKING([if $ac_cv_search_el_init is loadable])
           AC_RUN_IFELSE([AC_LANG_PROGRAM()],
                         [AC_MSG_RESULT([yes])],
                         [AC_MSG_RESULT([no])
                          AC_MSG_ERROR([$ac_cv_search_el_init cannot be located by the loader! You may need to modify your LD_LIBRARY_PATH or your /etc/ld.so.conf])])
                          ])
    AC_DEFINE([HAVE_EDITLINE], 1, [Define if editline is available])
])
