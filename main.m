//
//  main.m
//  Wcalc
//
//  Created by Kyle Wheeler on Thu Jan 17 2002.
//  Copyright (c) 2001 Kyle Wheeler. All rights reserved.
//

#include <Cocoa/Cocoa.h>

#include "calculator.h"
#include "variables.h"

#ifdef MEMWATCH
#include "memwatch.h"
#endif

int main(int argc, const char *argv[])
{
	initvar();
	standard_output = 0;
    return NSApplicationMain(argc, argv);
}
