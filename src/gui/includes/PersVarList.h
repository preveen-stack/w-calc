/* PersVarList */

#include <Cocoa/Cocoa.h>
#include "list.h"

@interface PersVarList : NSObject
{
    IBOutlet NSTableView *theList;
	IBOutlet NSWindow *theWindow;	
}
- (IBAction)addVar:(id)sender;
- (IBAction)clearVars:(id)sender;
- (IBAction)delVar:(id)sender;
- (IBAction)showList:(id)sender;
@end
