/* theDelegate */

#import <Cocoa/Cocoa.h>

@interface theDelegate : NSObject
{
	IBOutlet NSWindow *mainWindow; 
}
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)theApp;
@end
