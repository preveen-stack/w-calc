/* WcalcController */

#import <Cocoa/Cocoa.h>

@interface WcalcController : NSObject
{
    IBOutlet NSMenuItem *affectDrawerMenu;
    IBOutlet NSMenuItem *affectKeyboardMenu;
	IBOutlet NSMenuItem *baseMenu;
	
	IBOutlet NSWindow *mainWindow;
	
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
	
    IBOutlet NSTextField *AnswerField;
    IBOutlet NSTextField *ExpressionField;
	
    IBOutlet NSDrawer *theDrawer;
	IBOutlet NSDrawer *baseDrawer;
	
	IBOutlet NSMatrix *outputFormat;
	IBOutlet NSMatrix *outputFormat2;
	
    IBOutlet id errorController;
    IBOutlet id historyList;
    IBOutlet id PrecisionSlider;
    IBOutlet id theKeyboard;
	IBOutlet id keypad;
	
	IBOutlet NSPanel *thePrefPanel;
	IBOutlet NSPanel *conversionWindow;
	
	IBOutlet NSPopUpButton *roundingIndication;
	IBOutlet NSPopUpButton *convertType;

	IBOutlet NSTableView *variableList;
	IBOutlet NSTableView *convertFrom;
	IBOutlet NSTableView *convertTo;
	
	bool just_answered;
	NSView *superview;
}
- (IBAction)clear:(id)sender;
- (IBAction)enterData:(id)sender;
- (IBAction)go:(id)sender;
- (IBAction)setPrecision:(id)sender;
- (IBAction)showInspectorDrawer:(id)sender;
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
@end
