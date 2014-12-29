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

#include <sys/types.h> /* for ssize_t */
#include <unistd.h>    /* also for ssize_t */

#include "number.h"

void uber_conversion(Number    output,
                     const int utype,
                     const int fromunit,
                     const int tounit,
                     Number    value);
ssize_t identify_unit(const char *unit);
ssize_t identify_units(const char *unit1,
                       const char *unit2);
ssize_t unit_id(const int   utype,
                const char *unit);

struct conversion {
    char *factor;
    char *name;
    char *aka[9];
};

struct conv_req {
    char *u1;
    char *u2;
};

/* Conversion Types */
#define MAX_TYPE       11
#define LENGTH_C       0  /*-*/
#define AREA_C         1  /*-*/
#define VOLUME_C       2  /*-*/
#define MASS_C         3  /*-*/
#define SPEED_C        4  /*-*/
#define POWER_C        5  /*-*/
#define FORCE_C        6  /*-*/
#define ACCELERATION_C 7  /*-*/
#define TEMPERATURE_C  8  /*-*/
#define ANGLE_C        9  /*-*/
#define PRESSURE_C     10 /*-*/
#define METRIC_C       11 /**/
/* Astronomical Units */
#define MAX_ASTRONOMICAL_UNIT 5
#define A_METERS              0
#define A_KILOMETERS          1
#define A_MILES               2
#define LIGHT_YEAR            4
#define PARSEC                5
/* Length Units */
#define LENGTH_UNIT_COUNT 87
/* Area Units */
#define AREA_UNIT_COUNT 74
/* Volume Units */
#define VOLUME_UNIT_COUNT 136
/* Angle Units */
#define ANGLE_UNIT_COUNT 12
/* Mass Units */
#define MASS_UNIT_COUNT 66
/* Speed Units */
#define SPEED_UNIT_COUNT 17
/* Power Units */
#define POWER_UNIT_COUNT 45
/* Force Units */
#define FORCE_UNIT_COUNT 13
/* Acceleration Units */
#define ACCELERATION_UNIT_COUNT 23
/* Pressure Units */
#define PRESSURE_UNIT_COUNT 41
/* Temperature Units */
#define TEMPERATURE_UNIT_COUNT 5
#define KELVIN                 0
#define CELSIUS                1
#define RANKINE                2
#define FARENHEIT              3
#define REAUMUR                4
extern const struct conversion *conversions[MAX_TYPE + 2];
extern const char              *conversion_names[MAX_TYPE + 2];
#endif // ifndef WCALC_CONVERSIONS
/* vim:set expandtab: */
