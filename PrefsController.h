/* PrefsController */

#import <Cocoa/Cocoa.h>
#import "WcalcController.h"

@interface PrefsController : NSObject
{
    IBOutlet NSTextField *answerField;
    IBOutlet NSButton *engineeringNotation;
    IBOutlet NSTextField *expressionField;
    IBOutlet NSButton *historyDuplicates;
    IBOutlet NSButton *pickyVariables;
    IBOutlet NSPanel *thePrefPanel;
	IBOutlet NSButton *useRadians;
	IBOutlet WcalcController *mainController;
}
- (IBAction)setPrefs:(id)sender;
- (IBAction)showPrefs:(id)sender;
@end
