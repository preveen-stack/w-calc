/* ConversionList */

#include <Cocoa/Cocoa.h>

@interface ConversionList : NSObject
{
    IBOutlet NSTableView *toList;
    IBOutlet NSTableView *fromList;
    IBOutlet NSPopUpButton *theType;
}
- (int)numberOfRowsInTableView:(NSTableView *)atv;
- (id)tableView:(NSTableView *)atv objectValueForTableColumn:(NSTableColumn*)col row:(int)rowIndex;
- (IBAction)newType:(id)sender;
@end
