SUBDIRS = English.lproj src
ACLOCAL_AMFLAGS = -I config
EXTRA_DIST = \
			 Wcalc.xcodeproj/kyle.mode1 \
			 Wcalc.xcodeproj/kyle.pbxuser \
			 Wcalc.xcodeproj/project.pbxproj \
			 Info.plist \
			 test.errors test.recursion test.simple test.vars \
			 graphics/TheW.icns graphics/w.png graphics/Wred.png \
			 wcalc.info wcalc.rc wcalc.spec \
			 COPYRIGHT

man_MANS = wcalc.1
EXTRA_DIST += $(man_MANS)
clean-local:
	$(RM) parser.h parser.c scanner.c
