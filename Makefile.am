bin_PROGRAMS = wcalc
AM_YFLAGS = -dt
wcalc_SOURCES = calculator.h variables.h string_manip.h help.h parser.y string_manip.h historyManager.h files.h conversion.h scanner.l help.c calculator.c string_manip.c historyManager.c files.c variables.c conversion.c main.c number_formatting.c
AM_CFLAGS = -O2 -Wall
wcalc_LDADD = @LEXLIB@
man_MANS = wcalc.1
clean-local:
	rm -f parser.c parser.h scanner.c
distclean-local:
	rm -rf CVS
	rm -rf English.lproj/CVS
	rm -rf English.lproj/MainMenu.nib/CVS
	rm -rf Wcalc.xcodeproj/CVS
	rm -f .*.swp
	find . -name .DS_Store -exec rm -f {} \;
	rm -f .gdb_history
