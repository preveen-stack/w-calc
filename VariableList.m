#import "variables.h"
#import "calculator.h"
#import "string_manip.h"
#import "VariableList.h"

@implementation VariableList
// needs to be REALLY fast
- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{
	extern int contents;
	return contents;
}

// needs to be fast
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn*)col row:(int)rowIndex
{
	struct variable *keyval = getrealnvar(rowIndex);

	if (! keyval)
		return @"BAD ROW";

	if ([[col identifier] isEqualToString:@"value"]) {
		if (keyval->exp)
			return [NSString stringWithCString:keyval->expression];
		else
			return [NSString localizedStringWithFormat:@"%g",(keyval->value)];
	} else if ([[col identifier] isEqualToString:@"variable"]) {
		return [NSString stringWithCString:(keyval->key)];
	} else {
		return @"BAD COLUMN";
	}
}

- (void)tableView:(NSTableView*)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn*)col row:(int)rowIndex
{
	struct variable *theval = getrealnvar(rowIndex);
	NSString *ch = [col identifier];
	
//	printf("Column %s, Row %i was %s,%s becomes %s\n", [ch cString], rowIndex, theval->key, theval->value, [anObject cString]);
	if ([ch isEqualToString:@"value"]) {
		char * input=strdup([anObject cString]);
		if (justnumbers(input)) {
			double la = last_answer;
			theval->value = parseme(input);
			theval->exp = 0;
			last_answer = la;
		} else {
			theval->expression = input;
			theval->exp = 1;
		}
	} else if ([ch isEqualToString:@"variable"]) {
		int i,j;
		free(theval->key);
		theval->key = strdup([anObject cString]);
		for (j=i=0;i<strlen(theval->key);++i) {
			if (theval->key[i] != ' ') {
				theval->key[j] = theval->key[i];
				++j;
			}
		}
		for (;j<i;++j) theval->key[j] = '\0';
	}
}

- (IBAction)newVariable:(id)sender
{
	char varname[20];
	int i=1;
	sprintf(varname,"NewVariable%i",i);
	while(! getvar_full(varname).err) {
		++i;
		sprintf(varname,"NewVariable%i",i);
	}
	putval(varname,0);
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
    extern int contents;
    int i, total=contents;

    for (i=0; i<total; ++i) {
	delnvar(0);
    }
    [theList reloadData];
}

@end
