/*
 *  conversion.h
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Wed Jul 31 2002.
 *  Copyright (c) 2002 __MyCompanyName__. All rights reserved.
 *
 */

double uber_conversion (int utype, int fromunit, int tounit, double value);

/* Conversion Types */
#define MAX_TYPE        9
#define LENGTH_C        0 /*-*/
#define AREA_C          1 /*-*/
#define VOLUME_C        2 /*-*/
#define MASS_C          3 /*-*/
#define SPEED_C         4 /*-*/
#define POWER_C         5 /*-*/
#define FORCE_C         6 /*-*/
#define ACCELERATION_C  7 /*-*/
#define TEMPERATURE_C   8 /**/
#define ASTRONOMICAL_C  9 /*-*/

/* Length Units */
#define MAX_LENGTH_UNIT 26
#define ANGSTROMS      0
#define NANOMETERS     1
#define MICROINCHES    2
#define MICRONS        3
#define MILS           4
#define MILLIMETERS    5
#define CENTIMETERS    6
#define INCHES         7
#define LINKS          8
#define SPANS          9
#define FEET          10
#define CUBITS        11
#define VARAS         12
#define YARDS         13
#define METERS        14
#define FATHOMS       15
#define RODS          16
#define CHAINS        17
#define FURLONGS      18
#define CABLE_LENGTHS 19
#define KILOMETERS    20
#define MILES         21
#define NAUT_MILES    22
#define LEAGUES       23
#define NAUT_LEAGUES  24
#define POINTS        25
#define PICAS         26

/* Area Units */
#define MAX_AREA_UNIT 35
#define ACRE          0
#define COMM_ACRE     1
#define IRISH_ACRE    2
#define SURVEY_ACRE   3
#define ARE           4
#define BASE_BOX      5
#define BIN           6
#define BU            7
#define CANTERO       8
#define CAO           9
#define CENTAIRE     10
#define HECTARE      11
#define CAN_LABOR    12
#define US_LABOR     13
#define CR_MANZANA   14
#define AR_MANZANA   15
#define RAI          16
#define ROOD         17
#define SAO          18
#define SCRUPLE_A    19
#define SECTION      20
#define SQUARE       21
#define SL_SQUARE    22
#define SQ_CALIBER   23
#define SQ_CM        24
#define SQ_DIGIT     25
#define SQ_I_FOOT    26
#define SQ_US_FOOT   27
#define SQ_I_INCH    28
#define SQ_US_INCH   29
#define SQ_METER     30
#define SQ_KM        31
#define SQ_YARD      32
#define SQ_US_MILES  33
#define SQ_P_FOOT    34
#define TOWNSHIP     35

/* Volume Units */
#define MAX_VOLUME_UNIT 40
#define ML_LITERS      0
#define C_CM           1
#define C_DM           2
#define LITERS         3
#define H_LITERS       4
#define C_METERS       5
#define C_INCHES       6
#define A_FL_OZ        7
#define I_FL_OZ        8
#define CUPS           9
#define PINTS         10
#define GALLONS       11
#define C_FEET        12
#define BARREL        13
#define BOARD_FOOT    14
#define BUSHEL        15
#define CORD          16
#define CORD_FOOT     17
#define FIFTH         18
#define GILL          19
#define HOGSHEAD      20
#define JIGGER        21
#define MEASURE       22
#define PECK          23
#define PIPE          24
#define PONY          25
#define QUART         26
#define QUARTER_V     27
#define STERE         28
#define SCRUPLE_V     29
#define DROP          30
#define DASH          31
#define PINCH         32
#define CC            33
#define C_SPOON       34
#define TEASPOON_US   35
#define TEASPOON_UK   36
#define TABLESPOON_US 37
#define TABLESPOON_UK 38
#define 2_5_CAN       39
#define 10_CAN        40

/* Mass Units */
#define MAX_MASS_UNIT 19
#define GRAINS       0
#define SCRUPLES_M   1
#define CARATS       2
#define GRAMS        3
#define PENNYWEIGHT  4
#define DRAM_AV      5
#define DRAM_AP      6
#define OUNCES_AV    7
#define OUNCES_TAP   8
#define POUNDALS_M   9
#define POUNDS_M_T  10
#define POUNDS_M_AV 11
#define KILOGRAMS   12
#define STONES      13
#define QUARTER_M   14
#define SLUGS       15
#define H_WEIGHTS   16
#define S_TONS      17
#define TONNES      18
#define L_TONS      19

/* Speed Units */
#define MAX_SPEED_UNIT 9
#define CM_P_SEC 0
#define M_P_SEC  1
#define FT_P_SEC 2
#define FT_P_MIN 3
#define MPH      4
#define KPH      5
#define F_P_MIN  6
#define KNOTS    7
#define L_P_DAY  8
#define MACH     9

/* Power Units */
#define MAX_POWER_UNIT    10
#define ERGS_P_SEC         0
#define MIL_WATS           1
#define WATTS              2
#define KILOCALORIES_P_MIN 3
#define KILOCALORIES_P_SEC 4
#define BTU_P_HOUR         5
#define FOOT_LBS_P_SEC     6
#define HORSEPOWER         7
#define KILOWATTS          8
#define MEGAWATTS          9
#define GIGAWATTS         10

/* Force Units */
#define MAX_FORCE_UNIT 6
#define DYNE       0
#define GRAM_FORCE 1
#define POUNDALS_F 2
#define NEWTONS    3
#define POUNDS_F   4
#define KILOPOND   5
#define KIP        6

/* Acceleration Units */
#define MAX_ACCELERATION_UNIT 19
#define CELO           0
#define CENTIGAL       1
#define CM_P_SQ_SEC    2
#define DECIGAL        3
#define FOOT_P_SQ_SEC  4
#define G_UNIT         5
#define GAL            6
#define GALILEO        7
#define GN             8
#define GRAV           9
#define IN_P_SQ_SEC   10
#define KM_P_H_SEC    11
#define KM_P_SQ_SEC   12
#define LEO           13
#define M_P_SQ_SEC    14
#define MILE_P_H_MIN  15
#define MILE_P_H_SEC  16
#define MILE_P_SQ_SEC 17
#define MILLIGAL      18
#define MM_P_SQ_SEC   19

/* Temperature Units */
#define MAX_TEMPERATURE_UNIT 4
#define KELVIN      0
#define CELSIUS     1
#define RANKINE     2
#define FARENHEIT   3
#define REAMUR      4

/* Astronomical Units */
#define MAX_ASTRONOMICAL_UNIT 5
#define A_METERS          0
#define A_KILOMETERS      1
#define A_MILES           2
#define ASTRONOMICAL_UNIT 3
#define LIGHT_YEAR        4
#define PARSEC            5
