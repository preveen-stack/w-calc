#import "calculator.h"
#import "variables.h"
#import "ErrorController.h"
#import "historyManager.h"
#import "WcalcController.h"

#define KEYPAD_HEIGHT 165
#define MIN_WINDOW_WIDTH 171
#define MIN_WINDOW_HEIGHT_TOGGLED 118
#define MIN_WINDOW_HEIGHT_UNTOGGLED 283
#define FIELD_WIDTH_DIFFERENCE 22
#define MAX_WINDOW_SIZE 10000

static char update_history = 0;

@implementation WcalcController

- (IBAction)toggleSize:(id)sender
{
	static BOOL shrinking = TRUE;
	NSRect f = [mainWindow frame], e = [ExpressionField frame], p = [PrecisionSlider frame];
	NSSize w;
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];


	if (shrinking) {
		f.size.height -= KEYPAD_HEIGHT;
		f.origin.y += KEYPAD_HEIGHT;
		e.size.height = f.size.height - 100;
	} else {
		f.size.height += KEYPAD_HEIGHT;
		f.origin.y -= KEYPAD_HEIGHT;
		f.size.width = MIN_WINDOW_WIDTH;
	}

	e.size.width = p.size.width = f.size.width - FIELD_WIDTH_DIFFERENCE;
	
	if (shrinking) {
		[keypad removeFromSuperview];
		e.origin.y -= KEYPAD_HEIGHT;
		p.origin.y -= KEYPAD_HEIGHT;
	} else {
		e.origin.y += KEYPAD_HEIGHT;
		p.origin.y += KEYPAD_HEIGHT;
	}
	
	[ExpressionField removeFromSuperview];
	[PrecisionSlider removeFromSuperview];

	if (sender != 0)
		[mainWindow setFrame:f display:TRUE animate:TRUE];
	else
		[mainWindow setFrame:f display:FALSE animate:FALSE];

	if (! shrinking) {
		[superview addSubview:keypad];
		w.width = MIN_WINDOW_WIDTH;
		w.height = MIN_WINDOW_HEIGHT_UNTOGGLED;
		[mainWindow setMinSize:w];
		w.width = MIN_WINDOW_WIDTH;
		w.height = MAX_WINDOW_SIZE;
		[mainWindow setMaxSize:w];
		[prefs setObject:@"NO" forKey:@"toggled"];
	} else {
		w.width = 0;
		w.height = MIN_WINDOW_HEIGHT_TOGGLED;
		[mainWindow setMinSize:w];
	w = [mainWindow minSize];
		w.width = MAX_WINDOW_SIZE;
		w.height = MAX_WINDOW_SIZE;
		[mainWindow setMaxSize:w];
	w = [mainWindow maxSize];
		[prefs setObject:@"YES" forKey:@"toggled"];
	}
	
	[ExpressionField setFrame:e];
	[PrecisionSlider setFrame:p];

	[superview addSubview:ExpressionField];
	[superview addSubview:PrecisionSlider];

	[ExpressionField selectText:sender];

	shrinking = ! shrinking;
}

- (IBAction)clear:(id)sender
{
	if ([[ExpressionField stringValue] cStringLength]) {
		[ExpressionField setStringValue:@""];
		[ExpressionField selectText:self];
	} else if ([[AnswerField stringValue] cStringLength]) {
		[AnswerField setStringValue:@""];
		[ExpressionField selectText:self];
	}
}

- (IBAction)menuConstant:(id)sender
{
	NSString *str = [ExpressionField stringValue];
	NSString *str2;
	
	switch ([sender tag]) {
		case 1: str2 = @"¹"; break;
		case 2: str2 = @"e"; break;
		case 3: str2 = @"Na"; break;
		case 4: str2 = @"k"; break;
		case 5: str2 = @"Cc"; break;
		case 6: str2 = @"ec"; break;
		case 7: str2 = @"R"; break;
		case 8: str2 = @"G"; break;
		case 9: str2 = @"g"; break;
		case 10: str2 = @"Me"; break;
		case 11: str2 = @"Mp"; break;
		case 12: str2 = @"u"; break;
		case 13: str2 = @"c"; break;
		case 14: str2 = @"µ0"; break;
		case 15: str2 = @"random"; break;
		case 16: str2 = @"u"; break;
		case 17: str2 = @"Mn"; break;
		case 18: str2 = @"epsilon0"; break;
		case 19: str2 = @"Md"; break;
		case 20: str2 = @"alpha"; break;
		case 21: str2 = @"µB"; break;
		case 22: str2 = @"µN"; break;
		case 23: str2 = @"b"; break;
		case 24: str2 = @"sigma"; break;
		case 25: str2 = @"mW"; break;
		case 26: str2 = @"mZ"; break;
		case 27: str2 = @"gamma"; break;
		case 28: str2 = @"eV"; break;
		case 29: str2 = @"ao"; break;
		case 30: str2 = @"F"; break;
		case 31: str2 = @"Vm"; break;
		case 32: str2 = @"re"; break;
		default: return;
	}
	if ([str length]) {
		NSString *str3 = @"*";
		[ExpressionField setStringValue:[str stringByAppendingString:[str3 stringByAppendingString:str2]]];
	} else {
		[ExpressionField setStringValue:str2];
	}
}

- (IBAction)menuFunction:(id)sender
{
	NSString *str = [[ExpressionField stringValue] stringByAppendingString:@")"];
	NSString *str2;
	
	if ([str length] == 1) str = @"";
	
	switch ([sender tag]) {
		case 1: str2 = @"sin("; break;
		case 2: str2 = @"cos("; break;
		case 3: str2 = @"tan("; break;
		case 4: str2 = @"asin("; break;
		case 5: str2 = @"acos("; break;
		case 6: str2 = @"atan("; break;
		case 7: str2 = @"sinh("; break;
		case 8: str2 = @"cosh("; break;
		case 9: str2 = @"tanh("; break;
		case 10: str2 = @"asinh("; break;
		case 11: str2 = @"acosh("; break;
		case 12: str2 = @"atanh("; break;
		case 13: str2 = @"log("; break;
		case 14: str2 = @"ln("; break;
		case 15: str2 = @"round("; break;
		case 16: str2 = @"abs("; break;
		case 17: str2 = @"sqrt("; break;
		case 18: str2 = @"floor("; break;
		case 19: str2 = @"ceil("; break;
		default: return;
	}
	[ExpressionField setStringValue:[str2 stringByAppendingString:str]];
}

- (void)awakeFromNib
{
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
	NSRect w;
	NSSize bounds;
	if (! [prefs integerForKey:@"initialized"]) {
		[prefs setObject:@"1" forKey:@"initialized"];
		[prefs setObject:@"-1" forKey:@"precision"];
		[prefs setObject:@"NO" forKey:@"engineeringNotation"];
		[prefs setObject:@"NO" forKey:@"historyDuplicatesAllowed"];
		[prefs setObject:@"NO" forKey:@"flagUndefinedVariables"];
		[prefs setObject:@"YES" forKey:@"useRadians"];
		[prefs setObject:@"0" forKey:@"outputFormat"];
		[prefs setObject:@"NO" forKey:@"printPrefixes"];
		[prefs setObject:@"NO" forKey:@"updateHistory"];
		[prefs setObject:@"NO" forKey:@"useCommas"];
		[prefs setObject:@"YES" forKey:@"strictSyntax"];
	}
	precision = [prefs integerForKey:@"precision"];
	engineering = [prefs boolForKey:@"engineeringNotation"];
	allow_duplicates = [prefs boolForKey:@"historyDuplicatesAllowed"];
	picky_variables = [prefs boolForKey:@"flagUndefinedVariables"];
	use_radians = [prefs boolForKey:@"useRadians"];
	output_format = [prefs integerForKey:@"outputFormat"];
	[PrecisionSlider setEnabled:(output_format==DECIMAL_FORMAT)];
	print_prefixes = [prefs boolForKey:@"printPrefixes"];
	update_history = [prefs boolForKey:@"updateHistory"];
	use_commas = [prefs boolForKey:@"useCommas"];
	strict_syntax = [prefs boolForKey:@"strictSyntax"];
	
	[PrecisionSlider setIntValue:precision];
	just_answered = FALSE;

	superview = [keypad superview];
	[keypad retain];
	[PrecisionSlider retain];
	[ExpressionField retain];
	[mainWindow useOptimizedDrawing:TRUE];
	[mainWindow setFrameAutosaveName:@"wcalc"];
	w = [mainWindow frame];
	if ([prefs boolForKey:@"toggled"]) {
		w.size.height += KEYPAD_HEIGHT;
		w.origin.y -= KEYPAD_HEIGHT;
		[mainWindow setFrame:w display:FALSE animate:FALSE];
		[self toggleSize:0];
	} else {
		w.size.width = MIN_WINDOW_WIDTH;
		[mainWindow setFrame:w display:FALSE animate:FALSE];
		bounds.width = MIN_WINDOW_WIDTH;
		bounds.height = MIN_WINDOW_HEIGHT_UNTOGGLED;
		[mainWindow setMinSize:bounds];
		bounds.width = MIN_WINDOW_WIDTH;
		bounds.height = 1200;
		[mainWindow setMaxSize:bounds];
	}
	w = [mainWindow frame];
	bounds = [mainWindow minSize];
}

- (IBAction)setPrecision:(id)sender
{
	int last_pres=0;
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];

	last_pres = [prefs integerForKey:@"precision"];
	
	if (last_pres == [PrecisionSlider intValue])
		return;
	else
		last_pres = [PrecisionSlider intValue];

	precision = last_pres;
//	printf("precision = %i\n",precision);
	[prefs setObject:[NSString stringWithFormat:@"%i",precision] forKey:@"precision"];

	{
		char * temp;
		if (pretty_answer) free(pretty_answer);
		temp = print_this_result(last_answer);
		if (temp) pretty_answer = strdup(temp);
		else pretty_answer = NULL;
	}

	[AnswerField setStringValue:[NSString stringWithCString:(pretty_answer?pretty_answer:"Not Enough Memory")]];

	[ExpressionField selectText:self];
}

- (IBAction)go:(id)sender
{
	char * expression;
	double val;
	extern char * errstring;

	expression = strdup([[ExpressionField stringValue] cString]);

	val = parseme(expression);
	addToHistory(expression, val);
	free(expression);
	putvar("a",val);

	if (errstring && strlen(errstring)) {
		[errorController throwAlert:[NSString stringWithCString:errstring]];
		free(errstring);
		errstring = NULL;
	}
	[AnswerField setStringValue:[NSString stringWithFormat:@"%s",pretty_answer]];
	// if the drawer is open, refresh the data.
	// make sure the menu is correct for the state of the drawer
	if (! [theDrawer state]) {
		if (! [[affectDrawerMenu title] isEqualToString:@"Show Inspector Drawer"])
			[affectDrawerMenu setTitle:@"Show Inspector Drawer"];
	} else {
		[variableList reloadData];
		[historyList reloadData];
		if (! [[affectDrawerMenu title] isEqualToString:@"Hide Inspector Drawer"])
			[affectDrawerMenu setTitle:@"Hide Inspector Drawer"];
	}	
	just_answered = TRUE;
	if ([thePrefPanel isVisible])
		[prefsController displayPrefs:sender];
	[ExpressionField selectText:sender];
}

- (IBAction)enterData:(id)sender
{
	NSString * sent = [[sender attributedTitle] string];
	char * str = strdup([[ExpressionField stringValue] cString]);
	static short shiftdown = 0, capsdown = 0;

	[ExpressionField setSelectable:FALSE];
	if ([sent isEqualToString:@"delete"]) {
		if (strlen(str)) {
			str[strlen(str)-1] = 0;
			[ExpressionField setStringValue:[NSString stringWithCString:str]];
		}
	} else if ([sent isEqualToString:@"caps"]) {
		if (capsdown) {
			[shiftKey1 setEnabled:true];
			[shiftKey2 setEnabled:true];
			capsdown = 0;
		} else {
			[shiftKey1 setEnabled:false];
			[shiftKey2 setEnabled:false];
			capsdown = 1;
		}
	} else if ([sent isEqualToString:@"shift"]) {
		if (shiftdown) {
			[shiftKey1 setState:NSOffState];
			[shiftKey2 setState:NSOffState];
			shiftdown = 0;
		} else {
			[shiftKey1 setState:NSOnState];
			[shiftKey2 setState:NSOnState];
			shiftdown = 1;
		}
	} else {
		if (just_answered == FALSE) {
			[ExpressionField setStringValue:[[ExpressionField stringValue] stringByAppendingString:sent]];
		} else if ([sent isEqualToString:@"+"] || [sent isEqualToString:@"-"]||[sent isEqualToString:@"*"]||[sent isEqualToString:@"/"]||[sent isEqualToString:@"%"]||[sent isEqualToString:@"+"]||[sent isEqualToString:@"("]||[sent isEqualToString:@"&"]||[sent isEqualToString:@"|"]||[sent isEqualToString:@"Ö"]) {
			[ExpressionField setStringValue:[[@"a" self] stringByAppendingString:sent]];
		} else {
			[ExpressionField setStringValue:sent];
		}
		if (shiftdown) {
			[shiftKey1 setState:NSOffState];
			[shiftKey2 setState:NSOffState];
			shiftdown = 0;
		}
		just_answered = FALSE;
	}
	[ExpressionField setEditable:TRUE];
	free(str);
}

- (IBAction)showInspectorDrawer:(id)sender
{
	if (! [theDrawer state]) {
		[theDrawer open];
		[affectDrawerMenu setTitle:@"Show Inspector Drawer"];
	} else {
		[theDrawer close];
		[affectDrawerMenu setTitle:@"Hide Inspector Drawer"];
	}
}

- (IBAction)showKeyboardDrawer:(id)sender
{
	static char initialized = 0;
	if (! [theKeyboard isVisible]) {
		[theKeyboard makeKeyAndOrderFront:self];
		if (! initialized) {
			[theKeyboard center];
		}
		[theKeyboard setFrameAutosaveName:@"wcalc_keyboard"];
	} else {
		[theKeyboard close];
	}
}

- (IBAction)setPrefs:(id)sender
{
	short need_redraw = 0;
	short olde;
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];

	switch ([sender tag]) {
		case 1: // Flag Undefined Variables
			olde = picky_variables;
			picky_variables = ([pickyVariables state]==NSOnState);
			if (olde != picky_variables) {
				[prefs setObject:(picky_variables?@"YES":@"NO") forKey:@"flagUndefinedVariables"];
			}
				break;
		case 2: // Use Radians
			olde = use_radians;
			use_radians = ([useRadians state]==NSOnState);
			if (olde != use_radians) {
				need_redraw = 2;
				[prefs setObject:(use_radians?@"YES":@"NO") forKey:@"useRadians"];
			}
				break;
		case 3: // Use Engineering Notation
			olde = engineering;
			engineering = ([engineeringNotation state]==NSOnState);
			if (olde != engineering) {
				need_redraw = 1;
				[prefs setObject:(engineering?@"YES":@"NO") forKey:@"engineeringNotation"];
			}
				break;
		case 4: // Allow Duplicates in History
			olde = allow_duplicates;
			allow_duplicates = ([historyDuplicates state]==NSOnState);
			if (olde != allow_duplicates) {
				[prefs setObject:(allow_duplicates?@"YES":@"NO") forKey:@"historyDuplicatesAllowed"];
			}
				break;
		case 5: // Output Format
			olde = output_format;
			output_format = [[sender selectedCell] tag];
			if (olde != output_format) {
				need_redraw = 1;
				[prefs setObject:[NSString stringWithFormat:@"%i",output_format] forKey:@"outputFormat"];
				[PrecisionSlider setEnabled:(output_format==DECIMAL_FORMAT)];
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
				break;
		case 7: // Update History
			olde = update_history;
			update_history = ([sender state]==NSOnState);
			if (olde != update_history) {
				[prefs setObject:(update_history?@"YES":@"NO") forKey:@"updateHistory"];
			}
				break;
		case 8: // Use Commas
			olde = use_commas;
			use_commas = ([sender state]==NSOnState);
			if (olde != use_commas) {
				need_redraw = 1;
				[prefs setObject:(use_commas?@"YES":@"NO") forKey:@"useCommas"];
				[decimalKey setTitle:(use_commas?@",":@".")];
			}
				break;
		case 9: // Flag Confusing Numbers
			olde = strict_syntax;
			strict_syntax = ([sender state]==NSOnState);
			if (olde != strict_syntax) {
				[prefs setObject:(strict_syntax?@"YES":@"NO") forKey:@"strictSyntax"];
			}
		default: return;
	}

	switch (need_redraw) {
		case 2:
			if (update_history)
				recalculate = 1;
			[self go:sender];
		case 1:
		{
			char *temp;
			if (pretty_answer) free(pretty_answer);

			temp = print_this_result(last_answer);
			if (temp)
				pretty_answer = strdup(temp);
			else
				pretty_answer = NULL;

			[AnswerField setStringValue:[NSString stringWithCString:(pretty_answer?pretty_answer:"Not Enough Memory")]];

			if ([theDrawer state] || recalculate) {
				[historyList reloadData];
			}
			
			[ExpressionField selectText:self];
			break;
		}
//		case 2:
//			[self go:sender];
//			break;
    }
}

- (IBAction)showPrefs:(id)sender
{
	static char initialized = 0;
	[self displayPrefs:sender];
	[thePrefPanel makeKeyAndOrderFront:self];
	// centering may not be necessary, but...
	if (! initialized) {
		initialized=1;
		[thePrefPanel center];
	}
	[thePrefPanel setFrameAutosaveName:@"wcalc_prefs"];
}

- (IBAction)displayPrefs:(id)sender
{
	[engineeringNotation setState:(engineering?NSOnState:NSOffState)];
    [pickyVariables setState:(picky_variables?NSOnState:NSOffState)];
    [historyDuplicates setState:(allow_duplicates?NSOnState:NSOffState)];
	[useRadians setState:(use_radians?NSOnState:NSOffState)];
	[outputFormat selectCellWithTag:output_format];
	[printPrefixes setState:(print_prefixes?NSOnState:NSOffState)];
	[useCommas setState:(use_commas?NSOnState:NSOffState)];
	[strictSyntax setState:(strict_syntax?NSOnState:NSOffState)];
	[printPrefixes setEnabled:(output_format!=DECIMAL_FORMAT)];
	[engineeringNotation setEnabled:(output_format==DECIMAL_FORMAT)];
}

@end
