//
//  main.m
//  Wcalc
//
//  Created by Kyle Wheeler on Thu Jan 17 2002.
//  Copyright (c) 2001 Kyle Wheeler. All rights reserved.
//

#include <Cocoa/Cocoa.h>

#include "calculator.h" /* needed to set standard_output = 0 */
#include "variables.h" /* needed to run variable initialization */
#include "WcalcService.h" /* needed to init the service */

int main(int argc, const char *argv[])
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    WcalcService* wserv = [[WcalcService alloc] init];
    int retval;
    
    initvar();
    standard_output = 0;
    
    NSRegisterServicesProvider(wserv,@"Wcalc");
    [NSApp setServicesProvider:wserv];
    
    retval = NSApplicationMain(argc, argv);
    
    [pool release];
    exit(0);
    return 0;
}
