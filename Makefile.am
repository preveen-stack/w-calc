bin_PROGRAMS = wcalc
AM_YFLAGS = -dt
#AM_LFLAGS = -w
wcalc_SOURCES = calculator.h variables.h string_manip.h help.h parser.y \
		string_manip.h historyManager.h files.h conversion.h \
		scanner.l help.c calculator.c string_manip.c historyManager.c \
		files.c variables.c conversion.c main.c number_formatting.c \
		explain.h explain.c \
		list.h list.c \
		extract_vars.h extract_vars.c \
		isconst.h isconst.c \
		isfunc.h isfunc.c \
		iscmd.h iscmd.c \
		add_commas.h add_commas.c \
		number.h number.c \
		evalvar.h evalvar.c
wcalc_LDADD = @LEXLIB@
man_MANS = wcalc.1
clean-local:
	rm -f parser.c parser.h scanner.c
maintainer-clean-local:
	rm -f parser.h
distclean-local:
	rm -rf CVS
	rm -rf English.lproj/CVS
	rm -rf English.lproj/MainMenu.nib/CVS
	rm -rf Wcalc.xcodeproj/CVS
	rm -f .*.swp *~ ".#"*
	find . -name .DS_Store -exec rm -f {} \;
	find . -name .cvsignore -exec rm -f {} \;
	rm -f .gdb_history
	rm -rf build autom4te.cache
