/* MyTextField */

#include <Cocoa/Cocoa.h>
#include "WcalcController.h"

#include "number.h"

@interface MyTextField : NSTextField
{
	IBOutlet WcalcController *mainController;
	IBOutlet NSTextField *AnswerField;

	/* "Simple Calculator" Variables */
	char lastchar;
	char oper;
	Number cur_number;
	Number prev_number;
}
- (void)keyDown:(NSEvent*) theEvent;
- (void)keyUp:(NSEvent*) theEvent;
- (BOOL) acceptsFirstResponder;
- (BOOL) refusesFirstResponder;
@end
