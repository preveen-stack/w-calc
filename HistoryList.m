#import "historyManager.h"
#import "HistoryList.h"

@implementation HistoryList

- (int)numberOfRowsInTableView:(NSTableView *)atv
{
	return history_length();
}

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

@end
