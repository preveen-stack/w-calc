#import "calculator.h"
#import "historyManager.h"
#import "PrefsController.h"

@implementation PrefsController

- (IBAction)setPrefs:(id)sender
{
	short need_redraw = 0;
	short olde;
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];

	switch ([sender tag]) {
		case 1: // Flag Undefined Variables
			olde = picky_variables;
			picky_variables = ([pickyVariables state] == NSOffState)?0:1;
			if (olde != picky_variables) {
				[prefs setObject:(picky_variables?@"YES":@"NO") forKey:@"flagUndefinedVariables"];
			}
				break;
		case 2: // Use Radians
			olde = use_radians;
			use_radians = ([useRadians state] == NSOffState)?0:1;
			if (olde != use_radians) {
				need_redraw = 2;
				[prefs setObject:(use_radians?@"YES":@"NO") forKey:@"useRadians"];
			}
			break;
		case 3: // Use Engineering Notation
			olde = engineering;
			engineering = ([engineeringNotation state] == NSOffState)?0:1;
			if (olde != engineering) {
				need_redraw = 1;
				[prefs setObject:(engineering?@"YES":@"NO") forKey:@"engineeringNotation"];
			}				
			break;
		case 5: // Output Format
			olde = output_format;
			output_format = [[sender selectedCell] tag];
			if (olde != output_format) {
				need_redraw = 1;
				[prefs setObject:[NSString stringWithFormat:@"%i",output_format] forKey:@"outputFormat"];
				[precisionSlider setEnabled:(output_format==DECIMAL_FORMAT)];
				[printPrefixes setEnabled:(output_format!=DECIMAL_FORMAT)];
				[engineeringNotation setEnabled:(output_format==DECIMAL_FORMAT)];
			}
			break;
		case 6: // Print Prefixes
			olde = print_prefixes;
			print_prefixes = ([sender state]==NSOnState);
			if (olde != print_prefixes) {
				need_redraw = 1;
				[prefs setObject:(print_prefixes?@"YES":@"NO") forKey:@"printPrefixes"];
			}
		case 4: // Allow Duplicates in History
			olde = allow_duplicates;
			allow_duplicates = ([historyDuplicates state] == NSOffState)?0:1;
			if (olde != allow_duplicates) {
				[prefs setObject:(allow_duplicates?@"YES":@"NO") forKey:@"historyDuplicatesAllowed"];
			}				
			break;
		default: return;
	}

	switch (need_redraw) {
		case 1:
			if (pretty_answer) free(pretty_answer);
			pretty_answer = strdup(print_this_result(last_answer));

			[answerField setStringValue:[NSString stringWithCString:pretty_answer]];
			[historyList reloadData];

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
	[outputFormat selectCellWithTag:output_format];
	[printPrefixes setState:(print_prefixes?NSOnState:NSOffState)];
	[printPrefixes setEnabled:(output_format!=DECIMAL_FORMAT)];
	[engineeringNotation setEnabled:(output_format==DECIMAL_FORMAT)];

    [thePrefPanel makeKeyAndOrderFront:self];
    [thePrefPanel center];	
}

@end
