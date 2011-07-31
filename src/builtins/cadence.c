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

/* Cadence builtin
Displays the current cadence as a number in rpm, next to a vertical bar
representing that value. The bar shows only a fraction of possible values,
defined by CADENCE_BAR_MINIMUM and CADENCE_BAR_MAXIMUM values.
CADENCE_BAR_OPTIMUM is used to display a graphical hint about the cadence the
user wants to keep.
CADENCE_BAR_HEIGHT defines how many lines the bar spans vertically.
WARNING: this module's bar requires a line-based display.
*/

#include "../builtins/crank.h"
#include "../lib/calculations.h"
#include "../display/drawing.h"

#define CADENCE_BAR_HEIGHT 3
#define CADENCE_BAR_MINIMUM 50
#define CADENCE_BAR_MAXIMUM 150
#define CADENCE_BAR_OPTIMUM 90

#define CADENCE_DIGITS (number_display_t){.integer=3, .fractional=0}

/* calculated values */
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


// TODO: a lot of optimizations for size to be done
void cadence_draw_bar(uint16_t rpm) {
    int16_t height = rpm - CADENCE_BAR_MINIMUM;
    if (height < 0) {
        height = 0;
    }
    height *= CADENCE_BAR_HEIGHT * 8; // 8px in each line
    height /= CADENCE_BAR_MAXIMUM - CADENCE_BAR_MINIMUM;
    
    uint16_t optimum_pixels = ((CADENCE_BAR_OPTIMUM - CADENCE_BAR_MINIMUM) * CADENCE_BAR_HEIGHT * 8) / (CADENCE_BAR_MAXIMUM - CADENCE_BAR_MINIMUM);
    
    upoint_t position = {27, 0};
    for (uint8_t i = 0; i < CADENCE_BAR_HEIGHT; i++) {
        uint8_t current_top = 8 * (CADENCE_BAR_HEIGHT - i);
        //char buffer[6];
        set_column(position.x);
        set_row(position.y);
        
        char optimum_stamp = 0;
        optimum_stamp |= 1 << (current_top - optimum_pixels + 1);
        char rpm_stamp = ~0;
        if (height < current_top) {
            rpm_stamp <<= current_top - height;
        }
        
        send_raw_byte(optimum_stamp, true);
        send_raw_byte(optimum_stamp, true);
        send_raw_byte(rpm_stamp ^ optimum_stamp, true);
        send_raw_byte(rpm_stamp, true);
        send_raw_byte(rpm_stamp, true);
        send_raw_byte(~optimum_stamp, true);
        
        //for (uint8_t y = 0; y < 6; y++) {
        //    send_raw_byte(buffer[y], true);
        //}
        
        position.y++;
    }
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
        cadence_draw_bar(rpm);
        print_number(rpm, position, glyph_size, 1, CADENCE_DIGITS);
    }
}
