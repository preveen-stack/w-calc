bin_PROGRAMS = wcalc
AM_YFLAGS = -dt
wcalc_SOURCES = calculator.h variables.h string_manip.h help.h parser.y string_manip.h historyManager.h files.h scanner.l help.c calculator.c string_manip.c historyManager.c files.c variables.c main.c
CFLAGS = -O2 -Wall @CFLAGS@
wcalc_LDADD = @LEXLIB@
man_MANS = wcalc.1
