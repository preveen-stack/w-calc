/* PrefsController */

#import <Cocoa/Cocoa.h>

@interface PrefsController : NSObject
{
    IBOutlet NSTextField *answerField;
    IBOutlet NSButton *engineeringNotation;
    IBOutlet NSTextField *expressionField;
    IBOutlet NSButton *historyDuplicates;
    IBOutlet NSButton *pickyVariables;
    IBOutlet NSPanel *thePrefPanel;
}
- (IBAction)setPrefs:(id)sender;
- (IBAction)showPrefs:(id)sender;
@end
