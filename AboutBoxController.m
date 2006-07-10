#include "calculator.h"
#include "AboutBoxController.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif

@implementation AboutBoxController

- (IBAction)closeIt:(id)sender
{
	[aboutBox close];
}

- (IBAction)openIt:(id)sender
{
	[version setStringValue:[NSString stringWithFormat:@"Version %s",VERSION]];
	[aboutBox makeKeyAndOrderFront:self];
	[aboutBox center];
}

@end
