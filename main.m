//
//  main.m
//  Wcalc
//
//  Created by Kyle Wheeler on Thu Jan 17 2002.
//  Copyright (c) 2001 Kyle Wheeler. All rights reserved.
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
    return NSApplicationMain(argc, argv);
}
