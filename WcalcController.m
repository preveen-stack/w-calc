#import "calculator.h"
#import "variables.h"
#import "ErrorController.h"
#import "historyManager.h"
#import "WcalcController.h"
//#import <Foundation/NSRange.h>

#define KEYPAD_HEIGHT 165

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
	} else {
		f.size.height += KEYPAD_HEIGHT;
		f.origin.y -= KEYPAD_HEIGHT;
	}
	
	f.size.width = 171;
	e.size.width = 149;
	p.size.width = 149;
	
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

	[mainWindow setFrame:f display:TRUE animate:TRUE];

	if (! shrinking) {
		[superview addSubview:keypad];
		w.width = 171;
		w.height = 283;
		[mainWindow setMinSize:w];
		w.width = 171;
		w.height = 1200;
		[mainWindow setMaxSize:w];
		[prefs setObject:@"NO" forKey:@"toggled"];
	} else {
		w.width = 171;
		w.height = 118;
		[mainWindow setMinSize:w];
		w.width = 1600;
		w.height = 1200;
		[mainWindow setMaxSize:w];
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
		default: return;
	}
	[ExpressionField setStringValue:[str2 stringByAppendingString:str]];
}

- (void)awakeFromNib
{
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
	NSRect w;
	if (! [prefs integerForKey:@"initialized"]) {
		[prefs setObject:@"1" forKey:@"initialized"];
		[prefs setObject:@"-1" forKey:@"precision"];
		[prefs setObject:@"NO" forKey:@"engineeringNotation"];
		[prefs setObject:@"NO" forKey:@"historyDuplicatesAllowed"];
		[prefs setObject:@"NO" forKey:@"flagUndefinedVariables"];
		[prefs setObject:@"YES" forKey:@"useRadians"];
		[prefs setObject:@"0" forKey:@"outputFormat"];
		[prefs setObject:@"NO" forKey:@"printPrefixes"];
	}
	precision = [prefs integerForKey:@"precision"];
	engineering = [prefs boolForKey:@"engineeringNotation"];
	allow_duplicates = [prefs boolForKey:@"historyDuplicatesAllowed"];
	picky_variables = [prefs boolForKey:@"flagUndefinedVariables"];
	use_radians = [prefs boolForKey:@"useRadians"];
	output_format = [prefs integerForKey:@"outputFormat"];
	[PrecisionSlider setEnabled:(output_format==DECIMAL_FORMAT)];
	print_prefixes = [prefs boolForKey:@"printPrefixes"];
	
	[PrecisionSlider setIntValue:precision];
	[mainWindow setFrameAutosaveName:@"wcalc"];
	just_answered = FALSE;

	superview = [keypad superview];
	[keypad retain];
	[PrecisionSlider retain];
	[ExpressionField retain];
	[mainWindow useOptimizedDrawing:TRUE];
	if ([prefs boolForKey:@"toggled"]) {
		[self toggleSize:0];
	}
	[mainWindow setFrameUsingName:@"wcalc"];
	w = [mainWindow frame];
	if (! [prefs boolForKey:@"toggled"])
		w.size.width = 171;
	[mainWindow setFrame:w display:TRUE animate:FALSE];
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

	if (pretty_answer) free(pretty_answer);
	pretty_answer = strdup(print_this_result(last_answer));

	[AnswerField setStringValue:[NSString stringWithCString:pretty_answer]];

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
	} else {
		[AnswerField setStringValue:[NSString stringWithFormat:@"%s",pretty_answer]];
	}
	[variableList reloadData];
	[historyList reloadData];
	[ExpressionField selectText:sender];
	just_answered = TRUE;
}

- (IBAction)enterData:(id)sender
{
	NSString * sent = [[sender attributedTitle] string];
	char * str = strdup([[ExpressionField stringValue] cString]);
	static short shiftdown = 0, capsdown = 0;

	[ExpressionField setSelectable:false];
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
		} else if ([sent isEqualToString:@"+"] || [sent isEqualToString:@"-"]||[sent isEqualToString:@"*"]||[sent isEqualToString:@"/"]||[sent isEqualToString:@"%"]||[sent isEqualToString:@"+"]||[sent isEqualToString:@"("]||[sent isEqualToString:@"&"]||[sent isEqualToString:@"|"]) {
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
	[ExpressionField setSelectable:true];
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
	if (! [theKeyboard isVisible]) {
		[theKeyboard makeKeyAndOrderFront:self];
	} else {
		[theKeyboard close];
	}
}

@end
