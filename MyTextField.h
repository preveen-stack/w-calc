/* MyTextField */

#import <Cocoa/Cocoa.h>
#import "WcalcController.h"

@interface MyTextField : NSTextField
{
	IBOutlet WcalcController *mainController;
	IBOutlet NSTextField *AnswerField;
	
	/* "Simple Calculator" Variables */
	char lastchar;
	char oper;
	double cur_number;
	double prev_number;	
}
- (void)keyDown:(NSEvent*) theEvent;
- (void)keyUp:(NSEvent*) theEvent;
- (BOOL) acceptsFirstResponder;
- (BOOL) refusesFirstResponder;
@end
