#import "historyManager.h"
#import "HistoryList.h"

@implementation HistoryList

- (int)numberOfRowsInTableView:(NSTableView *)atv
{
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
//	printf("selected: %i\n",[sender selectedRow]);
	[expressionField setStringValue:[NSString stringWithCString:historynum([sender selectedRow])]];
	[expressionField selectText:self];
}

@end
