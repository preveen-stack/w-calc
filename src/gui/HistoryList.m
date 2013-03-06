#include "historyManager.h"
#include "HistoryList.h"

@implementation HistoryList

- (int)numberOfRowsInTableView:(NSTableView *)atv
{
	return historyLength();
}

/* returns the correct text for a given column and row */
- (id)tableView:(NSTableView *)atv objectValueForTableColumn:(NSTableColumn*)col row:(int)rowIndex
{
	char * val;

	if ([[col identifier] isEqualToString:@"history"])
		val = historynum(rowIndex,1);
	else
		val = historynum(rowIndex,2);

	if (! val)
		return @"BAD ROW (history manager)";

	return [NSString stringWithUTF8String:val];
}

- (IBAction)rowSelected:(id)sender
{
	int row = [sender selectedRow];
//	printf("selected: %i\n",[sender selectedRow]);
	if (row > -1) {
		[expressionField setStringValue:[NSString stringWithUTF8String:historynum([sender selectedRow],1)]];
	}
	[expressionField selectText:self];
}

- (IBAction)clearHistory:(id)sender
{
    clearHistory();
    [theList reloadData];
}

- (IBAction)copyMe:(id)sender
{
    NSPasteboard* pboard=[NSPasteboard generalPasteboard];
    NSString *theString = @"";
    NSIndexSet* rowEnumerator=[theList selectedRowIndexes];
    unsigned int theIndex=0;

    // Set the pasteboard types you want here.
    [pboard declareTypes:[NSArray arrayWithObject:NSStringPboardType] owner:self];

    theIndex = [rowEnumerator firstIndex];
    do {
	// Here, you'd look up the data in your data source instead, and add data to the pasteboard for one or more of the types.
	theString = [theString stringByAppendingFormat:@"%s\t%s\n", historynum(theIndex,1), historynum(theIndex,2)];
    } while ((theIndex = [rowEnumerator indexGreaterThanIndex:theIndex]) != NSNotFound);
    [pboard setString:theString forType:NSStringPboardType];
}

// ** Make sure we don't enable the copy menu item unless there is something to copy.
- (BOOL)validateMenuItem:(id <NSMenuItem>)menuItem
{
    if ([menuItem tag]==666)
    {
	return ([theList numberOfSelectedRows]>0);
    }
    return YES;
}

@end
