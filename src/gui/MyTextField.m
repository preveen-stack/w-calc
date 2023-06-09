#include "MyTextField.h"
#include "calculator.h"
#include "variables.h"
#include "simpleCalc.h"

@implementation MyTextField
- (void)keyDown:(NSEvent*) theEvent
{
	printf("mtf keydown\n");
	fflush(NULL);
}

- (void)keyUp:(NSEvent*) theEvent
{
    static char * ret = NULL;
	int keycode = [theEvent keyCode];
	//printf("mtf keyup\n"); fflush(NULL);
	if (! conf.simple_calc) {
		//printf("not simple_calc\n");
		switch (keycode) {
			case 71: // the clear button
				[self setStringValue:@""];
				//[self selectText:self];
				break;
			case 81: // =
			//		case 24:
			{
				char * temp;
				extern NSButton *e;
				extern NSTextField *ef;
				temp = strdup([[ef stringValue] UTF8String]);
				while (strlen(temp) && temp[strlen(temp)-1]=='=') {
					temp[strlen(temp)-1] = '\0';
				}
				[ef setStringValue:[NSString stringWithUTF8String:temp]];
				if (temp) free(temp);
				[e performClick:self];
				break;
			}
			case 123: // leftarrow
			case 124: // rightarrow
			case 126: // uparrow
			case 125: // downarrow
				break;
			default:
				//			printf("keyup %i => ", keycode);
	//			fflush(NULL);
 //			printf("%s\n", [[theEvent characters] UTF8String]);
				break;
		}
	} else {
		//printf("here, keycode: %i\n", keycode);
		switch(keycode) {
			case 123: // leftarrow
			case 124: // rightarrow
			case 126: // uparrow
			case 125: // downarrow
				break;
			case 71:
				if (! [[self stringValue] isEqualToString:@"0"]) {
					[self setStringValue:@"0"];
					[self selectText:self];
					simpleClearEntry();
				} else if (! [[AnswerField stringValue] isEqualToString:@"0"]) {
					[AnswerField setStringValue:@"0"];
					[self selectText:self];
					simpleClearAll();
				}
				break;
			case 76: // enter
			{
			    char *expr = strdup([[self stringValue] UTF8String]);
			    ret = simpleCalc('=', expr);
			    if (ret) {
				[self setStringValue:[NSString stringWithUTF8String:ret]];
			    } else {
				[mainController displayAnswer];
				[self setStringValue:[AnswerField stringValue]];
				ret = strdup([[AnswerField stringValue] UTF8String]);
			    }
			    free(expr);
			    break;
			}
			case 117: // delete
			case 51: // backspace
			    if (! [[self stringValue] isEqualToString:@"0"]) {
				[self setStringValue:@"0"];
				[self selectText:self];
				simpleClearEntry();
			    } else {
				[AnswerField setStringValue:@"0"];
				[self selectText:self];
				simpleClearAll();
			    }
			    if (ret) {
				free(ret);
			    }
			    ret = strdup("0");
			    break;
			default:
			{
				char thechar = [[theEvent characters] characterAtIndex:0];
				char * expr;
				unsigned len = [[self stringValue] length];
				if (ret) {
				    expr = ret;
				} else {
				    expr = strdup([[[self stringValue] substringToIndex:len-1] UTF8String]);
				}
				ret = simpleCalc(thechar,expr);
				if (ret) {
					[self setStringValue:[NSString stringWithUTF8String:ret]];
				} else {
					[mainController displayAnswer];
					[self setStringValue:[AnswerField stringValue]];
					ret = strdup([[AnswerField stringValue] UTF8String]);
				}
				free(expr);
				break;
			}
		}
	}
}

- (BOOL) acceptsFirstResponder
{
	return YES;
}

- (BOOL) refusesFirstResponder
{
	return NO;
}

@end
