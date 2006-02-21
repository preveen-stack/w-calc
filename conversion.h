/*
 *  conversion.h
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Wed Jul 31 2002.
 *  Copyright (c) 2002 Kyle Wheeler. All rights reserved.
 *
 */
#ifndef WCALC_CONVERSIONS
#define WCALC_CONVERSIONS

#include <gmp.h>
#include <mpfr.h>

void uber_conversion(mpfr_t output, int utype, int fromunit, int tounit,
		     mpfr_t value);
int identify_units(char *unit1, char *unit2);
int unit_id(int utype, char *unit);

struct conversion
{
    char *factor;
    char *name;
    char *aka[9];
};

struct conv_req
{
    char *u1;
    char *u2;
};

/* Conversion Types */
#define MAX_TYPE        8
#define LENGTH_C        0 /*-*/
#define AREA_C          1 /*-*/
#define VOLUME_C        2 /*-*/
#define MASS_C          3 /*-*/
#define SPEED_C         4 /*-*/
#define POWER_C         5 /*-*/
#define FORCE_C         6 /*-*/
#define ACCELERATION_C  7 /*-*/
#define TEMPERATURE_C   8 /**/
/* Astronomical Units */
#define MAX_ASTRONOMICAL_UNIT 5
#define A_METERS          0
#define A_KILOMETERS      1
#define A_MILES           2
#define LIGHT_YEAR        4
#define PARSEC            5
/* Length Units */
#define LENGTH_UNIT_COUNT		30
/* Area Units */
#define AREA_UNIT_COUNT			37
/* Volume Units */
#define VOLUME_UNIT_COUNT		41
/* Mass Units */
#define MASS_UNIT_COUNT			21
/* Speed Units */
#define SPEED_UNIT_COUNT		10
/* Power Units */
#define POWER_UNIT_COUNT		11
/* Force Units */
#define FORCE_UNIT_COUNT		7
/* Acceleration Units */
#define ACCELERATION_UNIT_COUNT	18
/* Temperature Units */
#define TEMPERATURE_UNIT_COUNT	5
#define KELVIN		0
#define CELSIUS		1
#define RANKINE		2
#define FARENHEIT	3
#define REAUMUR		4
extern const struct conversion *conversions[MAX_TYPE + 1];

#endif
