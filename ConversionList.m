#import "conversion.h"
#import "ConversionList.h"

@implementation ConversionList
- (IBAction)newType:(id)sender
{
	if ([toList selectedRow]>-1)
		[toList deselectAll:sender];
	if ([fromList selectedRow]>-1)
		[fromList deselectAll:sender];
	[toList reloadData];
	[fromList reloadData];
}

- (int)numberOfRowsInTableView:(NSTableView *)atv;
{
	switch ([theType indexOfSelectedItem]) {
		case LENGTH_C: return MAX_LENGTH_UNIT+1;
		case AREA_C: return MAX_AREA_UNIT+1;
		case VOLUME_C: return MAX_VOLUME_UNIT+1;
		case MASS_C: return MAX_MASS_UNIT+1;
		case SPEED_C: return MAX_SPEED_UNIT+1;
		case POWER_C: return MAX_POWER_UNIT+1;
		case FORCE_C: return MAX_FORCE_UNIT+1;
		case ACCELERATION_C: return MAX_ACCELERATION_UNIT+1;
		case ASTRONOMICAL_C: return MAX_ASTRONOMICAL_UNIT+1;
		case TEMPERATURE_C: return MAX_TEMPERATURE_UNIT+1;
		default:
			return 0;
	}
}

- (id)tableView:(NSTableView *)atv objectValueForTableColumn:(NSTableColumn*)col row:(int)rowIndex
{
	int type = [theType indexOfSelectedItem];
	if (type != TEMPERATURE_C) {
		return [NSString stringWithCString:conversions[type][rowIndex].name];
	} else {
		switch (rowIndex) {
			case KELVIN: return @"Kelvin";
			case CELSIUS: return @"Celsius";
			case RANKINE: return @"Rankine";
			case FARENHEIT: return @"Farenheit";
			case REAUMUR: return @"RŽaumur";
			default:
				return @"UNKNOWN";
		}
	}
}
@end
