/* MyTextField */

#import <Cocoa/Cocoa.h>
#import "WcalcController.h"

@interface MyTextField : NSTextField
{
	IBOutlet WcalcController *mainController;
}
- (void)keyDown:(NSEvent*) theEvent;
- (void)keyUp:(NSEvent*) theEvent;
@end
