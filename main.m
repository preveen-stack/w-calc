//
//  main.m
//  Wcalc
//
//  Created by Kyle Wheeler on Thu Jan 17 2002.
//  Copyright (c) 2001 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "calculator.h"
#import "variables.h"
#import "parser.h"

#import "WcalcController.h"

int main(int argc, const char *argv[])
{
	initvar();
	standard_output = 0;
	engineering = 0;
	precision = -1;
	[WcalcController initialize];
    return NSApplicationMain(argc, argv);
}
