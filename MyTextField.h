/* MyTextField */

#import <Cocoa/Cocoa.h>
#import "WcalcController.h"

#import <gmp.h>
#import <mpfr.h>

@interface MyTextField : NSTextField
{
	IBOutlet WcalcController *mainController;
	IBOutlet NSTextField *AnswerField;

	/* "Simple Calculator" Variables */
	char lastchar;
	char oper;
	mpfr_t cur_number;
	mpfr_t prev_number;
}
- (void)keyDown:(NSEvent*) theEvent;
- (void)keyUp:(NSEvent*) theEvent;
- (BOOL) acceptsFirstResponder;
- (BOOL) refusesFirstResponder;
@end
