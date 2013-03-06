SUBDIRS = English.lproj src
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
			 Wcalc.xcodeproj/kyle.mode1 \
			 Wcalc.xcodeproj/kyle.pbxuser \
			 Wcalc.xcodeproj/project.pbxproj \
			 Info.plist \
			 test.errors test.recursion test.simple test.vars \
			 TheW.icns w.png Wred.png \
			 wcalc.info wcalc.rc wcalc.spec \
			 COPYRIGHT

man_MANS = wcalc.1
EXTRA_DIST += $(man_MANS)
clean-local:
	$(RM) parser.h parser.c scanner.c
