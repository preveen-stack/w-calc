#import "calculator.h"
#import "historyManager.h"
#import "PrefsController.h"

@implementation PrefsController

- (IBAction)setPrefs:(id)sender
{
	short need_redraw = 0;

	{
		short olde = engineering;
		engineering = ([engineeringNotation state] == NSOffState)?0:1;
		need_redraw = (olde != engineering);
	}
	picky_variables = ([pickyVariables state] == NSOffState)?0:1;
	allow_duplicates = ([historyDuplicates state] == NSOffState)?0:1;

	if (need_redraw) {
		print_this_result(last_answer);

		[answerField setStringValue:[NSString stringWithCString:pretty_answer]];

		[expressionField selectText:self];
	}
}

- (IBAction)showPrefs:(id)sender
{
	[engineeringNotation setState:(engineering?NSOnState:NSOffState)];
	[pickyVariables setState:(picky_variables?NSOnState:NSOffState)];
	[historyDuplicates setState:(allow_duplicates?NSOnState:NSOffState)];
	
	[thePrefPanel makeKeyAndOrderFront:self];
	[thePrefPanel center];
}

@end
