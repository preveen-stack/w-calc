//
//  VariableList.m
//  Wcalc
//
//  Created by Kyle Wheeler on Sun Jan 20 2002.
//  Copyright (c) 2001 Kyle Wheeler. All rights reserved.
//

#import "variables.h"
#import "VariableList.h"

@implementation NSTableDataSource

// needs to be REALLY fast
- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{
	extern int contents;
	printf("contents = %i\n",contents);
	return contents;
}

// needs to be fast
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn*) aTableColumn row:(int)rowIndex
{
	extern struct variable *them;
	struct variable *cursor = them;
	int i;

	for (i=1;i<rowIndex;++i) {
		cursor = cursor->next;
	}
	if ([[[aTableColumn headerCell] stringValue] isEqualToString:@"Value"]) {
		printf("Value = %s!\n",strchr(cursor->keyvalue,'=')+1);
		return [NSString stringWithUTF8String:(strchr(cursor->keyvalue,'=')+1)];
	} else {
		char *temp = strchr(cursor->keyvalue,'=');
		NSString *foo;
		*temp=0;
		printf("Variable! = %s\n",temp2);
		foo = [NSString stringWithUTF8String:cursor->keyvalue];
		*temp = '=';
		return foo;
	}
}

- (void)tableView:(NSTableView*)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn*)aTableColumn row:(int)rowIndex
{
	printf("setvalue\n");
}

@end
