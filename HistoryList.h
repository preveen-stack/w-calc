/* HistoryList */

#import <Cocoa/Cocoa.h>

#import "WcalcController.h"

@interface HistoryList : NSObject
{
    IBOutlet NSTextField *expressionField;
    IBOutlet NSTableView *theList;
}
- (IBAction)rowSelected:(id)sender;
- (int)numberOfRowsInTableView:(NSTableView *)atv;
- (id)tableView:(NSTableView *)atv objectValueForTableColumn:(NSTableColumn*)col row:(int)rowIndex;
- (IBAction)clearHistory:(id)sender;
@end
