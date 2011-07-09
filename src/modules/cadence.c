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

static inline uint16_t get_crank_average(uint16_t time_difference, uint8_t pulse_count) {
    return get_rotational_speed(CRANK_FACTOR, time_difference, pulse_count);
}

void cadence_on_crank_pulse(void) {
    if (!cadence_stopped) {
        module_flags.cadence_changed = true;
    }
}

void cadence_on_stop(void) {
    module_flags.cadence_changed = true;
}

void cadence_redraw(const uint8_t force) {
    if (force || module_flags.cadence_changed) {
        uint16_t rpm;
        upoint_t position = {0, 5};
        upoint_t glyph_size = {8, 8};
        module_flags.cadence_changed = false;
        
        if (crank_pulse_count > 1) {
            uint16_t time_difference = crank_pulse_times[0] - crank_pulse_times[crank_pulse_count - 1];
    //        speed = ((uint32_t)(((uint64_t)SPEED_FACTOR * ((avgspeed_pulses - 1))) >> FRAC_BITS)) / time_difference;
            rpm = get_crank_average(time_difference, crank_pulse_count);
        } else {
            rpm = 0;
        }
        print_number(rpm, position, glyph_size, 1, CADENCE_DIGITS);
    }
}

#define cadence_signature {0b10000000, 0b01100000, 0b00011000, 0b00000110, 0b00000001, 0b00000110, 0b00011000, 0b11100000}
#define cadence_record {&cadence_redraw, &module_select_null, cadence_signature}
