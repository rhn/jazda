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
#include "distance.h"


#ifdef CONSTANT_PULSE_DISTANCE
    const uint16_t millimeters_pulse_distance = INITIAL_METRIC_PULSE_DIST;
    const uint16_t pulse_dist = get_pulse_dist(INITIAL_METRIC_PULSE_DIST);
#else
    volatile uint16_t millimeters_pulse_distance = INITIAL_METRIC_PULSE_DIST;
    volatile uint16_t pulse_dist = get_pulse_dist(INITIAL_METRIC_PULSE_DIST);
#endif

#ifndef CONSTANT_PULSE_DISTANCE
    void update_pulse_distance(void) {
        pulse_dist = get_pulse_dist(millimeters_pulse_distance);
    }
#endif
