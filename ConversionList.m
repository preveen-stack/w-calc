#include "conversion.h"
#include "ConversionList.h"

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

- (int)numberOfRowsInTableView:(NSTableView *)atv
{
	switch ([theType indexOfSelectedItem]) {
		case LENGTH_C: return LENGTH_UNIT_COUNT;
		case AREA_C: return AREA_UNIT_COUNT;
		case VOLUME_C: return VOLUME_UNIT_COUNT;
		case MASS_C: return MASS_UNIT_COUNT;
		case SPEED_C: return SPEED_UNIT_COUNT;
		case POWER_C: return POWER_UNIT_COUNT;
		case FORCE_C: return FORCE_UNIT_COUNT;
		case ACCELERATION_C: return ACCELERATION_UNIT_COUNT;
		case TEMPERATURE_C: return TEMPERATURE_UNIT_COUNT;
		default:
			return 0;
	}
}

- (id)tableView:(NSTableView *)atv objectValueForTableColumn:(NSTableColumn*)col row:(int)rowIndex
{
	int type = [theType indexOfSelectedItem];
	static NSString * reamur = NULL;
	if (! reamur) {
		reamur = [NSString stringWithFormat:@"R%Caumur", 0x00e9];
		[reamur retain];
	}
	if (type != TEMPERATURE_C) {
		return [NSString stringWithUTF8String:conversions[type][rowIndex].name];
	} else {
		switch (rowIndex) {
			case KELVIN: return @"Kelvin";
			case CELSIUS: return @"Celsius";
			case RANKINE: return @"Rankine";
			case FARENHEIT: return @"Farenheit";
			case REAUMUR: return reamur;
			default:
				return @"UNKNOWN";
		}
	}
}
@end
