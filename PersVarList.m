#include <mpfr.h>
#include "string_manip.h"
#include "calculator.h"
#include "list.h"
#include "PersVarList.h"

struct pers_var {
	char *name;
	mpfr_t val;
	char *exp;
};

static List persVars = NULL;

@implementation PersVarList

- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return listLen(persVars);
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn*)col row:(int)rowIndex
{
	struct pers_var * cursor = peekListElement(persVars, rowIndex);
	
	if (! cursor) {
		return @"BAD ROW";
	}
	if ([[col identifier] isEqualToString:@"value"]) {
		if (cursor->exp)
			return [NSString stringWithUTF8String:cursor->exp];
		else
			return [NSString stringWithUTF8String:print_this_result(cursor->val)];
	} else if ([[col identifier] isEqualToString:@"name"]) {
		return [NSString stringWithUTF8String:(cursor->name)];
	} else {
		printf("col identifier: %s\n",[[col identifier] cString]);
		return @"BAD COLUMN";
	}
}

- (void)tableView:(NSTableView*)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn*)col row:(int)rowIndex
{
	struct pers_var *cursor = peekListElement(persVars, rowIndex);
	
	if ([[col identifier] isEqualToString:@"value"]) {
		char * input = strdup([anObject UTF8String]);
		if (justnumbers(input)) {
			mpfr_t la;
			
			mpfr_init_set(la, last_answer, GMP_RNDN);
			parseme(input);
			mpfr_set(cursor->val, last_answer, GMP_RNDN);
			if (cursor->exp) {
				free(cursor->exp);
				cursor->exp = NULL;
			}
			mpfr_set(last_answer, la, GMP_RNDN);
			mpfr_clear(la);
		} else {
			cursor->exp = input;
			mpfr_set_ui(cursor->val, 0, GMP_RNDN);
		}
	} else if ([[col identifier] isEqualToString:@"name"]) {
		free(cursor->name);
		cursor->name = strdup([anObject UTF8String]);
		strstrip(' ',cursor->name);
	}
}

struct pers_var *getpersvar(char *key)
{
    struct pers_var *cursor = NULL;
    ListIterator li = NULL;

    if (!persVars || key == NULL || *key == 0 || listLen(persVars) == 0) {
	return NULL;
    }

    li = getListIterator(persVars);
    while ((cursor = (struct pers_var *)nextListElement(li)) != NULL) {
	if (!strncmp(cursor->name, key, strlen(cursor->name)+1))
	    break;
	else
	    cursor = NULL;
    }
    freeListIterator(li);
    return cursor;
}

int putpersvar(char *name, mpfr_t value, char *exp)
{
    struct pers_var *cursor = NULL;

    if (name == NULL || *name == 0) {
	return -1;
    }
    cursor = getpersvar(name);
    if (!cursor) {
	cursor = calloc(1, sizeof(struct pers_var));
	addToList(&persVars, cursor);
    }
    if (!cursor->name) {
	cursor->name = strdup(name);
	mpfr_init_set_ui(cursor->val, 0, GMP_RNDN);
    } else if (cursor->exp) {
	free(cursor->exp);
	cursor->exp = NULL;
	mpfr_set_ui(cursor->val, 0, GMP_RNDN);
    }

    if (value) {
	mpfr_set(cursor->val, value, GMP_RNDN);
    } else {
	cursor->exp = strdup(exp);
    }
    return 0;
}

int persvarexists(char* key)
{
	struct pers_var *cursor = NULL;
	ListIterator li = NULL;

	if (!persVars || !strlen(key) || listLen(persVars) == 0) {
		return 0;
	}
	li = getListIterator(persVars);
	while ((cursor = (struct pers_var *)nextListElement(li)) != NULL) {
		if (!strncmp(cursor->name, key, strlen(cursor->name) + 1))
			break;
		else
			cursor = NULL;
	}
	freeListIterator(li);
	return cursor ? 1 : 0;
}

- (IBAction)addVar:(id)sender
{
	char varname[20];
	int i=1;
	mpfr_t blank;
	
	mpfr_init_set_ui(blank,0,GMP_RNDN);
	sprintf(varname,"NewVariable%i",i);
	while(persvarexists(varname)) {
		++i;
		sprintf(varname,"NewVariable%i",i);
	}
	putpersvar(varname,blank,NULL);
	[theList reloadData];
}

- (IBAction)clearVars:(id)sender
{
	printf("clearVars\n");
}

- (IBAction)delVar:(id)sender
{
	printf("delVar\n");
}

- (IBAction)showList:(id)sender
{
	[theWindow makeKeyAndOrderFront:self];
}

@end
