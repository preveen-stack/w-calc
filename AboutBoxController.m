#import "calculator.h"
#import "AboutBoxController.h"

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
