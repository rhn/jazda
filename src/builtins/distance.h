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

/* distance data management subsystem */

// These two defines shouldn't ever be used by modules
// converts METRIC_PULSE_DIST in mm to km/100 (10^DIST_FRACTION_DIGITS) with FRAC_BITS long mantissa
#define get_pulse_dist(mpd) ((((uint32_t)mpd) << FRAC_BITS) / 10000L) // TODO: power 10 ^ (6 - DIST_DIGITS) : 6 = mm->km
#ifndef INITIAL_METRIC_PULSE_DIST
    #define INITIAL_METRIC_PULSE_DIST (METRIC_PULSE_DIST)
#endif
#define INITIAL_PULSE_DIST (get_pulse_dist(INITIAL_METRIC_PULSE_DIST))


typedef uint16_t pulse_dist_mm_t;
typedef uint16_t pulse_dist_t;


#ifdef CONSTANT_PULSE_DISTANCE
    extern const pulse_dist_mm_t millimeters_pulse_distance;
    extern const pulse_dist_t pulse_dist;
#else
    // Read-only values, only write with pulse_distance_set_mm
    // TODO: remove external references
    extern volatile pulse_dist_mm_t millimeters_pulse_distance;
    extern volatile pulse_dist_t pulse_dist;
#endif

#ifndef CONSTANT_PULSE_DISTANCE
    // Only set distance values. Private API, shouldn't be used by modules and builtins other than storage- and init- related
    void pulse_distance_set_metric(pulse_dist_mm_t mmpd);
    
    // Set distance values and save
    void pulse_distance_update_metric(pulse_dist_mm_t mmpd);
#endif
