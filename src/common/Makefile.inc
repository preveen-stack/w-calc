noinst_LIBRARIES = libwcalc.a

include %D%/includes/Makefile.inc

BUILT_SOURCES = %D%/parser.h

MORE_BUILT_SRC = \
			 %D%/parser.c \
			 %D%/scanner.c

libwcalc_a_SOURCES = \
		     %D%/add_commas.c \
		     %D%/calculator.c \
			 %D%/conf.c \
		     %D%/conversion.c \
		     %D%/evalvar.c \
		     %D%/explain.c \
		     %D%/extract_vars.c \
		     %D%/files.c \
		     %D%/historyManager.c \
		     %D%/iscmd.c \
		     %D%/isconst.c \
		     %D%/isfunc.c \
		     %D%/list.c \
		     %D%/number.c \
		     %D%/number_formatting.c \
		     %D%/parser.y \
		     %D%/scanner.l \
		     %D%/result_printer.c \
		     %D%/string_manip.c \
		     %D%/variables.c

clean-local:
	$(RM) $(BUILT_SOURCES) $(MORE_SRC)

UNIT_TESTS += \
			  %D%/add_commas.test.c \
			  %D%/extract_vars.test.c \
			  %D%/iscmd.test.c \
			  %D%/isfunc.test.c \
			  %D%/string_manip.test.c \
			  %D%/result_printer.test.c \
			  %D%/evalvar.test.c

# vim:ft=automake
