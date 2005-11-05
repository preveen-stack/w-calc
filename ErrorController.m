#include "ErrorController.h"

@implementation ErrorController

- (IBAction)closeAlert:(id)sender
{
	[errorMessage setString:@""];
	[errorDialog close];
}

- (IBAction)showAlert:(id)sender
{
	[errorDialog makeKeyAndOrderFront:self];
	[errorDialog center];
}

- (IBAction)throwAlert:(NSString *)message
{
	[errorDialog makeKeyAndOrderFront:self];
	[errorDialog center];
	[errorMessage setEditable:true];
	[errorMessage insertText:message];
	[errorMessage insertText:@"\n"];
	[errorMessage setEditable:false];
	NSBeep();
}
@end
