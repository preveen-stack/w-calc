/* AboutBoxController */

#import <Cocoa/Cocoa.h>

@interface AboutBoxController : NSObject
{
    IBOutlet NSWindow *aboutBox;
}
- (IBAction)closeIt:(id)sender;
- (IBAction)openIt:(id)sender;
@end
