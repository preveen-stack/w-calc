#import "MyTextField.h"
#import "calculator.h"
#import "variables.h"
#import "simpleCalc.h"

@implementation MyTextField
- (void)keyDown:(NSEvent*) theEvent
{
	/*printf("mtf keydown\n");
	fflush(NULL);*/
}

- (void)keyUp:(NSEvent*) theEvent
{
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
				temp = strdup([[ef stringValue] cString]);
				while (strlen(temp) && temp[strlen(temp)-1]=='=') {
					temp[strlen(temp)-1] = '\0';
				}
				[ef setStringValue:[NSString stringWithCString:temp]];
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
 //			printf("%s\n", [[theEvent characters] cString]);
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
			default:
			{
				char thechar = [[theEvent characters] characterAtIndex:0];
				char * ret;
				char * exp;
				exp = strdup([[self stringValue] cString]);
				exp[strlen(exp)-1] = 0;
				ret = simpleCalc(thechar,exp);
				if (ret) {
					[self setStringValue:[NSString stringWithCString:ret]];
					free(ret);
				} else {
					[self setStringValue:[NSString stringWithCString:exp]];
					[mainController displayAnswer];
				}
				free(exp);
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
