/* WcalcController */
#ifndef WCALC_CONTROLLER_H
#define WCALC_CONTROLLER_H

#include <Cocoa/Cocoa.h>
#include "InspectorController.h"

@interface WcalcController : NSObject
{
    IBOutlet NSMenuItem *affectKeyboardMenu;
    IBOutlet NSMenuItem *baseMenu;
    IBOutlet NSMenuItem *saveMenuItem;
    IBOutlet NSMenuItem *saveAsMenuItem;
    IBOutlet NSMenuItem *sizeToggleMenu;

    IBOutlet NSWindow *mainWindow;
    IBOutlet NSWindow *inspectorWindow;
    IBOutlet NSWindow *persVarsWindow;
    IBOutlet InspectorController *inspector;

    IBOutlet NSButton *capsLockKey;
    IBOutlet NSButton *shiftKey1;
    IBOutlet NSButton *shiftKey2;
    IBOutlet NSButton *engineeringNotation;
    IBOutlet NSButton *historyDuplicates;
    IBOutlet NSButton *pickyVariables;
    IBOutlet NSButton *printPrefixes;
    IBOutlet NSButton *useRadians;
    IBOutlet NSButton *updateHistory;
    IBOutlet NSButton *strictSyntax;
    IBOutlet NSButton *decimalKey;
    IBOutlet NSButton *rememberErrors;
    IBOutlet NSButton *enterKey;
    IBOutlet NSButton *limitHistory;
    IBOutlet NSButton *precisionGuard;
    IBOutlet NSButton *printInts;
    IBOutlet NSButton *simpleCalculator;
    IBOutlet NSButton *printDelimiters;
    IBOutlet NSButton *livePrecision;
    IBOutlet NSButton *cModPref;

    IBOutlet NSTextField *AnswerField;
    IBOutlet NSTextField *ExpressionField;
    IBOutlet NSTextField *limitHistoryLen;
    IBOutlet NSTextField *limitHistoryLenTag;
    IBOutlet NSTextField *bitsPref;

    IBOutlet NSDrawer *baseDrawer;
	
    IBOutlet NSStepper *bitsStepper;

    IBOutlet NSMatrix *outputFormat;
    IBOutlet NSMatrix *outputFormat2;

    IBOutlet NSSlider *PrecisionSlider;
    
    IBOutlet id errorController;
    IBOutlet id historyList;
    IBOutlet id theKeyboard;
    IBOutlet id keypad;

    IBOutlet NSPanel *thePrefPanel;
    IBOutlet NSPanel *conversionWindow;

    IBOutlet NSPopUpButton *roundingIndication;
    IBOutlet NSPopUpButton *convertType;

    IBOutlet NSTableView *variableList;
    IBOutlet NSTableView *convertFrom;
    IBOutlet NSTableView *convertTo;
	
    IBOutlet NSApplication *meta;

    bool just_answered;
    NSView *superview;
}
- (IBAction)clear:(id)sender;
- (IBAction)enterData:(id)sender;
- (IBAction)go:(id)sender;
- (IBAction)setPrecision:(id)sender;
- (IBAction)showKeyboardDrawer:(id)sender;
- (IBAction)toggleSize:(id)sender;
- (IBAction)menuFunction:(id)sender;
- (IBAction)menuConstant:(id)sender;
- (IBAction)setPrefs:(id)sender;
- (IBAction)showPrefs:(id)sender;
- (IBAction)displayPrefs:(id)sender;
- (IBAction)showBaseDrawer:(id)sender;
- (IBAction)shConversions:(id)sender;
- (IBAction)convert:(id)sender;
- (IBAction)open:(id)sender;
- (IBAction)save:(id)sender;
- (IBAction)saveAs:(id)sender;
- (IBAction)closeWindow:(id)sender;
- (IBAction)quit:(id)sender;

- (void)displayAnswer;
- (void)displayErrno:(int)err forFile:(NSString*)filename;
- (BOOL)validateMenuItem:(NSMenuItem *)anItem;
@end
#endif
