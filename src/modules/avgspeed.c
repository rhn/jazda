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

/* Average speed module
REQUIRES: speed, LONG_SPEED
Doesn't add single pulses to average when bicycle is stopped. Average counted
from first pulse in streak to stopped time.
*/

#include "../builtins/wheel.h"

volatile uint32_t avgspeed_total_time = 0;
volatile uint16_t avgspeed_pulses = 0;

void avgspeed_on_wheel_pulse(void) {
    if (wheel_pulse_count > 1) {
        // pulse index not incremented yet, but data already in the table!
        avgspeed_total_time += wheel_pulse_table[0] - wheel_pulse_table[1];
        avgspeed_pulses++;
        module_flags.avgspeed_changed = true;
    }
}

void avgspeed_reset(void) {
    avgspeed_pulses = 0;
    avgspeed_total_time = 0;
    module_flags.avgspeed_changed = true;
}

module_actions_t *avgspeed_select(const uint8_t state) {
    if (state) {
        avgspeed_reset();
    }
    return NULL;
}

void avgspeed_redraw(const uint8_t force) {
    if (force || module_flags.avgspeed_changed) {
        uint16_t speed;
        upoint_t position = {0, 5};
        upoint_t glyph_size = {8, 8};
        module_flags.avgspeed_changed = false;
        if (avgspeed_pulses) {
            uint32_t time_difference = avgspeed_total_time;
    //        speed = ((uint32_t)(((uint64_t)SPEED_FACTOR * ((avgspeed_pulses - 1))) >> FRAC_BITS)) / time_difference;
            speed = get_average_speed_long(time_difference, avgspeed_pulses);
        } else {
            speed = 0;
        }
        print_number(speed, position, glyph_size, 1, SPEED_DIGITS);
    }
}

#define avgspeed_signature {0b10000000, 0b01100000, 0b00011000, 0b00000110, 0b00000001, 0b00000110, 0b00011000, 0b11100000}
#ifdef COMBINED_RESET
    #define avgspeed_record {&avgspeed_redraw, &combined_reset_on_select, avgspeed_signature}
#else
    #define avgspeed_record {&avgspeed_redraw, &avgspeed_select, avgspeed_signature}
#endif
