#import "ErrorController.h"

@implementation ErrorController

- (IBAction)closeAlert:(id)sender
{
	[errorMessage setString:[NSString stringWithCString:""]];
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
	[errorMessage insertText:[NSString stringWithCString:"\n"]];
	[errorMessage setEditable:false];
	NSBeep();
}
@end
