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
	IBOutlet NSButton *printPrefixes;
    IBOutlet NSPanel *thePrefPanel;
	IBOutlet NSButton *useRadians;
	IBOutlet NSMatrix *outputFormat;
	IBOutlet NSSlider *precisionSlider;
	IBOutlet NSTableView *historyList;
	IBOutlet WcalcController *mainController;
}
- (IBAction)setPrefs:(id)sender;
- (IBAction)showPrefs:(id)sender;
@end
