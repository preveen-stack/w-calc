#import "InspectorController.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif

@implementation InspectorController

- (IBAction)closeIt:(id)sender
{
    [inspectorWindow close];
}

- (IBAction)openIt:(id)sender
{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    [prefs setObject:@"YES" forKey:@"historyShowing"];
    [affectInspectorMenu setTitle:@"Hide Inspector"];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onClose:) name:NSWindowWillCloseNotification object:inspectorWindow];
    [variableList reloadData];
    [historyList reloadData];
    [inspectorWindow setBecomesKeyOnlyIfNeeded:YES];
    [inspectorWindow orderFront:self];
}

- (IBAction)toggleIt:(id)sender
{
    if ([inspectorWindow isVisible]) {
	[self closeIt:sender];
    } else {
	[self openIt:sender];
    }
}

- (IBAction)onClose:(id)sender
{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];

    [[NSNotificationCenter defaultCenter] removeObserver:self name:NSWindowWillCloseNotification object:inspectorWindow];
    [prefs setObject:@"NO" forKey:@"historyShowing"];
    [affectInspectorMenu setTitle:@"Show Inspector"];
}

@end
