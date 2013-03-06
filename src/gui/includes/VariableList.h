/* VariableList */

#include <Cocoa/Cocoa.h>

@interface VariableList : NSObject
{
	IBOutlet NSTableView *theList;
}
- (int)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn*) aTableColumn row:(int)rowIndex;
- (void)tableView:(NSTableView*)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn*)aTableColumn row:(int)rowIndex;
- (IBAction)newVariable:(id)sender;
- (IBAction)delVariable:(id)sender;
- (IBAction)clearVariables:(id)sender;
- (IBAction)copyMe:(id)sender;
@end
