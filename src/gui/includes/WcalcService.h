//
//  WcalcService.h
//  Wcalc
//
//  Created by Kyle Wheeler on 7/19/07.
//  Copyright 2007 Kyle Wheeler. All rights reserved.
//

#import <AppKit/AppKit.h>


@interface WcalcService : NSObject
{
    IBOutlet NSWindow *inspectorWindow;
    IBOutlet NSTableView *variableList;
    IBOutlet NSTableView *historyList;
}
@end
