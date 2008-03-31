bin_PROGRAMS = wcalc
AM_YFLAGS = -d
#AM_LFLAGS = -w
EXTRA_DIST = WcalcController.h WcalcController.m \
			 AboutBoxController.h AboutBoxController.m \
			 ConversionList.h ConversionList.m \
			 ErrorController.h ErrorController.m \
			 HistoryList.h HistoryList.m \
			 InspectorController.h InspectorController.m \
			 MyTextField.h MyTextField.m \
			 OldVariableList.h OldVariableList.m \
			 PersVarList.h PersVarList.m \
			 VariableList.h VariableList.m \
			 WcalcService.h WcalcService.m \
			 theDelegate.h theDelegate.m \
			 main.m \
			 simpleCalc.h simpleCalc.c \
			 English.lproj/InfoPlist.strings \
			 English.lproj/MainMenu.nib/classes.nib \
			 English.lproj/MainMenu.nib/info.nib \
			 English.lproj/MainMenu.nib/keyedobjects.nib \
			 Wcalc.xcodeproj/kyle.mode1 \
			 Wcalc.xcodeproj/kyle.pbxuser \
			 Wcalc.xcodeproj/project.pbxproj \
			 Info.plist \
			 test.errors test.recursion test.simple test.vars \
			 TheW.icns w.png Wred.png \
			 wcalc.info wcalc.rc wcalc.spec \
			 COPYRIGHT

INCLUDES = -I$(top_builddir)
wcalc_SOURCES = \
				number.h calculator.h variables.h help.h files.h conversion.h \
				number_formatting.h \
				parser.y \
		string_manip.h historyManager.h \
		definitions.h \
		uint32_max.h \
		number_formatting.h number_formatting.c \
		scanner.l help.c calculator.c string_manip.c historyManager.c \
		files.c variables.c conversion.c main.c \
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
EXTRA_DIST += $(man_MANS)
clean-local:
	$(RM) parser.h parser.c scanner.c
