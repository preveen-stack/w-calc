/* InspectorController */

#import <Cocoa/Cocoa.h>

@interface InspectorController : NSObject
{
    IBOutlet NSPanel *inspectorWindow;
    IBOutlet NSMenuItem *affectInspectorMenu;
    IBOutlet NSTableView *variableList;
    IBOutlet NSTableView *historyList;
}
- (IBAction)closeIt:(id)sender;
- (IBAction)openIt:(id)sender;
- (IBAction)toggleIt:(id)sender;
- (IBAction)onClose:(id)sender;
@end
