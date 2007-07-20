//
//  WcalcService.m
//  Wcalc
//
//  Created by Kyle Wheeler on 7/19/07.
//  Copyright 2007 Kyle Wheeler. All rights reserved.
//

#import "WcalcService.h"
#include "calculator.h" /* for parseme() */
#include "variables.h" /* for putval() */
#include "historyManager.h" /* for addToHistory() */
#include "WcalcController.h" /* for access to the controller */

@implementation WcalcService

/* This service returns the value;
 * this is inspired in a large part by the SimpleService example Apple provides */
- (void)doWcalcCalculateService:(NSPasteboard *)pboard
		       userData:(NSString *)data
			  error:(NSString **)error
{
    NSString *pboardString;
    NSString *newString;
    NSArray *types;
    char * expression;
    extern char * errstring;
    
    types = [pboard types];
    
    if (![types containsObject:NSStringPboardType] || 
	!(pboardString = [pboard stringForType:NSStringPboardType])) {
	*error = NSLocalizedString(@"Error: Pasteboard doesn't contain a string.",
				   @"Pasteboard couldn't give string.");
	return;
    }
    
    expression = strdup([pboardString UTF8String]);
    parseme(expression);
    //putval("a",last_answer,"last answer from WcalcService");
    putval("WcalcServiceAnswer",last_answer,"last answer from WcalcService");
    
    /* if it isn't an error (or if you want me to remember errors) record it in the history */
    if (!errstring || (errstring && !strlen(errstring)) || conf.remember_errors) {
	addToHistory(expression, last_answer);
    }
    free(expression);

    if (errstring && strlen(errstring)) {
	extern int scanerror;
	scanerror = 0;
	*error = NSLocalizedString([NSString stringWithUTF8String:errstring],
				   @"Couldn't perform service operation.");
	free(errstring);
	errstring = NULL;
	return;
    }
    newString = [NSString stringWithFormat:@"%s",pretty_answer];
    
    if ([inspectorWindow isVisible]) { /* XXX: why does this not work??? */
	[variableList reloadData];
	[historyList reloadData];
    }
    
    /* We now return the capitalized string... */
    types = [NSArray arrayWithObject:NSStringPboardType];
    [pboard declareTypes:types owner:nil];
    [pboard setString:newString forType:NSStringPboardType];
    
    
    return;
}

@end
