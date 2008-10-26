#include <pthread.h>
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
#include "list.h"
#include "number.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif

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
static pthread_mutex_t displayLock;

@implementation WcalcController

- (IBAction)toggleSize:(id)sender
{
    static BOOL shrinking = TRUE;
    NSRect mainwindow = [mainWindow frame];
    NSRect expr = [ExpressionField frame];
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
	expr.size.height = mainwindow.size.height
	    - ans.size.height
	    - prec.size.height
	    - 57 /* the size of the rest of the window, including title bar */;
	[sizeToggleMenu setTitle:@"Show Keypad"];
    } else {
	// if adding the keypad, change the window height
	mainwindow.size.height += KEYPAD_HEIGHT;
	mainwindow.origin.y -= KEYPAD_HEIGHT;
	mainwindow.size.width = MIN_WINDOW_WIDTH;
	[sizeToggleMenu setTitle:@"Hide Keypad"];
    }

    expr.size.width = prec.size.width = mainwindow.size.width - FIELD_WIDTH_DIFFERENCE;

    if (shrinking) {
	[keypad removeFromSuperview];
	expr.origin.y -= KEYPAD_HEIGHT;
	prec.origin.y -= KEYPAD_HEIGHT;
    } else {
	expr.origin.y += KEYPAD_HEIGHT;
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

    [ExpressionField setFrame:expr];
    [PrecisionSlider setFrame:prec];

    [superview addSubview:ExpressionField];
    [superview addSubview:PrecisionSlider];

    [ExpressionField selectText:sender];

    shrinking = ! shrinking;
}

- (IBAction)clear:(id)sender
{
    if ([[ExpressionField stringValue] length]) {
	[ExpressionField setStringValue:@""];
	[ExpressionField selectText:self];
    } else if ([[AnswerField stringValue] length]) {
	[AnswerField setStringValue:@""];
	[ExpressionField selectText:self];
    }
}

- (IBAction)menuConstant:(id)sender
{
    NSString *str = [ExpressionField stringValue];
    NSString *str2 = @"error!";

    switch ([sender tag]) {
	case 1: str2 = [NSString stringWithFormat:@"%C", 0x03c0]; break;
	case 2: str2 = @"e"; break;
	case 3: str2 = [NSString stringWithFormat:@"%C", 0x03b3]; break;
	case 4: str2 = @"g"; break;
	case 5: str2 = @"Cc"; break;
	case 8: str = @"K"; break;

	case 101: str2 = @"Z0"; break;
	case 102: str2 = [NSString stringWithFormat:@"%C0", 0x03b5]; break;
	case 103: str2 = [NSString stringWithFormat:@"%C0", 0x03bc]; break;
	case 104: str2 = @"G"; break;
	case 105: str2 = @"h"; break;
	case 106: str2 = @"c"; break;

	case 201: str2 = [NSString stringWithFormat:@"%CB", 0x03bc]; break;
	case 202: str2 = [NSString stringWithFormat:@"%CN", 0x03bc]; break;
	case 203: str2 = @"G0"; break;
	case 204: str2 = @"ec"; break;
	case 205: str2 = @"Kj"; break;
	case 206: str2 = @"Rk"; break;
	case 207: str2 = [NSString stringWithFormat:@"%C0", 0x03a6]; break;

	case 301: str2 = [NSString stringWithFormat:@"M%C", 0x03b1]; break;
	case 302: str2 = @"a0"; break;
	case 303: str2 = @"Md"; break;
	case 304: str2 = @"Me"; break;
	case 305: str2 = @"re"; break;
	case 306: str2 = @"eV"; break;
	case 307: str2 = @"Gf"; break;
	case 308: str2 = [NSString stringWithFormat:@"%C", 0x03b1]; break;
	case 309: str2 = @"Eh"; break;
	case 310: str2 = @"Mh"; break;
	case 311: str2 = [NSString stringWithFormat:@"m%C", 0x03bc]; break;
	case 312: str2 = @"Mn"; break;
	case 313: str2 = @"Mp"; break;
	case 314: str2 = [NSString stringWithFormat:@"R%C", 0x221E]; break;
	case 315: str2 = [NSString stringWithFormat:@"M%C", 0x03c4]; break;

	case 401: str2 = @"u"; break;
	case 402: str2 = @"Na"; break;
	case 403: str2 = @"k"; break;
	case 404: str2 = @"F"; break;
	case 405: str2 = @"c1"; break;
	case 406: str2 = @"n0"; break;
	case 407: str2 = @"R"; break;
	case 408: str2 = @"Vm"; break;
	case 409: str2 = @"c2"; break;
	case 410: str2 = [NSString stringWithFormat:@"%C", 0x03c3]; break;
	case 411: str2 = @"b"; break;

	case 6: str2 = @"random"; break;
	case 7: str2 = @"irandom"; break;
	default: return;
    }
    if ([str length]) {
	NSString *str3 = [NSString stringWithFormat:@"%C",0x00d7];
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
	case 28: str2 = @"Gamma("; break;
	case 29: str2 = @"lnGamma("; break;
	case 30: str2 = @"zeta("; break;
	case 31: str2 = @"sec("; break;
	case 32: str2 = @"csc("; break;
	case 33: str2 = @"asec("; break;
	case 34: str2 = @"acsc("; break;
	case 35: str2 = @"sech("; break;
	case 36: str2 = @"csch("; break;
	case 37: str2 = @"asech("; break;
	case 38: str2 = @"acsch("; break;
	case 39: str2 = @"eint("; break;
	case 40: str2 = @"sinc("; break;
	case 41: str2 = @"exp("; break;
	case 42: str2 = @"fact("; break;
	case 43: str2 = @"comp("; break;
	default: return;
    }
    [ExpressionField setStringValue:[str2 stringByAppendingString:str]];
}

- (void)awakeFromNib
{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    NSRect w;
    NSSize bounds;
    int prefsVersion;

    Dprintf("awakeFromNib\n");
    prefsVersion = [prefs integerForKey:@"initialized"];
    Dprintf("prefsVersion: %i\n", prefsVersion);
    if (prefsVersion < 1) {
	Dprintf("Upgrading prefs to v1\n");
	[prefs setObject:@"-1" forKey:@"precision"];
	[prefs setObject:@"NO" forKey:@"historyDuplicatesAllowed"];
	[prefs setObject:@"NO" forKey:@"flagUndefinedVariables"];
	[prefs setObject:@"YES" forKey:@"useRadians"];
	[prefs setObject:@"0" forKey:@"outputFormat"];
	[prefs setObject:@"YES" forKey:@"printPrefixes"];
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
	[prefs setObject:@"NO" forKey:@"printDelimiters"];
    }
    if (prefsVersion < 2) {
	Dprintf("Upgrading prefs to v2\n");
	[prefs setObject:@"YES" forKey:@"livePrecision"];
	[prefs setObject:@"1024" forKey:@"internalPrecision"];
	[prefs setObject:@"YES" forKey:@"cModStyle"];
    }
    if (prefsVersion < 3) {
	Dprintf("Upgrading prefs to v3\n");
	[prefs setObject:@"3" forKey:@"initialized"];
	[prefs setObject:@"147" forKey:@"maxSliderPrecision"];
    }
    if (prefsVersion < 4) {
	Dprintf("Upgrading prefs to v4\n");
	[prefs setObject:@"4" forKey:@"initialized"];
	[prefs setObject:@"1" forKey:@"engineeringNotation"];
    }
    if (prefsVersion < 5) {
	Dprintf("Upgrading prefs to v5\n");
	[prefs setObject:@"5" forKey:@"initialized"];
	[prefs setObject:@"" forKey:@"alternateInputDecimalSeparator"];
	[prefs setObject:@"" forKey:@"alternateInputThousandsSeparator"];
    }
    conf.precision = [prefs integerForKey:@"precision"];
    switch([prefs integerForKey:@"engineeringNotation"]) {
		case 1: conf.engineering = automatic; break;
		case 2: conf.engineering = never; break;
		case 3: conf.engineering = always; break;
	}
    conf.picky_variables = [prefs boolForKey:@"flagUndefinedVariables"];
    conf.use_radians = [prefs boolForKey:@"useRadians"];
    conf.output_format = [prefs integerForKey:@"outputFormat"];
    conf.print_prefixes = [prefs boolForKey:@"printPrefixes"];
    /*	conf.strict_syntax = [prefs boolForKey:@"strictSyntax"]; */
    conf.rounding_indication = [prefs integerForKey:@"roundingIndication"];
    conf.precision_guard = [prefs boolForKey:@"precisionGuard"];
    conf.print_ints = [prefs boolForKey:@"printInts"];
    conf.print_commas = [prefs boolForKey:@"printDelimiters"];
    conf.simple_calc = [prefs boolForKey:@"simpleCalc"];
    conf.live_precision = [prefs boolForKey:@"livePrecision"];
    [PrecisionSlider setMaxValue:[prefs floatForKey:@"maxSliderPrecision"]];
    /* history preferences */
    allow_duplicates = [prefs boolForKey:@"historyDuplicatesAllowed"];
    update_history = [prefs boolForKey:@"updateHistory"];
    conf.remember_errors = [prefs boolForKey:@"rememberErrors"];
    conf.history_limit = [prefs boolForKey:@"historyLimit"];
    conf.history_limit_len = [prefs integerForKey:@"historyLimitLen"];
    {
	NSDictionary *temp = [prefs dictionaryForKey:@"persistentVariables"];
	NSEnumerator *enumerator = [temp keyEnumerator];
	id key;

	while ((key = [enumerator nextObject])) {
	    putexp(strdup([key UTF8String]),strdup([[temp objectForKey:key] UTF8String]),"preloaded");
	}
    }
    if ([prefs integerForKey:@"internalPrecision"] < 32) {
	    [prefs setObject:@"1024" forKey:@"internalPrecision"];
    }
    if ([prefs integerForKey:@"internalPrecision"] > 4096) {
	    [prefs setObject:@"4096" forKey:@"internalPrecision"];
    }
    init_numbers();
    num_set_default_prec([prefs integerForKey:@"internalPrecision"]);
    Dprintf("preferences read\n");

    [PrecisionSlider setIntValue:conf.precision];
    Dprintf("precision slider\n");
    just_answered = FALSE;

    /* Set up the character translation */
    NSNumberFormatter *numFormat = [[[NSNumberFormatter alloc] init] autorelease];
    if ([[numFormat decimalSeparator] length] > 0) {
	Dprintf("NSDecimalSeparator > 0\n");
	conf.dec_delimiter = [[numFormat decimalSeparator] characterAtIndex:0];
    } else {
	conf.dec_delimiter = '.';
    }
    Dprintf("NSDecimalSeparator set (%c)\n", conf.dec_delimiter);
    if ([[numFormat groupingSeparator] length] > 0) {
	Dprintf("NSGroupingSeparator > 0\n");
	conf.thou_delimiter = [[numFormat groupingSeparator] characterAtIndex:0];
    } else {
	conf.thou_delimiter = 0;
    }
    Dprintf("NSGroupingSeparator set (%c)\n", conf.thou_delimiter);
    if ([prefs integerForKey:@"alternateInputDecimalSeparator"]) {
	if ([[prefs stringForKey:@"alternateInputDecimalSeparator"] length] > 0) {
	    conf.in_dec_delimiter = [[prefs stringForKey:@"alternateInputDecimalSeparator"] characterAtIndex:0];
	}
	if ([[prefs stringForKey:@"alternateInputThousandsSeparator"] length] > 0) {
	    conf.in_thou_delimiter = [[prefs stringForKey:@"alternateInputThousandsSeparator"] characterAtIndex:0];
	}
	if (conf.in_dec_delimiter != 0) {
	    [decimalKey setTitle:[NSString stringWithCString:&conf.in_dec_delimiter length:1]];
	}
    } else {
	[decimalKey setTitle:[numFormat decimalSeparator]];
	Dprintf("decimalKey title set\n");
    }

    /* reset the window to the saved setting */
    superview = [keypad superview];
    [keypad retain];
    [PrecisionSlider retain];
    [ExpressionField retain];
    Dprintf("interface retained\n");
    [mainWindow useOptimizedDrawing:TRUE];
    [mainWindow setFrameAutosaveName:@"wcalc"];
    Dprintf("frame autosave set\n");
    w = [mainWindow frame];
    if ([prefs boolForKey:@"toggled"]) {
	Dprintf("window is toggled\n");
	w.size.height += KEYPAD_HEIGHT;
	w.origin.y -= KEYPAD_HEIGHT;
	[mainWindow setFrame:w display:TRUE animate:FALSE];
	Dprintf("frame set\n");
	[self toggleSize:0];
	Dprintf("toggled\n");
    } else {
	Dprintf("window not toggled\n");
	w.size.width = MIN_WINDOW_WIDTH;
	[mainWindow setFrame:w display:TRUE animate:FALSE];
	bounds.width = MIN_WINDOW_WIDTH;
	bounds.height = MIN_WINDOW_HEIGHT_UNTOGGLED;
	[mainWindow setMinSize:bounds];
	bounds.width = MIN_WINDOW_WIDTH;
	bounds.height = MAX_WINDOW_SIZE;
	[mainWindow setMaxSize:bounds];
	Dprintf("window size restored\n");
    }
    //	w = [mainWindow frame];
    //	bounds = [mainWindow minSize];

    e = enterKey;
    ef = ExpressionField;

    /* this restores the drawer states */
    if ([prefs boolForKey:@"historyShowing"]) {
	Dprintf("history showing\n");
	[NSTimer scheduledTimerWithTimeInterval:0 target:self selector:@selector(openIDrawer:) userInfo:nil repeats:NO];
	Dprintf("history drawer displayed\n");
    }
    if ([prefs boolForKey:@"baseShowing"]) {
	Dprintf("base showing\n");
	[NSTimer scheduledTimerWithTimeInterval:0 target:self selector:@selector(openBDrawer:) userInfo:nil repeats:NO];
	Dprintf("base drawer displayed\n");
    }
    Dprintf("done with drawers\n");

    /* set the correct expression display for simple_calc */
    if (conf.simple_calc) {
	Dprintf("simple calc\n");
	[ExpressionField setStringValue:@"0"];
	[AnswerField setStringValue:@"0"];
	Dprintf("values zeroed\n");
	simpleClearAll();
	Dprintf("simple all cleared\n");
    }
    num_init_set_ui(last_answer, 0);
    Dprintf("last answer cleared\n");
    pthread_mutex_init(&displayLock,NULL);
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(quit:) name:NSWindowWillCloseNotification object:mainWindow];
    NSUpdateDynamicServices();
}

- (void)openBDrawer: (id) sender
{
    [baseDrawer open];
    [baseMenu setTitle:@"Close Base Drawer"];
}

- (void)openIDrawer: (id) sender
{
    [inspector openIt:sender];
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

    Dprintf("setPrecision: %i to %i\n",conf.precision,last_pres);
    conf.precision = last_pres;
    [prefs setObject:[NSString stringWithFormat:@"%i",conf.precision] forKey:@"precision"];
    Dprintf("setPrecision - prefs set\n");

    Dprintf("getting lock\n");
    pthread_mutex_lock(&displayLock);
    Dprintf("locked\n");
    {
	set_prettyanswer(last_answer);
	Dprintf("setPrecision - set_prettyanswer\n");
	[self displayAnswer];
	Dprintf("setPrecision - done\n");
    }
    pthread_mutex_unlock(&displayLock);
    Dprintf("unlocked\n");
}

- (IBAction)go:(id)sender
{
    char * expression;
    extern char * errstring;

    expression = strdup([[ExpressionField stringValue] UTF8String]);

    parseme(expression);
    putval("a",last_answer,"previous answer");

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

    Dprintf("displayAnswer\n");
    /* if there is an error, display it */
    if (errstring && strlen(errstring)) {
	extern int scanerror;
	scanerror = 0;
	Dprintf("displayAnswer error %s\n",errstring);
	[errorController throwAlert:[NSString stringWithUTF8String:errstring]];
	free(errstring);
	errstring = NULL;
    }
    /* display the answer */
    Dprintf("displayAnswer - pretty_answer(%p)\n",pretty_answer);
    Dprintf("displayAnswer - %s\n",pretty_answer);
    [AnswerField setStringValue:[NSString stringWithFormat:@"%s",pretty_answer]];
    [AnswerField setTextColor:(not_all_displayed?[NSColor redColor]:[NSColor blackColor])];
    Dprintf("displayAnswer - changing size\n");
    { // Make the Answerfield big enough to display the answer
	NSRect curFrame, newFrame;
	curFrame = [AnswerField frame];
	Dprintf("displayAnswer - current size: %f x %f\n", curFrame.size.height, curFrame.size.width);
	newFrame = curFrame;
	// we set the height to a huge number because cellSizeForBounds only
	// works "within" the specified bound, which we want because we want to
	// limit the width.
	newFrame.size.height = DBL_MAX; 
	newFrame.size = [[AnswerField cell] cellSizeForBounds:newFrame];
	Dprintf("displayAnswer - needed size: %f x %f\n", newFrame.size.height, newFrame.size.width);
	if (curFrame.size.height != newFrame.size.height) {
	    size_t newHeight = newFrame.size.height;
	    int difference = newHeight - curFrame.size.height;
	    NSRect windowFrame = [mainWindow frame];
	    int nonAnswerPixels = windowFrame.size.height - curFrame.size.height;
	    NSSize ms = [mainWindow minSize];

	    newFrame = curFrame;
	    newFrame.size.height = newHeight;
	    ms.height += difference;
	    windowFrame.size.height += difference;
	    if (windowFrame.size.height < newFrame.size.height + nonAnswerPixels) {
		windowFrame.size.height = newFrame.size.height + nonAnswerPixels;
	    }
	    windowFrame.origin.y -= difference;
	    curFrame = [ExpressionField frame];
	    [AnswerField setHidden:TRUE];
	    [ExpressionField setHidden:TRUE];
	    [mainWindow setFrame:windowFrame display:YES animate:YES];
	    windowFrame = [mainWindow frame];
	    /* At this point, the windowFrame may not be *quite* what I wanted it to be,
		screens being of finite size and all... so we have to recalculate the size
		that the AnswerField should be. */
	    newFrame.size.height = windowFrame.size.height - nonAnswerPixels;
	    [mainWindow setMinSize:ms];
	    [AnswerField setFrame:newFrame];
	    [ExpressionField setFrame:curFrame];
	    [AnswerField setHidden:FALSE];
	    [ExpressionField setHidden:FALSE];
	}
    }
    Dprintf("displayAnswer - refresh inspector\n");
    // if the drawer is open, refresh the data.
    if ([inspectorWindow isVisible]) {
	[variableList reloadData];
	[historyList reloadData];
    }
    just_answered = TRUE;
    // refresh the prefs if necessary
    Dprintf("displayAnswer - refresh prefs\n");
    if ([thePrefPanel isVisible])
	[self displayPrefs:0];
    Dprintf("displayAnswer - cleaning up\n");
    [outputFormat2 selectCellWithTag:conf.output_format];
    [ExpressionField selectText:self];
    Dprintf("displayAnswer - done\n");
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
    [AnswerField setStringValue:[NSString stringWithUTF8String:(pretty_answer?pretty_answer:"Not Enough Memory")]];
    putval("a",last_answer,"previous answer");
    if ([inspectorWindow isVisible]) {
	[variableList reloadData];
    }
}

- (IBAction)enterData:(id)sender
{
    NSString * sent = [NSString stringWithString:[[sender attributedTitle] string]];
    static short shiftdown = 0, capsdown = 0;
    int tag;

    Dprintf("enterData\n");
    [ExpressionField setSelectable:FALSE];
    tag = [sender tag];
    switch (tag) {
	case 101: /* delete key on the onscreen keyboard */
	    if ([[ExpressionField stringValue] length] > 0) {
		unsigned len = [[ExpressionField stringValue] length];
		[ExpressionField setStringValue:[[ExpressionField stringValue] substringToIndex:len-1]];
	    }
	    break;
	case 100: /* clear key on the onscreen keypad */
	    if (!conf.simple_calc) {
		if ([[ExpressionField stringValue] length] > 0) {
		    [ExpressionField setStringValue:@""];
		    [ExpressionField selectText:self];
		} else if ([[AnswerField stringValue] length] > 0) {
		    [AnswerField setStringValue:@""];
		    [ExpressionField selectText:self];
		}
	    } else {
		if (! [[ExpressionField stringValue] isEqualToString:@"0"]) {
		    [ExpressionField setStringValue:@"0"];
		    [ExpressionField selectText:self];
		    simpleClearEntry();
		} else {
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
		char * expr = strdup([[ExpressionField stringValue] UTF8String]);
		char * ret;
		ret = simpleCalc('=',expr);
		if (ret) {
		    [ExpressionField setStringValue:[NSString stringWithUTF8String:ret]];
		    free(ret);
		} else {
		    [self displayAnswer];
		    [ExpressionField setStringValue:[AnswerField stringValue]];
		}
		free(expr);
	    }
	    break;
	case 105: /* the divide key on the onscreen keypad */
	case 106: /* the minus key on the onscreen keypad */
	case 107: /* the times key on the onscreen keypad */
	default:
	    if (! conf.simple_calc) { /* the real power of Wcalc */
		if (just_answered == FALSE) {
		    [ExpressionField setStringValue:[[ExpressionField stringValue] stringByAppendingString:sent]];
		} else if ([sent isEqualToString:@"+"] ||
			   [sent isEqualToString:@"-"] ||
			   [sent isEqualToString:@"*"] ||
			   [sent isEqualToString:@"/"] ||
			   [sent isEqualToString:@"%"] ||
			   [sent isEqualToString:@"("] ||
			   [sent isEqualToString:@"&"] ||
			   [sent isEqualToString:@"|"] ||
			   [sent isEqualToString:[NSString stringWithFormat:@"%C",0x00f7]]) {
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
		char *ret, *expr;
		expr = strdup([[ExpressionField stringValue] UTF8String]);
		Dprintf("expr: %s\n",expr);
		switch (tag) {
		    case 105: ret = simpleCalc('/', expr); break;
		    case 106: ret = simpleCalc('-', expr); break;
		    case 107: ret = simpleCalc('*', expr); break;
		    default:  ret = simpleCalc([sent characterAtIndex:0], expr);
		}
		free(expr);
		if (ret) {
		    [ExpressionField setStringValue:[NSString stringWithUTF8String:ret]];
		    free(ret);
		} else {
		    [self displayAnswer];
		    [ExpressionField setStringValue:[AnswerField stringValue]];
		}
	    }
	    break;
    }
    [ExpressionField setEditable:TRUE];
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

- (IBAction)showKeyboardDrawer:(id)sender
{
    static char initialized = 0;
    if (! [theKeyboard isVisible]) {
	[theKeyboard setBecomesKeyOnlyIfNeeded:YES];
	[theKeyboard orderFront:self];
	if (! initialized) {
	    [theKeyboard center];
	    initialized = 1;
	}
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
	[baseMenu setTitle:@"Close Base Drawer"];
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
    int tag = [sender tag];

    switch (tag) {
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
	    {
		int savedas = 1;
		olde = conf.engineering;
		printf("index: %i\n", [sender indexOfSelectedItem]);
		switch([sender indexOfSelectedItem]) {
		    case 0: conf.engineering = automatic; savedas = 1; break;
		    case 1: conf.engineering = never; savedas = 2; break;
		    case 2: conf.engineering = always; savedas = 3; break;
		}
		if (olde != conf.engineering) {
		    need_redraw = 1;
		    printf("integet: %i\n", conf.engineering);
		    [prefs setObject:[NSString stringWithFormat:@"%i",savedas] forKey:@"engineeringNotation"];
		}
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
		[printPrefixes setEnabled:(conf.output_format!=DECIMAL_FORMAT)];
		[engineeringNotation setEnabled:(conf.output_format==DECIMAL_FORMAT)];
	    }
	    [outputFormat selectCellWithTag:conf.output_format];
	    [outputFormat2 selectCellWithTag:conf.output_format];
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
	    if ((unsigned)[sender intValue] != conf.history_limit_len) {
		conf.history_limit_len = [sender intValue];
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
	    break;
	case 17:
	    olde = conf.print_commas;
	    conf.print_commas = ([sender state]==NSOnState);
	    if (olde != conf.print_commas) {
		need_redraw = 1;
		[prefs setObject:(conf.print_ints?@"YES":@"NO") forKey:@"printDelimiters"];
	    }
	    break;
	case 18:
	    if ((unsigned)[sender intValue] != num_get_default_prec()) {
		[bitsStepper takeIntValueFrom:sender];
		num_set_default_prec([bitsStepper intValue]); // to handle limits placed on bitsStepper
		[prefs setObject:[NSString stringWithFormat:@"%lu",num_get_default_prec()] forKey:@"internalPrecision"];
	    }
	    break;
	case 19:
	    if ((unsigned)[bitsStepper intValue] != num_get_default_prec()) {
		num_set_default_prec([bitsStepper intValue]);
		need_redraw = 2;
		[bitsPref takeIntValueFrom:bitsStepper];
		[prefs setObject:[NSString stringWithFormat:@"%lu",num_get_default_prec()] forKey:@"internalPrecision"];
	    }
	    break;
	case 20:
	    olde = conf.live_precision;
	    conf.live_precision = ([sender state] == NSOnState);
	    if (olde != conf.print_commas) {
		[prefs setObject:(conf.live_precision?@"YES":@"NO") forKey:@"livePrecision"];
		[PrecisionSlider setContinuous:(conf.live_precision?true:false)];
	    }
	    break;
	case 21:
	    olde = conf.c_style_mod;
	    conf.c_style_mod = ([sender state] == NSOnState);
	    if (olde != conf.c_style_mod) {
		need_redraw = 2;
		[prefs setObject:(conf.c_style_mod?@"YES":@"NO") forKey:@"cModStyle"];
	    }
	    break;
	case 22:
	    if ((unsigned)[sliderStepper intValue] != [PrecisionSlider maxValue]) {
		unsigned long tmp = [sliderStepper intValue];
		[PrecisionSlider setMaxValue:(double)tmp];
		[sliderPref takeIntValueFrom:sliderStepper];
		[prefs setObject:[NSString stringWithFormat:@"%lu",tmp] forKey:@"maxSliderPrecision"];
	    }
	    break;
	case 23:
	    if ((double)[sender intValue] != [PrecisionSlider maxValue]) {
		[sliderStepper takeIntValueFrom:sender];
		// these handle limits placed on sliderStepper
		[sliderPref takeIntValueFrom:sliderStepper];
		[PrecisionSlider setMaxValue:[sliderStepper doubleValue]];
		[prefs setObject:[NSString stringWithFormat:@"%lu",[sliderStepper intValue]] forKey:@"maxSliderPrecision"];
	    }
	    break;
	case 24:
	    [altInputDecSep setEnabled:([sender state] == NSOnState)];
	    [altInputThouSep setEnabled:([sender state] == NSOnState)];
	    if ([sender state] != NSOnState) {
		[altInputDecSep setStringValue:@""];
		[altInputThouSep setStringValue:@""];
		[decimalKey setTitle:[NSString stringWithCString:&conf.dec_delimiter length:1]];
	    }
	    break;
	case 25:
	    if ([[sender stringValue] length] > 0) {
		conf.in_dec_delimiter = [[sender stringValue] characterAtIndex:0];
		[decimalKey setTitle:[NSString stringWithCString:&conf.in_dec_delimiter length:1]];
	    } else {
		conf.in_dec_delimiter = 0;
		[decimalKey setTitle:[NSString stringWithCString:&conf.dec_delimiter length:1]];
	    }
	    break;
	case 26:
	    if ([[sender stringValue] length] > 0) {
		conf.in_thou_delimiter = [[sender stringValue] characterAtIndex:0];
	    } else {
		conf.in_thou_delimiter = 0;
	    }
	default: return;
    }

    switch (need_redraw) {
	case 2:
	    if (update_history)
		recalculate = 1;
	    [self go:sender];
	    break;
	case 1:
	    set_prettyanswer(last_answer);
	    [AnswerField setStringValue:[NSString stringWithUTF8String:(pretty_answer?pretty_answer:"Not Enough Memory")]];
	    [AnswerField setTextColor:((not_all_displayed)?([NSColor redColor]):([NSColor blackColor]))];
	    break;
    }
    if ([inspectorWindow isVisible] || recalculate) {
	[historyList reloadData];
    }
    [ExpressionField selectText:self];
}

- (IBAction)showPrefs:(id)sender
{
    //static char initialized = 0;
    [self displayPrefs:sender];
    [thePrefPanel setBecomesKeyOnlyIfNeeded:TRUE];
    [thePrefPanel orderFront:self];
    // centering at first
    /*if (! initialized) {
	initialized=1;
	[thePrefPanel center];
    }*/
    // this should be redundant
    [thePrefPanel setFrameAutosaveName:@"wcalc_prefs"];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(savePrefs:) name:NSWindowWillCloseNotification object:thePrefPanel];
}

- (IBAction)savePrefs:(id)sender
{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    
    Dprintf("synching the prefs to disk\n");
    [prefs synchronize];
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
	    retval = loadState(strdup([curFile UTF8String]),1);
	    if ([inspectorWindow isVisible]) {
		[variableList reloadData];
		[historyList reloadData];
	    }
	    /* if there is an error, display it */
	    if (errstring && strlen(errstring)) {
		extern int scanerror;
		scanerror = 0;
		[errorController throwAlert:[NSString stringWithUTF8String:errstring]];
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
    errstr = malloc(strlen(strerror(errno))+[filename length]+3);
    sprintf(errstr,"%s: %s",[filename UTF8String], strerror(errno));
    [errorController throwAlert:[NSString stringWithUTF8String:errstr]];
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
	retval = saveState(strdup([curFile UTF8String]));
	if (retval)
	    [self displayErrno:retval forFile:curFile];
    }
}

- (IBAction)displayPrefs:(id)sender
{
    switch (conf.engineering) {
	case automatic: [engineeringNotation selectItemAtIndex:0]; break;
	case never: [engineeringNotation selectItemAtIndex:1]; break;
	case always: [engineeringNotation selectItemAtIndex:2]; break;
    }
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
    [printDelimiters setState:(conf.print_commas?NSOnState:NSOffState)];
    [bitsPref setIntValue:num_get_default_prec()];
    [bitsStepper setIntValue:num_get_default_prec()];
    [livePrecision setState:(conf.live_precision?NSOnState:NSOffState)];
    [cModPref setState:(conf.c_style_mod?NSOnState:NSOffState)];
    [sliderPref setIntValue:(int)[PrecisionSlider maxValue]];
    [sliderStepper setIntValue:(int)[PrecisionSlider maxValue]];
    [alternateInputPref setState:(conf.in_dec_delimiter != 0 || conf.in_thou_delimiter != 0)];

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
    [altInputDecSep setEnabled:[alternateInputPref state]];
    [altInputThouSep setEnabled:[alternateInputPref state]];

    [limitHistoryLen setStringValue:[NSString stringWithFormat:@"%lu",conf.history_limit_len]];
    if ([alternateInputPref state]) {
	[altInputDecSep setStringValue:[NSString stringWithFormat:@"%c",conf.in_dec_delimiter]];
	[altInputThouSep setStringValue:[NSString stringWithFormat:@"%c",conf.in_thou_delimiter]];
    }
}

- (IBAction)closeWindow:(id)sender
{
    if ([inspectorWindow isKeyWindow]) {
	[inspectorWindow close];
    } else if ([persVarsWindow isKeyWindow]) {
	[persVarsWindow close];
    } else if ([mainWindow isKeyWindow]) {
	[mainWindow close];
    }
}

- (IBAction)quit:(id)sender
{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];

    [prefs synchronize];
    clearHistory();
    cleanupvar();
    if (pretty_answer) {
	extern char *pa;
	free(pretty_answer);
	if (pa) {
	    free(pa);
	}
    }
    num_free(last_answer);
    lists_cleanup();
    pthread_mutex_destroy(&displayLock);
    exit(EXIT_SUCCESS);
}

@end
