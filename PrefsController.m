#import "calculator.h"
#import "historyManager.h"
#import "PrefsController.h"

@implementation PrefsController

- (IBAction)setPrefs:(id)sender
{
	short need_redraw = 0;
	short olde;
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];

	olde = engineering;
	engineering = ([engineeringNotation state] == NSOffState)?0:1;
	if (olde != engineering) {
		need_redraw = 1;
		[prefs setObject:(engineering?@"YES":@"NO") forKey:@"engineeringNotation"];
	}

	olde = use_radians;
	use_radians = ([useRadians state] == NSOffState)?0:1;
	if (olde != use_radians) {
		need_redraw = 2;
		[prefs setObject:(use_radians?@"YES":@"NO") forKey:@"useRadians"];
	}

	olde = picky_variables;
    picky_variables = ([pickyVariables state] == NSOffState)?0:1;
	if (olde != picky_variables) {
		[prefs setObject:(picky_variables?@"YES":@"NO") forKey:@"flagUndefinedVariables"];
	}

	olde = allow_duplicates;
    allow_duplicates = ([historyDuplicates state] == NSOffState)?0:1;
	if (olde != allow_duplicates) {
		[prefs setObject:(allow_duplicates?@"YES":@"NO") forKey:@"historyDuplicatesAllowed"];
	}

	switch (need_redraw) {
		case 1:
			print_this_result(last_answer);

			[answerField setStringValue:[NSString stringWithCString:pretty_answer]];

			[expressionField selectText:self];
			break;
		case 2:
			[mainController go:sender];
			break;
    }	
}

- (IBAction)showPrefs:(id)sender
{
	[engineeringNotation setState:(engineering?NSOnState:NSOffState)];
    [pickyVariables setState:(picky_variables?NSOnState:NSOffState)];
    [historyDuplicates setState:(allow_duplicates?NSOnState:NSOffState)];
	[useRadians setState:(use_radians?NSOnState:NSOffState)];

    [thePrefPanel makeKeyAndOrderFront:self];
    [thePrefPanel center];	
}

@end
