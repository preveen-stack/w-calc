#import "MyTextField.h"

@implementation MyTextField
- (void)keyDown:(NSEvent*) theEvent
{
	//printf("keydown\n");
}

- (void)keyUp:(NSEvent*) theEvent
{
	int keycode = [theEvent keyCode];
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
			if (strlen(temp)) {
				temp[strlen(temp)-1] = '\0';
				[ef setStringValue:[NSString stringWithCString:temp]];
			}
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
			printf("keyup %i => ", keycode);
			fflush(NULL);
			printf("%s\n", [[theEvent characters] cString]);
			break;
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
