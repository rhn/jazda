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

/* Module displaying max speed
REQUIRES: current speed module

Finds average speed based on the rolling average of PULSE_TABLE_SIZE last pulses
(similar algorithm as speed).

Same algorithm as speed isn't used due to size/speed overhead when comparing
time differences of different pulse counts.
*/


#include "../builtins/wheel.h"
#include "../display/drawing.h"
#include "../lib/calculations.h"

// TODO: -1 and no special code or 0 and special case? size optimization
static volatile uint16_t maxspeed_min_difference = -1;

void maxspeed_on_wheel_pulse(void) {
    if (wheel_pulse_count < WHEEL_PULSE_TABLE_SIZE) { // rolling average of PULSE_TABLE_SIZE pulses
        return;
    }
    uint16_t time_difference = wheel_pulse_table[0] - wheel_pulse_table[WHEEL_PULSE_TABLE_SIZE - 1];
    
    if (maxspeed_min_difference > time_difference) {
        maxspeed_min_difference = time_difference;
        module_flags.maxspeed_changed = true;
    }
}

void maxspeed_reset(void) {
    maxspeed_min_difference = -1;
    module_flags.maxspeed_changed = true;
}

module_actions_t *maxspeed_select(const uint8_t state) {
    if (state) {
        maxspeed_reset();
    }
    return NULL;
}

void maxspeed_redraw(const uint8_t force) {
    if (force || module_flags.maxspeed_changed) {
        uint16_t speed;
        module_flags.maxspeed_changed = false;
        if (maxspeed_min_difference == (uint16_t)-1) {
            speed = 0;
        } else {
            speed = get_average_speed(maxspeed_min_difference, WHEEL_PULSE_TABLE_SIZE - 1);
        }
        upoint_t position = {0, 5};
        upoint_t glyph_size = {8, 8};
        print_number(speed, position, glyph_size, 1, SPEED_DIGITS);
    }
}

#define maxspeed_signature {0b10000000, 0b01100000, 0b00011000, 0b00000110, 0b00000001, 0b00000110, 0b00011000, 0b11100000}
#ifdef COMBINED_RESET
    #define maxspeed_record {&maxspeed_redraw, &combined_reset_on_select, maxspeed_signature}
#else
    #define maxspeed_record {&maxspeed_redraw, &maxspeed_select, maxspeed_signature}
#endif
