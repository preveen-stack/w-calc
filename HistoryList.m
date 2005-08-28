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
		return @"BAD ROW";

	return [NSString stringWithCString:val];
}

- (IBAction)rowSelected:(id)sender
{
	int row = [sender selectedRow];
//	printf("selected: %i\n",[sender selectedRow]);
	if (row > -1) {
		[expressionField setStringValue:[NSString stringWithCString:historynum([sender selectedRow],1)]];
	}
	[expressionField selectText:self];
}

- (IBAction)clearHistory:(id)sender
{
    clearHistory();
    [theList reloadData];
}

@end
