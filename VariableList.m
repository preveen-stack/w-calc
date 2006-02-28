#include "variables.h"
#include "calculator.h"
#include "string_manip.h"
#include "VariableList.h"

#include "gmp.h"
#include "mpfr.h"

@implementation VariableList
// needs to be REALLY fast
- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return numvars();
}

// needs to be fast
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn*)col row:(int)rowIndex
{
	struct variable *keyval;
	static struct variable *keyval_cache = NULL;
	static int rowIndex_cache = -1;

	if (rowIndex == rowIndex_cache) {
		rowIndex_cache = -1;
		keyval = keyval_cache;
	} else {
		rowIndex_cache = rowIndex;
		keyval = keyval_cache = getrealnvar(rowIndex);
	}
	if (! keyval)
		return @"BAD ROW (VariableList)";

	if ([[col identifier] isEqualToString:@"value"]) {
		if (keyval->exp)
			return [NSString stringWithUTF8String:keyval->expression];
		else
			return [NSString stringWithUTF8String:print_this_result(keyval->value)];
	} else if ([[col identifier] isEqualToString:@"variable"]) {
		return [NSString stringWithUTF8String:(keyval->key)];
	} else {
		return @"BAD COLUMN";
	}
}

- (void)tableView:(NSTableView*)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn*)col row:(int)rowIndex
{
	struct variable *theval = getrealnvar(rowIndex);
	NSString *ch = [col identifier];

//	printf("Column %s, Row %i was %s,%s becomes %s\n", [ch UTF8String], rowIndex, theval->key, theval->value, [anObject UTF8String]);
	if ([ch isEqualToString:@"value"]) {
		char * input=strdup([anObject UTF8String]);
		if (justnumbers(input)) {
			mpfr_t la;

			mpfr_init_set(la, last_answer, GMP_RNDN);
			parseme(input);
			mpfr_set(theval->value, last_answer, GMP_RNDN);
			theval->exp = 0;
			mpfr_set(last_answer, la, GMP_RNDN);
			mpfr_clear(la);
		} else {
			theval->expression = input;
			theval->exp = 1;
		}
	} else if ([ch isEqualToString:@"variable"]) {
		free(theval->key);
		theval->key = strdup([anObject UTF8String]);
		strstrip(' ',theval->key);
	}
}

- (IBAction)newVariable:(id)sender
{
	char varname[20];
	int i=1;
	mpfr_t blank;

	mpfr_init_set_ui(blank,0,GMP_RNDN);
	sprintf(varname,"NewVariable%i",i);
	while(varexists(varname)) {
		++i;
		sprintf(varname,"NewVariable%i",i);
	}
	putval(varname,blank,NULL);
	[theList reloadData];
}

- (IBAction)delVariable:(id)sender
{
	if ([theList selectedRow] > -1) {
		delnvar([theList selectedRow]);
		[theList reloadData];
	}
}

- (IBAction)clearVariables:(id)sender
{
    int i, total=numvars();

    for (i=0; i<total; ++i) {
	delnvar(0);
    }
    [theList reloadData];
}

@end
