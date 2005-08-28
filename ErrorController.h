/* ErrorController */

#include <Cocoa/Cocoa.h>

@interface ErrorController : NSObject
{
    IBOutlet id errorDialog;
    IBOutlet id errorMessage;
}
- (IBAction)closeAlert:(id)sender;
- (IBAction)showAlert:(id)sender;
- (IBAction)throwAlert:(NSString *)message;
@end
