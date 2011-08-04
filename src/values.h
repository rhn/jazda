/*
    Copyright 2011 rhn <gihu.rhn@porcupinefactory.org>

    This file is part of Jazda.

    Jazda is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Jazda is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Jazda.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "preferences.h"

/* GENERATED VALUES */

/* generates numeric data but also interrupt defines for the CPU header */

// converts METRIC_PULSE_DIST in mm to km/100 (10^DIST_FRACTION_DIGITS) with FRAC_BITS long mantissa
#define PULSE_DIST (uint64_t)((uint64_t)(METRIC_PULSE_DIST << FRAC_BITS) / 10000L) // TODO: power 10 ^ (6 - DIST_DIGITS) : 6 = mm->km
#define pulse_dist PULSE_DIST
#define ONE_SECOND 3695 // in timer ticks, calibrated for 1MHz/256 // TODO: autocalculate
