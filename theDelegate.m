#include "theDelegate.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif

@implementation theDelegate
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)theApp
{
	return YES;
}
@end
