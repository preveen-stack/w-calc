#import "AboutBoxController.h"

@implementation AboutBoxController

- (IBAction)closeIt:(id)sender
{
	[aboutBox close];
}

- (IBAction)openIt:(id)sender
{
	[aboutBox makeKeyAndOrderFront:self];
	[aboutBox center];
}

@end
