/* WcalcController */

#import <Cocoa/Cocoa.h>

@interface WcalcController : NSObject
{
    IBOutlet NSMenuItem *affectDrawerMenu;
    IBOutlet NSMenuItem *affectKeyboardMenu;
    IBOutlet id AnswerField;
    IBOutlet NSButton *capsLockKey;
    IBOutlet id errorController;
    IBOutlet NSTextField *ExpressionField;
    IBOutlet id historyList;
    IBOutlet id PrecisionSlider;
    IBOutlet NSButton *shiftKey1;
    IBOutlet NSButton *shiftKey2;
    IBOutlet NSDrawer *theDrawer;
    IBOutlet id theKeyboard;
    IBOutlet id variableList;
	IBOutlet NSWindow *mainWindow;
	IBOutlet id keypad;

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
@end
