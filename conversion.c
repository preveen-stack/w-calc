/*
 *  conversion.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Wed Jul 31 2002.
 *  Copyright (c) 2002 __MyCompanyName__. All rights reserved.
 *
 */

#include "conversion.h"

/* The conversion table */
char *** conversions[MAX_TYPE+1];

double lengths[MAX_LENGTH_UNIT+1] = {
	10000000000,     // angstroms
	1000000000,      // nanometers
	39370078.740157, // microinches
	1000000,         // microns
	39370.07874,     // mils
	1000,            // millimeters
	100,             // centimeters
	39.370079,       // inches
	4.97097,         // links
	4.374453,        // spans
	3.28084,         // feet
	2.187227,        // cubits
	1.1811,          // varas
	1.093613,        // yards
	1.00,            // meters *********
	0.546807,        // fathoms
	0.198839,        // rods
	0.04971,         // chains
	0.004971,        // furlongs
	0.004557,        // cable lengths
	0.001,           // kilometers
	0.000621,        // miles
	0.000539957,     // nautical miles
	0.000207,        // leagues
	0.00018,         // nautical leagues
	2834.6457,       // points
	236.2205         // pica
};

double areas[MAX_AREA_UNIT+1] = {
	1.00,            // acre
	1.21,            // commercial acre
	0.617369,        // irish acre
	0.999996,        // survey acre
	40.468564,       // are
	200.020406,      // base box (tin plated steel)
	1224.0945,       // bin (Taiwan)
	1224.0945,       // bu (Japan)
	9.176545,        // cantero (Ecuador)
	11.241268,       // cao (Vietnam)
	4046.8564,       // centaire
	0.404686,        // hectare
	5.645726,        // labor (Canada)
	0.005645,        // labor (US)
	0.579036,        // manzana (Costa Rica)
	0.404686,        // manzana (Argentina)
	2.529285,        // rai (Thailand)
	3.999984,        // rood
	11.241268,       // sao (Vietnam)
	463.027064,      // scruple (ancient Rome)
	0.001562,        // section (US Survey)
	435.599981,      // square
	48.401584,       // square (Sri Lanka)
	62726399652.54841 // square caliber
	40468564,        // square centimeter
	11151360.073126, // square digit
	43559.999759,    // square international foot
	43559.825338,    // square US Survey foot
	6272639.96528,   // square international inch
	6272614.848634,  // square US Survey inch
	4046.8564,       // square meter
	0.004047,        // square kilometer
	4839.980766,     // square US Survey yard
	0.001562,        // square miles
	38,358.828494,   // square Paris foot (Canada)
	0.000043         // township
};

double volumes[MAX_VOLUME_UNIT+1] = {
	3785.411784,     // milliliters
	3785.411784,     // cubic centimeters (cc)
	3.785411784,     // cubic decimeter
	3.785411784,     // liter
	0.03785411784,   // hectoliter
	0.003785411784,  // cubic meters
	231,             // cubic inches
	127.999998,      // fluid ounce (US)
	133.227867,      // fluid ounce (UK)
	16,              // cup (US)
	8,               // pint (US)
	1,               // gallons (US)
	0.133681,        // cubic feet
	0.031746,        // barrel (US, liquid)
	1.604167,        // board foot
	0.107421,        // bushel (US)
	0.001044,        // cord (firewood)
	0.008355,        // cord foot (timber)
	5,               // fifth
	32,              // gill (US)
	0.015873,        // hogshead (US)
	85.333334,       // jigger
	0.491612,        // measure (ancient hebrew)
	0.429684,        // peck (US)
	0.007937,        // pipe (US)
	127.999998,      // pony
	4,               // quart (US)
	0.013011,        // quarter (UK)
	0.003785411784,  // stere
	3197.46888,      // scruple (UK)
	58350.801541,    // drop
	12284.379957,    // dash
	6142.189979,     // pinch
	3784.296607,     // cooking milliliter (cc)
	3071.09474,      // coffee spoon
	768,             // teaspoon (US)
	818.981651,      // teaspoon (UK)
	256,             // tablespoon (US)
	204.745415,      // tablespoon (UK)
	4,               // #2.5 can
	1                // #10 can
};

double masses[MAX_MASS_UNIT+1] = {
	7000,            // grains
	350,             // scruples (Apothecaries, aka. Druggists)
	2268,            // carats
	453.6,           // grams
	291.666666666667,// pennyweight
	256,             // dram (Avoirdupois, aka. Commerce)
	116.666666666667,// dram (Apoth.)
	16,              // ounces (Avoir.)
	14.5833333333334,// ounces (Apoth. & Troy)
	32.2,            // poundals
	1.21527777777777,// pounds (Troy (for gold))
	1,               // pounds (Avoir.)
	0.4536,          // kilograms
	0.07142857142857142,// stones
	0.04,            // quarter (US)
	0.03105590062111801,// slugs
	0.01,            // hundredweight (US)
	0.0005,          // short ton
	0.0004536,       // metric ton / tonne
	0.0004464285714285714// long ton
};

double speeds[MAX_SPEED_UNIT+1] = {
	30.48,                  // centimeters/second
	0.3048,                 // meters/second
	1,                      // feet/second
	60,                     // feet/minute
	0.68181818181818181819, // miles/hour
	1.0972853161278995,     // kilometers/hour
	0.0909090909090909091,  // furlongs/min
	0.5924837511331251,     // knots
	5.454644629902362,      // leagues/day
	0.0009191187537183524   // Mach (dry air, 273 kelvin)
};

double powers[MAX_POWER_UNIT+1] = {
	7460000000,       // ergs/sec
	746000,           // milliwatts (mW)
	746,              // watts (joules/sec)
	10.697948,        // kiloCalories/min
	0.178299,         // kiloCalories/sec
	2547.160889,      // BTU/hour
	550.221342,       // foot-lbs/sec
	1,                // horsepower
	0.746,            // kilowatts
	0.000746,         // megawatts
	0.000001          // gigawatts
};

double forces[MAX_FORCE_UNIT+1] = {
	100000,           // dyne
	101.971621,       // gram-force
	7.233011,         // poundal
	1,                // newton
	0.224809,         // pound
	0.101972,         // kilopond (kgm-force)
	0.000225          // kip
};

double accelerations[MAX_ACCELERATION_UNIT+1] = {
	3.28084,          // celo
	10000,            // centigal
	100,              // centimeter/square second
	1000,             // decigal
	3.28084,          // foot/square second
	0.101972,         // g-unit (G)
	100,              // gal
	100,              // galileo
	0.101972,         // gn
	0.101972,         // grav
	39.370079,        // inch/square second
	3.6,              // kilometer/hour/second
	0.001,            // kilometer/square second
	0.1,              // leo
	1,                // meter/square second
	134.216178,       // mile/hour/minute
	2.236936,         // mile/hour/second
	0.000621,         // mile/square second
	100000,           // millegal
	1000              // millimeter/square second
};

double astronomicals[MAX_ASTRONOMICAL_UNIT+1] = {
	149598073000,    // meters
	149598073,       // kilometers
	92955932.976418, // miles
	1,               // astronomical unit (AU)
	0.000016,        // light-year
	0.000005,        // parsec
};

