#include "preferences.h"

/* GENERATED VALUES */

/* generates numeric data but also interrupt defines for the CPU header */

// converts METRIC_PULSE_DIST in mm to km/100 (10^DIST_FRACTION_DIGITS) with FRAC_BITS long mantissa
#define PULSE_DIST (uint64_t)((uint64_t)(METRIC_PULSE_DIST << FRAC_BITS) / 10000L) // TODO: power 10 ^ (6 - DIST_DIGITS) : 6 = mm->km

#define ONE_SECOND 986 // in timer ticks // TODO: autocalculate
