/*
 *  conversion.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Wed Jul 31 2002.
 *  Copyright (c) 2002 Kyle Wheeler. All rights reserved.
 *
 */

#include "config.h"
#include <stdio.h>
#include "calculator.h"
#include "conversion.h"

/* The conversion tables */

/* based on Meters */
const struct conversion lengths[MAX_LENGTH_UNIT+1] = {
	{10000000000,     "Angstroms"},
	{0.004557,        "Cable Lengths"},
	{100,             "Centimeters"},
	{0.04971,         "Chains"},
	{2.187227,        "Cubits"},
	{0.546807,        "Fathoms"},
	{3.28084,         "Feet"},
	{0.004971,        "Furlongs"},
	{39.370079,       "Inches"},
	{0.001,           "Kilometers"},
	{0.000207,        "Leagues"},
	{0.00018,         "Nautical Leagues"},
	{4.97097,         "Links"},
	{1.00,            "Meters"},
	{39370078.740157, "Microinches"},
	{1000000,         "Microns"},
	{0.000621,        "Miles"},
	{0.000539957,     "Nautical Miles"},
	{1000,            "Millimeters"},
	{39370.07874,     "Mils"},
	{1000000000,      "Nanometers"},
	{236.2205,        "Pica"},
	{2834.6457,       "Points"},
	{0.198839,        "Rods"},
	{4.374453,        "Spans"},
	{1.1811,          "Varas"},
	{1.093613,        "Yards"},
};

/* based on Acres */
const struct conversion areas[MAX_AREA_UNIT+1] = {
	{1.00,              "Acre"},
	{1.21,              "Commercial Acre"},
	{0.617369,          "Irish Acre"},
	{0.999996,          "Survey Acre"},
	{40.468564,         "Are"},
	{200.020406,        "Base boxe (tin plated steel)"},
	{1224.0945,         "Bin (Taiwan)"},
	{1224.0945,         "Bu (Japan)"},
	{9.176545,          "Cantero (Ecuador)"},
	{11.241268,         "Cao (Vietnam)"},
	{4046.8564,         "Centaire"},
	{0.404686,          "Hectare"},
	{5.645726,          "Labor (Canada)"},
	{0.005645,          "Labor (US)"},
	{0.579036,          "Manzana (Costa Rica)"},
	{0.404686,          "Manzana (Argentina)"},
	{2.529285,          "Rai (Thailand)"},
	{3.999984,          "Rood"},
	{11.241268,         "Sao (Vietnam)"},
	{463.027064,        "Scruple (ancient Rome)"},
	{0.001562,          "Section (US Survey)"},
	{435.599981,        "Square"},
	{48.401584,         "Square (Sri Lanka)"},
	{62726399652.54841, "Square Caliber"},
	{40468564,          "Square Centimeter"},
	{11151360.073126,   "Square Digit"},
	{43559.999759,      "Square International Foot"},
	{43559.825338,      "Square US Survey Foot"},
	{6272639.96528,     "Square International Inch"},
	{6272614.848634,    "Square US Survey Inch"},
	{4046.8564,         "Square Meter"},
	{0.004047,          "Square Kilometer"},
	{4839.980766,       "Square US Survey Yard"},
	{0.001562,          "Square Miles"},
	{38358.828494,      "Square Paris Foot (Canada)"},
	{0.000043,          "Township"}
};

/* based on US Gallons */
const struct conversion volumes[MAX_VOLUME_UNIT+1] = {
	{4,              "#2.5 can"},
	{1,              "#10 can"},
	{0.031746,       "Barrel (US, liquid)"},
	{1.604167,       "Board Foot"},
	{0.107421,       "Bushel (US)"},
	{3071.09474,     "Coffee spoon"},
	{3784.296607,    "Cooking milliliter (cc)"},
	{0.001044,       "Cord (firewood)"},
	{0.008355,       "Cord Foot (timber)"},
	{3785.411784,    "Cubic Centimeter (cc)"},
	{3.785411784,    "Cubic Decimeter"},
	{0.133681,       "Cubic Foot"},
	{231,            "Cubic Inch"},
	{0.003785411784, "Cubic Meter"},
	{16,             "Cup (US)"},
	{12284.379957,   "Dash"},
	{58350.801541,   "Drop"},
	{5,              "Fifth"},
	{127.999998,     "Fluid Ounce (US)"},
	{133.227867,     "Fluid Ounce (UK)"},
	{1,              "Gallon (US)"},
	{32,             "Gill (US)"},
	{0.03785411784,  "Hectoliter"},
	{0.015873,       "Hogshead (US)"},
	{85.333334,      "Jigger"},
	{3.785411784,    "Liter"},
	{0.491612,       "Measure (Ancient Hebrew)"},
	{3785.411784,    "Milliliter"},
	{8,              "Pint (US)"},
	{0.429684,       "Peck (US)"},
	{0.007937,       "Pipe (US)"},
	{127.999998,     "Pony"},
	{4,              "Quart (US)"},
	{0.013011,       "Quarter (UK)"},
	{0.003785411784, "Stere"},
	{3197.46888,     "Scruple (UK)"},
	{6142.189979,    "Pinch"},
	{256,            "Tablespoon (US)"},
	{204.745415,     "Tablespoon (UK)"},
	{768,            "Teaspoon (US)"},
	{818.981651,     "Teaspoon (UK)"},
};

/* based on the Avoirdupois Pound */
const struct conversion masses[MAX_MASS_UNIT+1] = {
	{2268,                 "Carat"},
	{256,                  "Dram (Avoirdupois)"},
	{116.666666666667,     "Dram (Apothecaries)"},
	{7000,                 "Grain"},
	{453.6,                "Gram"},
	{0.01,                 "Hundredweight (US)"},
	{0.4536,               "Kilogram"},
	{16,                   "Ounce (Avoir.)"},
	{14.5833333333334,     "Ounce (Apoth. & Troy)"},
	{291.666666666667,     "Pennyweight"},
	{1.21527777777777,     "Pound (Troy)"},
	{1,                    "Pound (Avoir.)"},
	{32.2,                 "Poundal"},
	{0.04,                 "Quarter (US)"},
	{350,                  "Scruple (Apoth.)"},
	{0.03105590062111801,  "Slug"},
	{0.07142857142857142,  "Stone"},
	{0.0005,               "Short Ton"},
	{0.0004536,            "Metric Ton / Tonne"},
	{0.0004464285714285714,"Long Ton"}
};

/* based on feet per second */
const struct conversion speeds[MAX_SPEED_UNIT+1] = {
	{30.48,                  "Centimeters/second"},
	{0.3048,                 "Meters/second"},
	{1,                      "Feet/second"},
	{60,                     "Feet/minute"},
	{0.0909090909090909091,  "Furlongs/min"},
	{1.0972853161278995,     "Kilometers/hour"},
	{0.5924837511331251,     "Knots"},
	{5.454644629902362,      "Leagues/day"},
	{0.0009191187537183524,  "Mach (dry air, 273 kelvin)"},
	{0.68181818181818181819, "Miles/hour"},
};

/* based on horsepower */
const struct conversion powers[MAX_POWER_UNIT+1] = {
	{2547.160889, "BTU/hour"},
	{7460000000,  "Ergs/sec"},
	{550.221342,  "Foot-lbs/sec"},
	{1,           "Horsepower"},
	{10.697948,   "KiloCalories/min"},
	{0.178299,    "KiloCalories/sec"},
	{746000,      "Milliwatts (mW)"},
	{746,         "Watts (joules/sec)"},
	{0.746,       "Kilowatts"},
	{0.000746,    "Megawatts"},
	{0.000000746, "Gigawatts"}
};

/* based on Newtons */
const struct conversion forces[MAX_FORCE_UNIT+1] = {
	{100000,     "Dyne"},
	{101.971621, "Gram-force"},
	{0.101972,   "Kilopond (kgm-force)"},
	{0.000225,   "Kip"},
	{1,          "Newton"},
	{0.224809,   "Pound"},
	{7.233011,   "Poundal"},
};

/* based on meters per square second */
const struct conversion accelerations[MAX_ACCELERATION_UNIT+1] = {
	{3.28084,    "Celo"},
	{10000,      "Centigal"},
	{100,        "Centimeter/square second"},
	{1000,       "Decigal"},
	{3.28084,    "Foot/square second"},
	{0.101972,   "G-unit (G)"},
	{100,        "Gal"},
	{100,        "Galileo"},
	{0.101972,   "Gn"},
	{0.101972,   "Grav"},
	{39.370079,  "Inch/square second"},
	{3.6,        "Kilometer/hour/second"},
	{0.001,      "Kilometer/square second"},
	{0.1,        "Leo"},
	{1,          "Meter/square second"},
	{134.216178, "Mile/hour/minute"},
	{2.236936,   "Mile/hour/second"},
	{0.000621,   "Mile/square second"},
	{100000,     "Millegal"},
	{1000,       "Millimeter/square second"}
};

/* based on Astronomical Units */
const struct conversion astronomicals[MAX_ASTRONOMICAL_UNIT+1] = {
	{1,               "Astronomical Unit (AU)"},
	{149598073,       "Kilometers"},
	{0.000016,        "Light-year"},
	{149598073000,    "Meters"},
	{92955932.976418, "Miles"},
	{0.000005,        "Parsec"}
};


char * from_temperatures[MAX_TEMPERATURE_UNIT+1] = {
	"[%1.15f]",                     // kelvin
	"[%1.15f + 273.15]",            // celsius
	"[%1.15f / 1.8]",               // rankine
	"[(%1.15f - 32)/1.8 + 273.15]", // farenheit
	"[((5/4) * %1.15f) + 273.15]"   // reaumur
};

char * to_temperatures[MAX_TEMPERATURE_UNIT+1] = {
	"%s",                 // kelvin
	"%s - 273.15",        // celsius
	"%s * 1.8",           // rankine
	"1.8*(%s-273.15)+32", // farenheit
	"(%s-273.15)*(4/5)"   // reaumur
};

const struct conversion * conversions[MAX_TYPE+1] = {
	lengths,
	areas,
	volumes,
	masses,
	speeds,
	powers,
	forces,
	accelerations,
	astronomicals,
};

double uber_conversion (int utype, int fromunit, int tounit, double value)
{
	if (utype != TEMPERATURE_C) {
		const struct conversion *ltable = conversions[utype];
		return (ltable[tounit].factor/ltable[fromunit].factor)*value;
	} else {
		char stage1[100];
		char composite[100];
		sprintf(stage1,from_temperatures[fromunit],value);
		sprintf(composite,to_temperatures[tounit],stage1);
		return parseme(composite);
	}
}



