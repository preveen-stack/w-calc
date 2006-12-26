/*  vim:expandtab
 *  conversion.c
 *  Wcalc
 *
 *  Created by Kyle Wheeler on Wed Jul 31 2002.
 *  Copyright (c) 2002 Kyle Wheeler. All rights reserved.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <string.h> /* for strcmp() */
#include "calculator.h"
#include "conversion.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif

/* The conversion tables */

/* based on Meters */
const struct conversion lengths[] = {
    {"10000000000",     "Angstroms",            {"Å","angstrom","angstroms",NULL}},
    {"6.68457808276714901267478224803e-12",
                        "Astronomical Units",   {"AU","ua","astronomical_unit","astronomical_units",NULL}},
    {"0.004557",        "Cable Lengths",        {"cable_length","cable_lengths",NULL}},
    {"100",             "Centimeters",          {"cm","centimeter","centimeters",NULL}},
    {"0.04971",         "Chains",               {"ch","chain","chains",NULL}},
    {"2.187227",        "Cubits",               {"cubit","cubits",NULL}},
    {"0.546807",        "Fathoms",              {"fth","fath","fathom","fathoms",NULL}},
    {"3.28083991666666","Feet",                 {"ft","feet","foot","'",NULL}},
    {"0.004971",        "Furlongs",             {"fur","furlong","furlongs",NULL}},
    {"39.3700789999999","Inches",               {"in","inch","inches","\"",NULL}},
    {"0.001",           "Kilometers",           {"km","kilometer","kilometers",NULL}},
    {"0.000207",        "Leagues",              {"league","leagues",NULL}},
    {"0.00018",         "Nautical Leagues",     {"nautical_league","nautical_leagues",NULL}},
    {"1.05702289450170391750030273856e-16",
                        "Light-years",          {"ly","lightyear","lightyears",NULL}},
    {"4.97097",         "Links",                {"link","links",NULL}},
    {"1.00",            "Meters",               {"m","meter","meters",NULL}},
    {"39370078.740157", "Microinches",          {"mci","microinch","microinches",NULL}},
    {"1000000",         "Microns",              {"µ","micron","microns",NULL}},
    {"0.000621",        "Miles",                {"mi","mile","miles",NULL}},
    {"0.000539957",     "Nautical Miles",       {"nmi","n_mile","n_miles","naut_mi","NM","n.m.","nautical_mile","nautical_miles",NULL}},
    {"1000",            "Millimeters",          {"mm","millimeter","millimeters",NULL}},
    {"39370.07874",     "Mils",                 {"mil","mils",NULL}},
    {"1000000000",      "Nanometers",           {"nm","nanometer","nanometers",NULL}},
    {"3.24077648680546210191415652999e-17",
                        "Parsec",               {"pc","psc","parsec","parsecs",NULL}},
    {"236.2205",        "Pica",                 {"pi","pica","picas",NULL}},
    {"2834.6457",       "Points",               {"pt","point","points",NULL}},
    {"0.198839",        "Rods",                 {"rd","rod","rods",NULL}},
    {"4.374453",        "Spans",                {"span","spans",NULL}},
    {"1.1811",          "Varas",                {"vara","varas",NULL}},
    {"1.093613",        "Yards",                {"yd","yard","yards",NULL}},
    {NULL,NULL,{NULL}}
};

/* based on Acres */
const struct conversion areas[] = {
    {"1.00",              "Acre",                           {"ac","A","acre","acres",NULL}},
    {"1.21",              "Commercial Acre",                {"commercial_acre","commercial_acres",NULL}},
    {"0.617369",          "Irish Acre",                     {"irish_acre","irish_acres",NULL}},
    {"0.999996",          "Survey Acre",                    {"survey_acre","survey_acres",NULL}},
    {"40.468564",         "Are",                            {"a","are","ares",NULL}},
    {"200.020406",        "Base boxe (tin plated steel)",   {"base_boxe",NULL}},
    {"1224.0945",         "Bin (Taiwan)",                   {"bin","bins",NULL}},
    {"1224.0945",         "Bu (Japan)",                     {"bu.JP",NULL}},
    {"9.176545",          "Cantero (Ecuador)",              {"cantero",NULL}},
    {"11.241268",         "Cao (Vietnam)",                  {"cao",NULL}},
    {"0.617369",          "Colpa (Ireland)",                {"colpas","colps","collops",NULL}},
    {"4046.8564",         "Centaire",                       {"ca","centiare","cetaires",NULL}},
    {"0.404686",          "Hectare",                        {"ha","hectare","hectares",NULL}},
    {"5.645726",          "Labor (Canada)",                 {"labors.CA",NULL}},
    {"0.005645",          "Labor (US)",                     {"labors.US","labor","labors",NULL}},
    {"0.579036",          "Manzana (Costa Rica)",           {"manzana","manzanas","manzanas.CR",NULL}},
    {"0.404686",          "Manzana (Argentina)",            {"manzanas.AR",NULL}},
    {"2.529285",          "Rai (Thailand)",                 {"rai",NULL}},
    {"3.999984",          "Rood",                           {"rood","roods",NULL}},
    {"11.241268",         "Sao (Vietnam)",                  {"sao",NULL}},
    {"463.027064",        "Scruple (ancient Rome)",         {"scruples.Rome",NULL}},
    {"0.001562",          "Section (US Survey)",            {"sec","section","sections",NULL}},
    {"435.599981",        "Square",                         {"sq","square","squares",NULL}},
    {"48.401584",         "Square (Sri Lanka)",             {"sq.SL",NULL}},
    {"62726399652.54841", "Square Caliber",                 {"sq.cal.",NULL}},
    {"40468564",          "Square Centimeter",              {"sq.cm.",NULL}},
    {"11151360.073126",   "Square Digit",                   {"sq.digit.",NULL}},
    {"43559.999759",      "Square International Foot",      {"sq.ft.IN",NULL}},
    {"43559.825338",      "Square US Survey Foot",          {"sq.ft","sq.ft.US",NULL}},
    {"6272639.96528",     "Square International Inch",      {"sq.in.IN",NULL}},
    {"6272614.848634",    "Square US Survey Inch",          {"sq.in","sq.in.US",NULL}},
    {"4046.8564",         "Square Meter",                   {"sq.m.",NULL}},
    {"0.004047",          "Square Kilometer",               {"sq.km.",NULL}},
    {"4839.980766",       "Square US Survey Yard",          {"sq.yd.","sq.yd.US",NULL}},
    {"0.001562",          "Square Miles",                   {"sq.mi.","square_miles",NULL}},
    {"38358.828494",      "Square Paris Foot (Canada)",     {"sq.ft.CA",NULL}},
    {"0.000043",          "Township",                       {"twp","township","townships",NULL}},
    {NULL,NULL,{NULL}}
};

/* based on US Gallons */
const struct conversion volumes[] = {
    {"4",              "#2.5 can",                  {"2.5can",NULL}},
    {"1",              "#10 can",                   {"10can",NULL}},
    {"0.031746",       "Barrel (US, liquid)",       {"bbl","barrel","barrels",NULL}},
    {"1.604167",       "Board Foot",                {"bd_ft","fbm","BF","board_feet",NULL}},
    {"0.107421",       "Bushel (US)",               {"bu",NULL}},
    {"3071.09474",     "Coffeespoon",               {"coffeespoon","coffeespoons",NULL}},
    {"3784.296607",    "Cooking milliliter",        {"cooking_milliliter","cooking_milliliters",NULL}},
    {"0.001044",       "Cord (firewood)",           {"cd","cord","cords",NULL}},
    {"0.008355",       "Cord Foot (timber)",        {"cd_ft","cord_foot","cord_feet",NULL}},
    {"3785.411784",    "Cubic Centimeter",          {"cc","cm3","cubic_centimeter","cubic_centimeters",NULL}},
    {"3.785411784",    "Cubic Decimeter",           {"dm3","cubic_decimeter","cubic_decimeters",NULL}},
    {"0.133681",       "Cubic Foot",                {"ft3","cubic_foot","cubic_feet",NULL}},
    {"231",            "Cubic Inch",                {"i3","cubic_inch","cubic_inches",NULL}},
    {"0.003785411784", "Cubic Meter",               {"m3","cubic_meter","cubic_meters",NULL}},
    {"16",             "Cup (US)",                  {"c","cups","cup",NULL}},
    {"12284.379957",   "Dash",                      {"ds",NULL}},
    {"58350.801541",   "Drop",                      {"gtt","drops","drop",NULL}},
    {"5",              "Fifth",                     {"fifths","fifth",NULL}},
    {"127.999998",     "Fluid Ounce (US)",          {"fl.oz.","fl.oz.US",NULL}},
    {"133.227867",     "Fluid Ounce (UK)",          {"fl.oz.UK",NULL}},
    {"1",              "Gallon (US)",               {"gal","gallon","gallons",NULL}},
    {"32",             "Gill (US)",                 {"gi","gill","gills",NULL}},
    {"0.03785411784",  "Hectoliter",                {"hl","hectoliter","hectoliters",NULL}},
    {"0.015873",       "Hogshead (US)",             {"hhd","hogshead","hogsheads",NULL}},
    {"85.333334",      "Jigger",                    {"jigger","jiggers",NULL}},
    {"3.785411784",    "Liter",                     {"l","liter","liters",NULL}},
    {"0.491612",       "Measure (Ancient Hebrew)",  {"measure","measures",NULL}},
    {"3785.411784",    "Milliliter",                {"ml","milliliter","milliliters",NULL}},
    {"8",              "Pint (US)",                 {"pt","pint","pints",NULL}},
    {"0.429684",       "Peck (US)",                 {"pk","peck","pecks",NULL}},
    {"6142.189979",    "Pinch",                     {"pinch","pinches",NULL}},
    {"0.007937",       "Pipe (US)",                 {"pipe","pipes",NULL}},
    {"127.999998",     "Pony",                      {"pony","ponies",NULL}},
    {"4",              "Quart (US)",                {"qt","quart","quarts",NULL}},
    {"0.013011",       "Quarter (UK)",              {"qtr.UK","Q.UK","quarter.UK","quarters.UK",NULL}},
    {"0.003785411784", "Stere",                     {"st","stere","steres",NULL}},
    {"3197.46888",     "Scruple (UK)",              {"scruple.UK","scruples.UK",NULL}},
    {"256",            "Tablespoon (US)",           {"tbsp","tbsp.US","tablespoon","tablespoons",NULL}},
    {"204.745415",     "Tablespoon (UK)",           {"tbsp.UK",NULL}},
    {"768",            "Teaspoon (US)",             {"tsp","tsp.US","teaspoon","teaspoons",NULL}},
    {"818.981651",     "Teaspoon (UK)",             {"tsp.UK",NULL}},
    {NULL,NULL,{NULL}}
};

/* based on the Avoirdupois Pound */
const struct conversion masses[] = {
    {"2268",                 "Carat",                   {"ct","carat","carats",NULL}},
    {"256",                  "Dram (Avoirdupois)",      {"dr.Av","dr","dram","drams",NULL}},
    {"116.666666666667",     "Dram (Apothecaries)",     {"dr.Ap",NULL}},
    {"7000",                 "Grain",                   {"gr","grain","grains",NULL}},
    {"453.6",                "Gram",                    {"g","gram","grams",NULL}},
    {"0.0004536",            "Microgram",               {"µg","mcg","microgram","micrograms",NULL}},
    {"0.4536",               "Milligram",               {"mg","milligram","milligrams",NULL}},
    {"0.01",                 "Hundredweight (US)",      {"Cwt","cwt","hundredweight","hundredweights",NULL}},
    {"0.4536",               "Kilogram",                {"kg","kilogram","kilograms",NULL}},
    {"16",                   "Ounce (Avoir.)",          {"oz","ounce","ounces","oz.Av",NULL}},
    {"14.5833333333334",     "Ounce (Apoth. & Troy)",   {"oz.Ap","oz.Tr",NULL}},
    {"291.666666666667",     "Pennyweight",             {"dwt","pwt","pennyweight","pennyweights",NULL}},
    {"1.21527777777777",     "Pound (Troy)",            {"lb_t","lb.Tr","#",NULL}},
    {"1",                    "Pound (Avoir.)",          {"lb","lb_av","lb.Av","lbs","pounds","pound",NULL}},
    {"0.04",                 "Quarter (US)",            {"qtr","Q","Qr","quarter","quarters","Qr.US",NULL}},
    {"350",                  "Scruple (Apoth.)",        {"s","scruple","scruples","s.Ap",NULL}},
    {"0.03105590062111801",  "Slugs",                   {"slug","slugs",NULL}},
    {"0.07142857142857142",  "Stone",                   {"stone","stones",NULL}},
    {"0.0005",               "Short Ton",               {"T","ton","tons","short_ton","short_tons",NULL}},
    {"0.0004536",            "Metric Ton / Tonne",      {"MT","t","metric_ton","metric_tons",NULL}},
    {"0.0004464285714285714","Long Ton",                {"L.T.","LT","long_ton","long_tons",NULL}},
    {NULL,NULL,{NULL}}
};

/* based on feet per second */
const struct conversion speeds[] = {
    {"30.48",                  "Centimeters/second",            {"cm/s",NULL}},
    {"0.3048",                 "Meters/second",                 {"m/s",NULL}},
    {"1",                      "Feet/second",                   {"f/s",NULL}},
    {"60",                     "Feet/minute",                   {"f/m",NULL}},
    {"0.0909090909090909091",  "Furlongs/minute",               {"fur/m",NULL}},
    {"1.0972853161278995",     "Kilometers/hour",               {"km/h",NULL}},
    {"0.5924837511331251",     "Knots",                         {"k","knot","knots",NULL}},
    {"5.454644629902362",      "Leagues/day",                   {"leagues/day","league/day",NULL}},
    {"0.0009191187537183524",  "Mach (dry air, 273 kelvin)",    {"M","Ma","mach","machs",NULL}},
    {"0.68181818181818181819", "Miles/hour",                    {"mph","mile/hr","miles/hr","mile/hour","miles/hour",NULL}},
    {NULL,NULL,{NULL}}
};

/* based on horsepower */
const struct conversion powers[] = {
    {"2547.160889", "BTU/hour",             {"BTU/hr",NULL}},
    {"7460000000",  "Ergs/sec",             {"ergs/s",NULL}},
    {"550.221342",  "Foot-lbs/sec",         {"ft-lbf/s","ft-lb/s","foot-pound_per_second","foot-pounds_per_second",NULL}},
    {"1",           "Horsepower",           {"hp","horse","horsepower",NULL}},
    {"10.697948",   "KiloCalories/min",     {"kC/m",NULL}},
    {"0.178299",    "KiloCalories/sec",     {"kC/s",NULL}},
    {"0.746",       "Kilowatts",            {"kW","kilowatt","kilowatts",NULL}},
    {"0.000000746", "Gigawatts",            {"GW","gigawatt","gigawatts",NULL}},
    {"0.000746",    "Megawatts",            {"MW","megawatt","megawatts",NULL}},
    {"746000",      "Milliwatts (mW)",      {"mW","milliwatt","milliwatts",NULL}},
    {"746",         "Watts (joules/sec)",   {"W","watt","watts","J/s",NULL}},
    {NULL,NULL,{NULL}}
};

/* based on Newtons */
const struct conversion forces[] = {
    {"100000",     "Dyne",                  {"dyn","dyne","dynes",NULL}},
    {"101.971621", "Gram-force",            {"gf","gram-force",NULL}},
    {"0.101972",   "Kilopond (kgm-force)",  {"kgf","kilopond","kiloponds",NULL}},
    {"0.000225",   "Kip (kilopound",        {"kips","kilopound","kilopounds",NULL}},
    {"1",          "Newton",                {"N","newton","newtons",NULL}},
    {"0.224809",   "Pound",                 {"lbf","pound_force","pounds_force",NULL}},
    {"7.233011",   "Poundal",               {"pdl","pl","poundal","poundals",NULL}},
    {NULL,NULL,{NULL}}
};

/* based on meters per square second */
const struct conversion accelerations[] = {
    {"3.28084",    "Celo",                      {"celo","celos",NULL}},
    {"10000",      "Centigal",                  {"centigal","centigals",NULL}},
    {"100",        "Centimeter/square second",  {"cm/s/s","cm/s^2",NULL}},
    {"1000",       "Decigal",                   {"Dg","decigal","decigals",NULL}},
    {"3.28084",    "Foot/square second",        {"f/s/s","f/s^2",NULL}},
    {"0.101972",   "G-unit (G)",                {"G","G-units",NULL}},
    {"100",        "Galileo",                   {"Gal","gal","galileo","galileos",NULL}},
    {"0.101972",   "Grav",                      {"grav","gravs",NULL}},
    {"39.370079",  "Inch/square second",        {"in/s/s","in/s^2",NULL}},
    {"3.6",        "Kilometer/hour/second",     {"km/h/s",NULL}},
    {"0.001",      "Kilometer/square second",   {"km/s/s","km/s^2",NULL}},
    {"0.1",        "Leo",                       {"leo","leos",NULL}},
    {"1",          "Meter/square second",       {"m/s/s","m/s^2",NULL}},
    {"134.216178", "Mile/hour/minute",          {"mi/h/m",NULL}},
    {"2.236936",   "Mile/hour/second",          {"mi/h/s",NULL}},
    {"0.000621",   "Mile/square second",        {"mi/s/s","mi/s^2",NULL}},
    {"100000",     "Milligal",                  {"mGal","milligals",NULL}},
    {"1000",       "Millimeter/square second",  {"mm/s/s","mm/s^2",NULL}},
    {NULL,NULL,{NULL}}
};

const struct conversion temperatures[] = {
    {"", "Kelvin", {"kelvin", "Kelvin", "K", NULL}},
    {"", "Celsius", {"celsius", "Celsius", "C", NULL}},
    {"", "Rankine", {"rankine", "Rankine", NULL}},
    {"", "Farenheit", {"farenheit", "Farenheit", "F", NULL}},
    {"", "Reaumur", {"reaumur", "Reaumur", "R", NULL}},
    {NULL,NULL,{NULL}}
};

char * from_temperatures[] = {
    "[%1.15f]",                     // kelvin
    "[%1.15f + 273.15]",            // celsius
    "[%1.15f / 1.8]",               // rankine
    "[(%1.15f - 32)/1.8 + 273.15]", // farenheit
    "[((5/4) * %1.15f) + 273.15]"   // reaumur
};

char * to_temperatures[] = {
    "%s",                 // kelvin
    "%s - 273.15",        // celsius
    "%s * 1.8",           // rankine
    "1.8*(%s-273.15)+32", // farenheit
    "(%s-273.15)*(4/5)"   // reaumur
};

const struct conversion * conversions[] = {
    lengths,
    areas,
    volumes,
    masses,
    speeds,
    powers,
    forces,
    accelerations,
    temperatures,
    NULL
};

#define CONVERS(x) (x>7)?accelerations:((x>6)?forces:((x>5)?powers:((x>4)?speeds:((x>3)?masses:((x>2)?volumes:((x>1)?areas:lengths))))))

/* returns the category number of the unit */
int identify_unit(char * unit)
{
    size_t cat_num;
    int u = -1;

    for (cat_num = 0; conversions[cat_num] != NULL; cat_num++) {
        size_t unit_num;
        const struct conversion * category = conversions[cat_num];

        for (unit_num = 0; category[unit_num].name != NULL; unit_num++) {
            size_t abbrev_num;

            for (abbrev_num = 0; category[unit_num].aka[abbrev_num] != NULL; abbrev_num++) {
                if (!strcmp(category[unit_num].aka[abbrev_num], unit)) {
                    return cat_num;
                }
            }
        }
    }
    return -1;
}

int identify_units(char * unit1, char * unit2)
{
    size_t cat_num;
    int u1 = -1, u2 = -1;

    for (cat_num = 0; conversions[cat_num] != NULL; cat_num++) {
        size_t unit_num;
        const struct conversion *category = conversions[cat_num];

        for (unit_num = 0; category[unit_num].name != NULL; unit_num++) {
            size_t abbrev_num;

            for (abbrev_num = 0;
                    category[unit_num].aka[abbrev_num] != NULL ;
                    abbrev_num++) {
                if (!strcmp(category[unit_num].aka[abbrev_num], unit1)) {
                    if (u2 == -1) {
                        u1 = cat_num;
                    } else if (u2 == cat_num) {
                        return u2;
                    } else {
                        return -1;
                    }
                } else if (!strcmp(category[unit_num].aka[abbrev_num],unit2)) {
                    if (u1 == -1) {
                        u2 = cat_num;
                    } else if (u1 == cat_num) {
                        return u1;
                    } else {
                        return -1;
                    }
                }
            }
        }
    }
    if (-1 == u1) {
        return -3;
    } else if (-1 == u2) {
        return -4;
    }
    return -2;
}

int unit_id(int utype, char * unit)
{
    size_t unit_num;

    for (unit_num = 0; (conversions[utype])[unit_num].name != NULL; unit_num++) {
        size_t abbrev_num;

        for (abbrev_num = 0;
                (conversions[utype])[unit_num].aka[abbrev_num] != NULL ;
                abbrev_num++) {
            if (!strcmp((conversions[utype])[unit_num].aka[abbrev_num], unit)) {
                return unit_num;
            }
        }
    }
    return -1;
}

void uber_conversion (mpfr_t output, int utype, int fromunit, int tounit, mpfr_t value)
{
    if (utype != TEMPERATURE_C) {
        const struct conversion *ltable = conversions[utype];
        mpfr_t tofac, fromfac;

        mpfr_init_set_str(tofac,ltable[tounit].factor,0,GMP_RNDN);
        mpfr_init_set_str(fromfac,ltable[fromunit].factor,0,GMP_RNDN);
        mpfr_div(tofac,tofac,fromfac,GMP_RNDN);
        mpfr_mul(output,tofac,value,GMP_RNDN);
        mpfr_clear(tofac);
        mpfr_clear(fromfac);
    } else {
        mpfr_t temp;

        mpfr_init(temp);
        /* convert to kelvin */
        switch (fromunit) {
            case KELVIN:
                break;
            case CELSIUS:
                mpfr_set_d(temp,273.15,GMP_RNDN);
                mpfr_add(value,value,temp,GMP_RNDN);
                break;
            case RANKINE:
                mpfr_set_d(temp,1.8,GMP_RNDN);
                mpfr_div(value,value,temp,GMP_RNDN);
                break;
            case FARENHEIT:
                mpfr_sub_ui(value,value,32,GMP_RNDN);
                mpfr_set_d(temp,1.8,GMP_RNDN);
                mpfr_div(value,value,temp,GMP_RNDN);
                mpfr_set_d(temp,273.15,GMP_RNDN);
                mpfr_add(value,value,temp,GMP_RNDN);
                break;
            case REAUMUR:
                mpfr_set_d(temp,(5/4),GMP_RNDN);
                mpfr_mul(value,value,temp,GMP_RNDN);
                mpfr_set_d(temp,273.15,GMP_RNDN);
                mpfr_add(value,value,temp,GMP_RNDN);
                break;
        }
        /* convert from kelvin */
        switch (tounit) {
            case KELVIN:
                break;
            case CELSIUS:
                mpfr_set_d(temp,273.15,GMP_RNDN);
                mpfr_sub(output,value,temp,GMP_RNDN);
                break;
            case RANKINE:
                mpfr_set_d(temp,1.8,GMP_RNDN);
                mpfr_mul(output,value,temp,GMP_RNDN);
                break;
            case FARENHEIT:
                mpfr_set_d(temp,273.15,GMP_RNDN);
                mpfr_sub(value,value,temp,GMP_RNDN);
                mpfr_set_d(temp,1.8,GMP_RNDN);
                mpfr_mul(value,value,temp,GMP_RNDN);
                mpfr_add_ui(output,value,32,GMP_RNDN);
                break;
            case REAUMUR:
                mpfr_set_d(temp,273.15,GMP_RNDN);
                mpfr_sub(value,value,temp,GMP_RNDN);
                mpfr_set_d(temp,(4/5),GMP_RNDN);
                mpfr_mul(output,value,temp,GMP_RNDN);
                break;
        }
        mpfr_clear(temp);
/* The old way *//*
        //char stage1[100];
        //char composite[100];
        //sprintf(stage1,from_temperatures[fromunit],value);
        //sprintf(composite,to_temperatures[tounit],stage1);
        //return parseme(composite);
        */
    }
}



