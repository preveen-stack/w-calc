#import "calculator.h"
#import "variables.h"
#import "ErrorController.h"
#import "historyManager.h"
#import "WcalcController.h"


@implementation WcalcController

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

- (void)awakeFromNib
{
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
	if (! [prefs integerForKey:@"initialized"]) {
		[prefs setObject:@"1" forKey:@"initialized"];
		[prefs setObject:@"-1" forKey:@"precision"];
		[prefs setObject:@"NO" forKey:@"engineeringNotation"];
		[prefs setObject:@"NO" forKey:@"historyDuplicatesAllowed"];
		[prefs setObject:@"NO" forKey:@"flagUndefinedVariables"];
		[prefs setObject:@"YES" forKey:@"useRadians"];
	}
	precision = [prefs integerForKey:@"precision"];
	engineering = [prefs boolForKey:@"engineeringNotation"];
	allow_duplicates = [prefs boolForKey:@"historyDuplicatesAllowed"];
	picky_variables = [prefs boolForKey:@"flagUndefinedVariables"];
	use_radians = [prefs boolForKey:@"useRadians"];
	
	[PrecisionSlider setIntValue:precision];
	[mainWindow setFrameAutosaveName:@"wcalc"];
}

- (IBAction)setPrecision:(id)sender
{
	static int last_pres=0;
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];

	if (last_pres == [PrecisionSlider intValue])
		return;
	else
		last_pres = [PrecisionSlider intValue];

	precision = last_pres;
	[prefs setObject:[NSString stringWithFormat:@"%i",precision] forKey:@"precision"];

	print_this_result(last_answer);

	[AnswerField setStringValue:[NSString stringWithCString:pretty_answer]];

	[ExpressionField selectText:self];
}

- (IBAction)go:(id)sender
{
	char * expression;
	double val;
	extern char * errstring;

	expression = strdup([[ExpressionField stringValue] cString]);

	addToHistory(expression);
	val = parseme(expression);
	free(expression);
	putvar("a",val);

	if (errstring && strlen(errstring)) {
		[errorController throwAlert:[NSString stringWithCString:errstring]];
		free(errstring);
		errstring = NULL;
	} else {
		[AnswerField setStringValue:[NSString stringWithCString:pretty_answer]];
	}
	[variableList reloadData];
	[historyList reloadData];
	[ExpressionField selectText:self];
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
		[ExpressionField setStringValue:[[ExpressionField stringValue] stringByAppendingString:sent]];
		if (shiftdown) {
			[shiftKey1 setState:NSOffState];
			[shiftKey2 setState:NSOffState];
			shiftdown = 0;
		}
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
