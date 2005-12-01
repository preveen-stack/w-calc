/* InspectorController */

#import <Cocoa/Cocoa.h>

@interface InspectorController : NSObject
{
    IBOutlet NSWindow *inspectorWindow;
	IBOutlet NSMenuItem *affectInspectorMenu;
}
- (IBAction)closeIt:(id)sender;
- (IBAction)openIt:(id)sender;
- (IBAction)toggleIt:(id)sender;
- (IBAction)onClose:(id)sender;
@end
