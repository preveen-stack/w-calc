#import "theDelegate.h"

@implementation theDelegate
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)theApp {
	return YES;
}
@end
