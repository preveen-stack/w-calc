/*
 *  VariableList.h
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Sun Jan 20 2002.
 *  Copyright (c) 2001 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef VARIABLELIST_H
#define VARIABLELIST_H
#import <AppKit/AppKit.h>

@interface NSTableDataSource : NSObject
{
}
- (int)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn*) aTableColumn row:(int)rowIndex;
- (void)tableView:(NSTableView*)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn*)aTableColumn row:(int)rowIndex;
@end
#endif VARIABLELIST_H