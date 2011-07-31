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

/* Cadence module
REQUIRES: crank
*/

#include "../builtins/crank.h"
#include "../lib/calculations.h"
#include "../display/drawing.h"

#define CADENCE_DIGITS (number_display_t){.integer=3, .fractional=0}
#define CADENCE_FACTOR ((((uint32_t)(ONE_SECOND)) << FRAC_BITS) * 60)

volatile uint8_t cadence_changed = true;

static inline uint16_t get_crank_average(uint16_t time_difference, uint8_t pulse_count) {
    return get_rot_speed(CADENCE_FACTOR, time_difference, pulse_count);
}

void cadence_on_crank_pulse(void) {
//    if (!crank_stopped) { // TODO: extend crank to support stop
        cadence_changed = true;
//    }
}

void cadence_on_stop(void) {
    cadence_changed = true;
}

void cadence_redraw(void) {
    if (cadence_changed) {
        uint16_t rpm;
        upoint_t position = {0, 0};
        upoint_t glyph_size = {8, 8};
        cadence_changed = false;
        
        if (crank_pulse_count > 1) {
            uint16_t time_difference = crank_pulse_times[0] - crank_pulse_times[crank_pulse_count - 1];
            rpm = get_crank_average(time_difference, crank_pulse_count - 1);
        } else {
            rpm = 0;
        }
        print_number(rpm, position, glyph_size, 1, CADENCE_DIGITS);
    }
}
