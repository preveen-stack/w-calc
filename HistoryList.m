#import "historyManager.h"
#import "HistoryList.h"

@implementation HistoryList

- (int)numberOfRowsInTableView:(NSTableView *)atv
{
//	printf("history length = %i\n",history_length());
	return history_length();
}

- (id)tableView:(NSTableView *)atv objectValueForTableColumn:(NSTableColumn*)col row:(int)rowIndex
{
	char * val = historynum(rowIndex);
//	printf("%i.)> %s\n", rowIndex, val);
	if (! val)
		return @"BAD ROW";

	return [NSString stringWithCString:val];
}

- (IBAction)rowSelected:(id)sender
{
	int row = [sender selectedRow];
//	printf("selected: %i\n",[sender selectedRow]);
	if (row > -1) {
		[expressionField setStringValue:[NSString stringWithCString:historynum([sender selectedRow])]];
	}
	[expressionField selectText:self];
}

@end
