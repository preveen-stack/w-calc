#include "calculator.h"
#include "variables.h"
#include "conversion.h"
#include "ErrorController.h"
#include "historyManager.h"
#include "WcalcController.h"
#include "string_manip.h"
#include "files.h"
#include "MyTextField.h"
#include "simpleCalc.h"

#define KEYPAD_HEIGHT 165
#define MIN_WINDOW_WIDTH 171
#define MIN_WINDOW_HEIGHT_TOGGLED 112
#define MIN_WINDOW_HEIGHT_UNTOGGLED 277
#define FIELD_WIDTH_DIFFERENCE 22
#define MAX_WINDOW_SIZE 10000

static char update_history = 0;
NSButton *e;
NSTextField *ef;
static NSString *curFile = NULL;

@implementation WcalcController

- (IBAction)toggleSize:(id)sender
{
	static BOOL shrinking = TRUE;
	NSRect mainwindow = [mainWindow frame];
	NSRect exp = [ExpressionField frame];
	NSRect prec = [PrecisionSlider frame];
	NSRect ans = [AnswerField frame];
	NSSize w;
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];

	if (shrinking) {
		// if removing the keypad, change the window height
		mainwindow.size.height -= KEYPAD_HEIGHT;
		mainwindow.origin.y += KEYPAD_HEIGHT;
		// this is set in case the calc starts up toggled (under other
		// conditions it is not strictly necessary, because the ExpressionField
		// has achieved the correct size automatically
		exp.size.height = mainwindow.size.height
			- ans.size.height
			- prec.size.height
			- 57 /* the size of the rest of the window, including title bar */;
	} else {
		// if adding the keypad, change the window height
		mainwindow.size.height += KEYPAD_HEIGHT;
		mainwindow.origin.y -= KEYPAD_HEIGHT;
		mainwindow.size.width = MIN_WINDOW_WIDTH;
	}

	exp.size.width = prec.size.width = mainwindow.size.width - FIELD_WIDTH_DIFFERENCE;

	if (shrinking) {
		[keypad removeFromSuperview];
		exp.origin.y -= KEYPAD_HEIGHT;
		prec.origin.y -= KEYPAD_HEIGHT;
	} else {
		exp.origin.y += KEYPAD_HEIGHT;
		prec.origin.y += KEYPAD_HEIGHT;
	}

	[ExpressionField removeFromSuperview];
	[PrecisionSlider removeFromSuperview];

	if (sender != 0) {
		[mainWindow setFrame:mainwindow display:TRUE animate:TRUE];
	} else {
		[mainWindow setFrame:mainwindow display:FALSE animate:FALSE];
	}

	if (! shrinking) {
		[superview addSubview:keypad];
		w.width = MIN_WINDOW_WIDTH;
		w.height = MIN_WINDOW_HEIGHT_UNTOGGLED + (ans.size.height - 21);
		[mainWindow setMinSize:w];
		w.width = MIN_WINDOW_WIDTH;
		w.height = MAX_WINDOW_SIZE;
		[mainWindow setMaxSize:w];
		[prefs setObject:@"NO" forKey:@"toggled"];
	} else {
		w.width = MIN_WINDOW_WIDTH;
		w.height = MIN_WINDOW_HEIGHT_TOGGLED + (ans.size.height - 21);
		[mainWindow setMinSize:w];
		w.width = MAX_WINDOW_SIZE;
		w.height = MAX_WINDOW_SIZE;
		[mainWindow setMaxSize:w];
		[prefs setObject:@"YES" forKey:@"toggled"];
	}

	[ExpressionField setFrame:exp];
	[PrecisionSlider setFrame:prec];

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
		case 3: str2 = @"gamma"; break;
		case 4: str2 = @"g"; break;
		case 5: str2 = @"Cc"; break;

		case 101: str2 = @"Z0"; break;
		case 102: str2 = @"epsilon0"; break;
		case 103: str2 = @"µ0"; break;
		case 104: str2 = @"G"; break;
		case 105: str2 = @"h"; break;
		case 106: str2 = @"c"; break;

		case 201: str2 = @"µB"; break;
		case 202: str2 = @"µN"; break;
		case 203: str2 = @"G0"; break;
		case 204: str2 = @"ec"; break;
		case 205: str2 = @"Kj"; break;
		case 206: str2 = @"Rk"; break;

		case 301: str2 = @"Mampha"; break;
		case 302: str2 = @"ao"; break;
		case 303: str2 = @"Md"; break;
		case 304: str2 = @"Me"; break;
		case 305: str2 = @"re"; break;
		case 306: str2 = @"eV"; break;
		case 307: str2 = @"Gf"; break;
		case 308: str2 = @"alpha"; break;
		case 309: str2 = @"Eh"; break;
		case 310: str2 = @"Mh"; break;
		case 311: str2 = @"Mµ"; break;
		case 312: str2 = @"Mn"; break;
		case 313: str2 = @"Mp"; break;
		case 314: str2 = @"Rinf"; break;
		case 315: str2 = @"Mt"; break;

		case 401: str2 = @"u"; break;
		case 402: str2 = @"Na"; break;
		case 403: str2 = @"k"; break;
		case 404: str2 = @"F"; break;
		case 405: str2 = @"c1"; break;
		case 406: str2 = @"n0"; break;
		case 407: str2 = @"R"; break;
		case 408: str2 = @"Vm"; break;
		case 409: str2 = @"c2"; break;
		case 410: str2 = @"sigma"; break;
		case 411: str2 = @"b"; break;

		case 6: str2 = @"random"; break;
		case 7: str2 = @"irandom"; break;
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
		case 20: str2 = @"cbrt("; break;
		case 21: str2 = @"logtwo("; break;
		case 22: str2 = @"cot("; break;
		case 23: str2 = @"acot("; break;
		case 24: str2 = @"coth("; break;
		case 25: str2 = @"acoth("; break;
                case 26: str2 = @"rand("; break;
                case 27: str2 = @"irand("; break;
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
		[prefs setObject:@"YES" forKey:@"strictSyntax"];
		[prefs setObject:@"0" forKey:@"roundingIndication"];
		[prefs setObject:@"NO" forKey:@"historyShowing"];
		[prefs setObject:@"YES" forKey:@"rememberErrors"];
		[prefs setObject:@"NO" forKey:@"baseShowing"];
		[prefs setObject:@"YES" forKey:@"precisionGuard"];
		[prefs setObject:@"NO" forKey:@"historyLimit"];
		[prefs setObject:@"1000" forKey:@"historyLimitLength"];
		[prefs setObject:@"NO" forKey:@"printInts"];
		[prefs setObject:@"NO" forKey:@"simpleCalc"];
	}
	conf.precision = [prefs integerForKey:@"precision"];
	conf.engineering = [prefs boolForKey:@"engineeringNotation"];
	conf.picky_variables = [prefs boolForKey:@"flagUndefinedVariables"];
	conf.use_radians = [prefs boolForKey:@"useRadians"];
	conf.output_format = [prefs integerForKey:@"outputFormat"];
	[PrecisionSlider setEnabled:(conf.output_format==DECIMAL_FORMAT)];
	conf.print_prefixes = [prefs boolForKey:@"printPrefixes"];
/*	conf.strict_syntax = [prefs boolForKey:@"strictSyntax"]; */
	conf.rounding_indication = [prefs integerForKey:@"roundingIndication"];
	conf.precision_guard = [prefs boolForKey:@"precisionGuard"];
	conf.print_ints = [prefs boolForKey:@"printInts"];
	conf.simple_calc = [prefs boolForKey:@"simpleCalc"];
	/* history preferences */
	allow_duplicates = [prefs boolForKey:@"historyDuplicatesAllowed"];
	update_history = [prefs boolForKey:@"updateHistory"];
	conf.remember_errors = [prefs boolForKey:@"rememberErrors"];
	conf.history_limit = [prefs boolForKey:@"historyLimit"];
	conf.history_limit_len = [prefs integerForKey:@"historyLimitLen"];

	[PrecisionSlider setIntValue:conf.precision];
	just_answered = FALSE;

	/* Set up the character translation */
	conf.dec_delimiter = [[prefs objectForKey:NSDecimalSeparator] characterAtIndex:0];
	conf.thou_delimiter = [[prefs objectForKey:NSThousandsSeparator] characterAtIndex:0];
	[decimalKey setAttributedTitle:[prefs objectForKey:NSDecimalSeparator]];

	/* reset the window to the saved setting */
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
		[mainWindow setFrame:w display:TRUE animate:FALSE];
		[self toggleSize:0];
	} else {
		w.size.width = MIN_WINDOW_WIDTH;
		[mainWindow setFrame:w display:TRUE animate:FALSE];
		bounds.width = MIN_WINDOW_WIDTH;
		bounds.height = MIN_WINDOW_HEIGHT_UNTOGGLED;
		[mainWindow setMinSize:bounds];
		bounds.width = MIN_WINDOW_WIDTH;
		bounds.height = MAX_WINDOW_SIZE;
		[mainWindow setMaxSize:bounds];
	}
//	w = [mainWindow frame];
//	bounds = [mainWindow minSize];

	e = enterKey;
	ef = ExpressionField;

	/* this restores the drawer states */
	if ([prefs boolForKey:@"historyShowing"]) {
		[NSTimer scheduledTimerWithTimeInterval:0 target:self selector:@selector(openIDrawer:) userInfo:nil repeats:NO];
	}
	if ([prefs boolForKey:@"baseShowing"]) {
		[NSTimer scheduledTimerWithTimeInterval:0 target:self selector:@selector(openBDrawer:) userInfo:nil repeats:NO];
	}

	/* set the correct expression display for simple_calc */
	if (conf.simple_calc) {
		[ExpressionField setStringValue:@"0"];
		[AnswerField setStringValue:@"0"];
		simpleClearAll();
	}
	mpfr_set_default_prec(1024);
	mpfr_init_set_ui(last_answer, 0, GMP_RNDN);
}

- (void)openBDrawer: (id) sender
{
	[baseDrawer open];
}

- (void)openIDrawer: (id) sender
{
	[theDrawer open];
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

	conf.precision = last_pres;
	[prefs setObject:[NSString stringWithFormat:@"%i",conf.precision] forKey:@"precision"];

	set_prettyanswer(last_answer);

	[self displayAnswer];
}

- (IBAction)go:(id)sender
{
	char * expression;
	extern char * errstring;

	expression = strdup([[ExpressionField stringValue] cString]);

	parseme(expression);
	putval("a",last_answer);

	/* if it isn't an error (or if you want me to remember errors) record it in the history */
	if (!errstring || (errstring && !strlen(errstring)) || conf.remember_errors) {
		addToHistory(expression, last_answer);
		free(expression);
	}

	[self displayAnswer];
}

- (void)displayAnswer
{
	extern char * errstring;

	/* if there is an error, display it */
	if (errstring && strlen(errstring)) {
		extern int scanerror;
		scanerror = 0;
		[errorController throwAlert:[NSString stringWithCString:errstring]];
		free(errstring);
		errstring = NULL;
	}
	/* display the answer */
	[AnswerField setStringValue:[NSString stringWithFormat:@"%s",pretty_answer]];
	[AnswerField setTextColor:(not_all_displayed?[NSColor redColor]:[NSColor blackColor])];
	{ // Make the Answerfield big enough to display the answer
		NSRect curFrame, newFrame;
		curFrame = [AnswerField frame];
		newFrame = curFrame;
		//newFrame.size.height = 10000000.0; // arbitrarily big number
		newFrame.size = [[AnswerField cell] cellSizeForBounds:newFrame];
		if (curFrame.size.height != newFrame.size.height) {
			size_t newHeight;
			int difference;
			NSRect windowFrame;
			NSSize ms;
			newHeight = newFrame.size.height;
			newFrame = curFrame;
			newFrame.size.height = newHeight;
			difference = newHeight - curFrame.size.height;
			windowFrame = [mainWindow frame];
			windowFrame.size.height += difference;
			windowFrame.origin.y -= difference;
			curFrame = [ExpressionField frame];
			[AnswerField setHidden:TRUE];
			[ExpressionField setHidden:TRUE];
			[mainWindow setFrame:windowFrame display:YES animate:YES];
			ms = [mainWindow minSize];
			ms.height += difference;
			[mainWindow setMinSize:ms];
			[AnswerField setFrame:newFrame];
			[ExpressionField setFrame:curFrame];
			[AnswerField setHidden:FALSE];
			[ExpressionField setHidden:FALSE];
		}
	}

	// if the drawer is open, refresh the data.
	if ([theDrawer state]) {
		[variableList reloadData];
		[historyList reloadData];
	}
	just_answered = TRUE;
	// refresh the prefs if necessary
	if ([thePrefPanel isVisible])
		[self displayPrefs:0];
	[outputFormat2 selectCellWithTag:conf.output_format];
	[ExpressionField selectText:self];
}

- (IBAction)convert:(id)sender
{
	int type = [convertType indexOfSelectedItem];
	int from = [convertFrom selectedRow];
	int to = [convertTo selectedRow];

	if (type < 0 || type > MAX_TYPE) return;
	if (from < 0) return;
	if (to < 0) return;

	uber_conversion(last_answer, type, from, to, last_answer);
	set_prettyanswer(last_answer);
	[AnswerField setStringValue:[NSString stringWithCString:(pretty_answer?pretty_answer:"Not Enough Memory")]];
	putval("a",last_answer);
	if ([theDrawer state]) {
		[variableList reloadData];
	}
}

- (IBAction)enterData:(id)sender
{
	NSString * sent = [[sender attributedTitle] string];
	char * str = strdup([[ExpressionField stringValue] cString]);
	static short shiftdown = 0, capsdown = 0;
	int tag;

	[ExpressionField setSelectable:FALSE];
	tag = [sender tag];
	switch (tag) {
		case 101: /* delete key on the onscreen keyboard */
			if (strlen(str)) {
				str[strlen(str)-1] = 0;
				[ExpressionField setStringValue:[NSString stringWithCString:str]];
			}
			break;
		case 100: /* clear key on the onscreen keypad */
			if (!conf.simple_calc) {
				if ([[ExpressionField stringValue] cStringLength]) {
					[ExpressionField setStringValue:@""];
					[ExpressionField selectText:self];
				} else if ([[AnswerField stringValue] cStringLength]) {
					[AnswerField setStringValue:@""];
					[ExpressionField selectText:self];
				}
			} else {
				if (! [[ExpressionField stringValue] isEqualToString:@"0"]) {
					[ExpressionField setStringValue:@"0"];
					[ExpressionField selectText:self];
					simpleClearEntry();
				} else if (! [[AnswerField stringValue] isEqualToString:@"0"]) {
					[AnswerField setStringValue:@"0"];
					[ExpressionField selectText:self];
					simpleClearAll();
				}
			}
			break;
		case 102: /* caps lock key on onscreen keyboard */
			if (capsdown) {
				[shiftKey1 setEnabled:true];
				[shiftKey2 setEnabled:true];
				capsdown = 0;
			} else {
				[shiftKey1 setEnabled:false];
				[shiftKey2 setEnabled:false];
				capsdown = 1;
			}
			break;
		case 103: /* shift key on onscreen keyboard */
			if (shiftdown) {
				[shiftKey1 setState:NSOffState];
				[shiftKey2 setState:NSOffState];
				shiftdown = 0;
			} else {
				[shiftKey1 setState:NSOnState];
				[shiftKey2 setState:NSOnState];
				shiftdown = 1;
			}
			break;
		case 104: /* = key on onscreen keypad */
			if (! conf.simple_calc) {
				[self go:sender];
			} else {
				char * exp = strdup([[ExpressionField stringValue] cString]);
				char * ret;
				ret = simpleCalc('=',exp);
				if (ret) {
					[ExpressionField setStringValue:[NSString stringWithCString:ret]];
					free(ret);
				} else {
					[self displayAnswer];
					[ExpressionField setStringValue:[AnswerField stringValue]];
				}
				free(exp);
			}
			break;
		case 105: /* the divide key on the onscreen keypad */
		default:
			if (! conf.simple_calc) { /* the real power of Wcalc */
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
			} else { /* stupid calculator */
				char *ret, *exp;
				exp = strdup([[ExpressionField stringValue] cString]);
				if (tag == 105) {
					ret = simpleCalc('/',exp);
				} else {
					ret = simpleCalc([sent characterAtIndex:0],exp);
				}
				free(exp);
				if (ret) {
					[ExpressionField setStringValue:[NSString stringWithCString:ret]];
					free(ret);
				} else {
					[self displayAnswer];
					[ExpressionField setStringValue:[AnswerField stringValue]];
				}
			}
			break;
	}
	[ExpressionField setEditable:TRUE];
	free(str);
}

- (IBAction)shConversions:(id)sender
{
	static char initialized = 0;
	if (! [conversionWindow isVisible]) {
		[conversionWindow makeKeyAndOrderFront:self];
		if (! initialized) {
			[conversionWindow center];
			initialized = 1;
		}
	} else {
		[conversionWindow close];
	}
}

- (IBAction)showInspectorDrawer:(id)sender
{
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
	if (! [theDrawer state]) {
		[theDrawer open];
		[prefs setObject:@"YES" forKey:@"historyShowing"];
		[affectDrawerMenu setTitle:@"Hide Inspector Drawer"];
	} else {
		[theDrawer close];
		[prefs setObject:@"NO" forKey:@"historyShowing"];
		[affectDrawerMenu setTitle:@"Show Inspector Drawer"];
	}
}

- (IBAction)showKeyboardDrawer:(id)sender
{
	static char initialized = 0;
	if (! [theKeyboard isVisible]) {
		[theKeyboard makeKeyAndOrderFront:self];
		if (! initialized) {
			[theKeyboard center];
			initialized = 1;
		}
		[theKeyboard setFrameAutosaveName:@"wcalc_keyboard"];
	} else {
		[theKeyboard close];
	}
}

- (IBAction)showBaseDrawer:(id)sender
{
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
	if (! [baseDrawer state]) {
		[baseDrawer open];
		[prefs setObject:@"YES" forKey:@"baseShowing"];
		[baseMenu setTitle:@"Hide Base Drawer"];
	} else {
		[baseDrawer close];
		[prefs setObject:@"NO" forKey:@"baseShowing"];
		[baseMenu setTitle:@"Open Base Drawer"];
	}
}

- (IBAction)setPrefs:(id)sender
{
	short need_redraw = 0;
	short olde;
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];

	switch ([sender tag]) {
		case 1: // Flag Undefined Variables
			olde = conf.picky_variables;
			conf.picky_variables = ([pickyVariables state]==NSOnState);
			if (olde != conf.picky_variables) {
				[prefs setObject:(conf.picky_variables?@"YES":@"NO") forKey:@"flagUndefinedVariables"];
			}
				break;
		case 2: // Use Radians
			olde = conf.use_radians;
			conf.use_radians = ([useRadians state]==NSOnState);
			if (olde != conf.use_radians) {
				need_redraw = 2;
				[prefs setObject:(conf.use_radians?@"YES":@"NO") forKey:@"useRadians"];
			}
				break;
		case 3: // Use Engineering Notation
			olde = conf.engineering;
			conf.engineering = ([engineeringNotation state]==NSOnState);
			if (olde != conf.engineering) {
				need_redraw = 1;
				[prefs setObject:(conf.engineering?@"YES":@"NO") forKey:@"engineeringNotation"];
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
			olde = conf.output_format;
			conf.output_format = [[sender selectedCell] tag];
			if (olde != conf.output_format) {
				need_redraw = 1;
				[prefs setObject:[NSString stringWithFormat:@"%i",conf.output_format] forKey:@"outputFormat"];
				[PrecisionSlider setEnabled:(conf.output_format==DECIMAL_FORMAT)];
				[printPrefixes setEnabled:(conf.output_format!=DECIMAL_FORMAT)];
				[engineeringNotation setEnabled:(conf.output_format==DECIMAL_FORMAT)];
			}
				break;
		case 6: // Print Prefixes
			olde = conf.print_prefixes;
			conf.print_prefixes = ([sender state]==NSOnState);
			if (olde != conf.print_prefixes) {
				need_redraw = 1;
				[prefs setObject:(conf.print_prefixes?@"YES":@"NO") forKey:@"printPrefixes"];
			}
				break;
		case 7: // Update History
			olde = update_history;
			update_history = ([sender state]==NSOnState);
			if (olde != update_history) {
				[prefs setObject:(update_history?@"YES":@"NO") forKey:@"updateHistory"];
			}
				break;
		case 10: // Rounding Indication
			olde = conf.rounding_indication;
			conf.rounding_indication = [sender indexOfSelectedItem];
			if (olde != conf.rounding_indication) {
				need_redraw = 1;
				[prefs setObject:[NSString stringWithFormat:@"%i",conf.rounding_indication] forKey:@"roundingIndication"];
			}
			break;
		case 11: // Record errors in history
			olde = conf.remember_errors;
			conf.remember_errors = ([sender state]==NSOnState);
			if (olde != conf.remember_errors) {
				[prefs setObject:(conf.remember_errors?@"YES":@"NO") forKey:@"rememberErrors"];
			}
			break;
		case 12: // Conservative precision
			olde = conf.precision_guard;
			conf.precision_guard = ([sender state]==NSOnState);
			if (olde != conf.precision_guard) {
				need_redraw = 2;
				[prefs setObject:(conf.precision_guard?@"YES":@"NO") forKey:@"precisionGuard"];
			}
			break;
		case 13: // History length limit toggle
		    olde = conf.history_limit;
		    conf.history_limit = ([sender state]==NSOnState);
		    [limitHistoryLen setEnabled:conf.history_limit];
		    [limitHistoryLenTag setEnabled:conf.history_limit];
		    if (olde != conf.history_limit) {
				[prefs setObject:(conf.history_limit?@"YES":@"NO") forKey:@"historyLimit"];
		    }
			break;
		case 14: // History length limit
		    olde = conf.history_limit_len;
		    conf.history_limit_len = [sender intValue];
		    if (olde != conf.history_limit_len) {
				[prefs setObject:[NSString stringWithFormat:@"%i",conf.history_limit_len] forKey:@"historyLimitLength"];
		    }
				break;
		case 15: // Print ints
			olde = conf.print_ints;
			conf.print_ints = ([sender state]==NSOnState);
			if (olde != conf.print_ints) {
				need_redraw = 1;
				[prefs setObject:(conf.print_ints?@"YES":@"NO") forKey:@"printInts"];
			}
				break;
		case 16: // Simple calculator
			olde = conf.simple_calc;
			conf.simple_calc = ([sender state]==NSOnState);
			if (olde != conf.simple_calc) {
				[prefs setObject:(conf.simple_calc?@"YES":@"NO") forKey:@"simpleCalc"];
				if (conf.simple_calc) {
					[ExpressionField setStringValue:@"0"];
					[AnswerField setStringValue:@"0"];
					simpleClearAll();
				} else {
					[ExpressionField setStringValue:@""];
					[AnswerField setStringValue:@""];
				}
				/* disable irrelevant preferences */
				[historyDuplicates setEnabled:!conf.simple_calc];
				[limitHistory setEnabled:!conf.simple_calc];
				[limitHistoryLen setEnabled:!conf.simple_calc];
				[limitHistoryLenTag setEnabled:!conf.simple_calc];
				[pickyVariables setEnabled:!conf.simple_calc];
				[rememberErrors setEnabled:!conf.simple_calc];
				[updateHistory setEnabled:!conf.simple_calc];
				[useRadians setEnabled:!conf.simple_calc];
			}
		default: return;
	}

	switch (need_redraw) {
		case 2:
			if (update_history)
				recalculate = 1;
			[self go:sender];
		case 1:
			set_prettyanswer(last_answer);

			[AnswerField setStringValue:[NSString stringWithCString:(pretty_answer?pretty_answer:"Not Enough Memory")]];
			[AnswerField setTextColor:((not_all_displayed)?([NSColor redColor]):([NSColor blackColor]))];

			if ([theDrawer state] || recalculate) {
				[historyList reloadData];
			}

			[ExpressionField selectText:self];
			break;
//		case 2:
//			[self go:sender];
//			break;
    }
}

- (IBAction)showPrefs:(id)sender
{
	static char initialized = 0;
	[self displayPrefs:sender];
	[thePrefPanel setBecomesKeyOnlyIfNeeded:TRUE];
	[thePrefPanel orderFront:self];
	// centering at first
	if (! initialized) {
		initialized=1;
		[thePrefPanel center];
	}
	[thePrefPanel setFrameAutosaveName:@"wcalc_prefs"];
}

- (IBAction)open:(id)sender
{
	int result;
	NSArray *fileTypes = [NSArray arrayWithObjects:@"txt",@"text",@"wcalc",
		NSFileTypeForHFSTypeCode('TEXT'), nil];
	NSOpenPanel *oPanel = [NSOpenPanel openPanel];
	[oPanel setAllowsMultipleSelection:NO];
	/* display the panel */
	result = [oPanel runModalForDirectory:nil file:nil types:fileTypes];
	/* if they clicked the OK button */
	if (result == NSOKButton) {
		NSArray *filesToOpen = [oPanel filenames];
		int i, count = [filesToOpen count];
		/* loop through the files to open (there should only be one, but
			it's good to be able to handle multiple anyway */
		for (i=0; i<count; i++) {
			int retval;
			extern char * errstring;
			curFile = [filesToOpen objectAtIndex:i];
			retval = loadState(strdup([curFile cString]));
			if ([theDrawer state]) {
				[variableList reloadData];
				[historyList reloadData];
			}
			/* if there is an error, display it */
			if (errstring && strlen(errstring)) {
				extern int scanerror;
				scanerror = 0;
				[errorController throwAlert:[NSString stringWithCString:errstring]];
				free(errstring);
				errstring = NULL;
			}
			// refresh the prefs if necessary
			if ([thePrefPanel isVisible])
				[self displayPrefs:sender];
			[outputFormat2 selectCellWithTag:conf.output_format];
			if (retval)
				[self displayErrno:retval forFile:curFile];
		}
	}
}

- (void)displayErrno:(int)err forFile:(NSString*)filename
{
	char * errstr;
	errstr = malloc(strlen(strerror(errno))+[filename cStringLength]+3);
	sprintf(errstr,"%s: %s",[filename cString], strerror(errno));
	[errorController throwAlert:[NSString stringWithCString:errstr]];
	free(errstr);
}

- (IBAction)saveAs:(id)sender
{
	NSSavePanel *sp;
	int runResult;
	/* create or get the shared instance of NSSavePanel */
	sp = [NSSavePanel savePanel];
	/* set up new attributes */
//	[sp setAccessoryView:newView];
	[sp setRequiredFileType:@"txt"];
	/* display the NSSavePanel */
	runResult = [sp runModalForDirectory:nil file:@""];
	/* if successful, save file under designated name */
	if (runResult == NSOKButton) {
		curFile = [sp filename];
		[self save:sender];
//		if (![textData writeToFile:[sp filename] atomically:YES])
//			NSBeep();
	}
}

- (BOOL)validateMenuItem:(NSMenuItem *)anItem
{
	switch ([anItem tag]) {
		case 1001: // save
		case 1002: // save as
			if (historyLength()) return YES;
			else return NO;
			break;
		default:
			return YES;
	}
}

- (IBAction)save:(id)sender
{
	if (! curFile) {
		[self saveAs:sender];
	} else {
		int retval;
		retval = saveState(strdup([curFile cString]));
		if (retval)
			[self displayErrno:retval forFile:curFile];
	}
}

- (IBAction)displayPrefs:(id)sender
{
	[engineeringNotation setState:(conf.engineering?NSOnState:NSOffState)];
    [pickyVariables setState:(conf.picky_variables?NSOnState:NSOffState)];
    [historyDuplicates setState:(allow_duplicates?NSOnState:NSOffState)];
	[useRadians setState:(conf.use_radians?NSOnState:NSOffState)];
	[outputFormat selectCellWithTag:conf.output_format];
	[printPrefixes setState:(conf.print_prefixes?NSOnState:NSOffState)];
	[roundingIndication selectItemAtIndex:conf.rounding_indication];
	[rememberErrors setState:(conf.remember_errors?NSOnState:NSOffState)];
	[limitHistory setState:(conf.history_limit?NSOnState:NSOffState)];
	[printInts setState:(conf.print_ints?NSOnState:NSOffState)];
	[precisionGuard setState:(conf.precision_guard?NSOnState:NSOffState)];
	[simpleCalculator setState:(conf.simple_calc?NSOnState:NSOffState)];

	/* disable irrelevant preferences */
	[historyDuplicates setEnabled:!conf.simple_calc];
	[limitHistory setEnabled:!conf.simple_calc];
	[pickyVariables setEnabled:!conf.simple_calc];
	[rememberErrors setEnabled:!conf.simple_calc];
	[updateHistory setEnabled:!conf.simple_calc];
	[useRadians setEnabled:!conf.simple_calc];
	[printPrefixes setEnabled:(conf.output_format!=DECIMAL_FORMAT)];
	[engineeringNotation setEnabled:(conf.output_format==DECIMAL_FORMAT)];
	[limitHistoryLen setEnabled:conf.history_limit&&!conf.simple_calc];
	[limitHistoryLenTag setEnabled:conf.history_limit&&!conf.simple_calc];

	{
	    char len[6];
	    sprintf(len,"%lu",conf.history_limit_len);
	    [limitHistoryLen setStringValue:[NSString stringWithFormat:@"%s",len]];
	}
}

@end
