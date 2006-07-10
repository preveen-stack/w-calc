#include "string_manip.h"
#include "calculator.h"
#include "list.h"
#include "PersVarList.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif

struct pers_var {
    char *name;
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
	    return [NSString stringWithUTF8String:cursor->exp];
    } else if ([[col identifier] isEqualToString:@"name"]) {
	   return [NSString stringWithUTF8String:(cursor->name)];
    } else {
	return @"BAD COLUMN";
    }
}

- (void)tableView:(NSTableView*)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn*)col row:(int)rowIndex
{
    struct pers_var *cursor = peekListElement(persVars, rowIndex);
    NSMutableDictionary *temp = [NSMutableDictionary dictionaryWithCapacity:listLen(persVars)];
    ListIterator li = NULL;
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];

    if ([[col identifier] isEqualToString:@"value"]) {
	free(cursor->exp);
	cursor->exp = strdup([anObject UTF8String]);
    } else if ([[col identifier] isEqualToString:@"name"]) {
	free(cursor->name);
	cursor->name = strdup([anObject UTF8String]);
	strstrip(' ',cursor->name);
    }
    li = getListIterator(persVars);
    while ((cursor = (struct pers_var *)nextListElement(li)) != NULL) {
	[temp setObject:[NSString stringWithUTF8String:cursor->exp]
		 forKey:[NSString stringWithUTF8String:cursor->name]];
    }
    [prefs setObject:temp forKey:@"persistentVariables"];
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

int putpersvar(char *name, char *exp)
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
    } else if (cursor->exp) {
	free(cursor->exp);
    }
    cursor->exp = strdup(exp);
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

    sprintf(varname,"NewVariable%i",i);
    while(persvarexists(varname)) {
	++i;
	sprintf(varname,"NewVariable%i",i);
    }
    putpersvar(varname,"0");
    [theList reloadData];
}

- (IBAction)clearVars:(id)sender
{
	while (listLen(persVars) > 0) {
		getListElement(persVars,0);
	}
	[theList reloadData];
}

- (IBAction)delVar:(id)sender
{
	if ([theList selectedRow] > -1) {
		getListElement(persVars,[theList selectedRow]);
		[theList reloadData];
	}
}

- (IBAction)showList:(id)sender
{
    if (persVars == NULL) {
	NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
	NSDictionary *temp = [prefs dictionaryForKey:@"persistentVariables"];
	NSEnumerator *enumerator = [temp keyEnumerator];
	id key;

	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onClose:) name:NSWindowWillCloseNotification object:theWindow];
	while ((key = [enumerator nextObject])) {
	    putpersvar(strdup([key UTF8String]),strdup([[temp objectForKey:key] UTF8String]));
	}
    }
    [theWindow makeKeyAndOrderFront:self];
}

- (IBAction)onClose:(id)sender
{
    struct pers_var *cursor = NULL;
    NSMutableDictionary *temp = [NSMutableDictionary dictionaryWithCapacity:listLen(persVars)];
    ListIterator li = NULL;
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self name:NSWindowWillCloseNotification object:theWindow];
    li = getListIterator(persVars);
    while ((cursor = (struct pers_var *)nextListElement(li)) != NULL) {
	[temp setObject:[NSString stringWithUTF8String:cursor->exp]
		 forKey:[NSString stringWithUTF8String:cursor->name]];
    }
    [prefs setObject:temp forKey:@"persistentVariables"];
    freeListIterator(li);
}

@end
