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

/*
const pdist = (mpd << FRAC_BITS) / 10000L
const speed_factor = pdist * ONE_SECOND * 36 * 10

uint16_t div(uint32_t, uint16_t);

uint16_t nonzero mpd = 2133;
uint16_t pdist = divide(((uint32_t)mpd) << FRAC_BITS, 10000L) // 12 << 10 = 22b / 14 = 6
uint16_t speed_factor = multiply(pdist, ONE_SECOND * 36 * 10);
*/

#include "../display/drawing.h"
#include "../actions.h"
#include "../builtins/distance.h"
#include "../builtins/speed.h"

#define CONFIG_DIGITS 4

static volatile uint8_t config_level = 0;
static volatile uint8_t config_place;
static volatile pulse_dist_mm_t config_pulse_distance;

pulse_dist_mm_t config_power10(void) {
    pulse_dist_mm_t ret;
    if (config_place > 0) {
        ret = 1;
        for (uint8_t i = 1; i < config_place; i++) {
            ret *= 10;
        }
    } else {
        ret = 0;
    }
    return ret;
}

void config_save(void) {
    pulse_distance_update_metric(config_pulse_distance);
}

void config_update_display_values(const pulse_dist_mm_t difference) {
    pulse_dist_mm_t newmpd = config_pulse_distance + difference;
    if (newmpd >= 10000) {
        newmpd -= 10 * difference;
    }
    config_pulse_distance = newmpd;
    module_flags.config_changed = true;
}

void config_value_up(const uint8_t state) {
    if (!state) {
        config_update_display_values(config_power10());
    }
}

void config_value_down(const uint8_t state) {
    if (!state) {
        config_update_display_values(-config_power10());
    }
}

const module_actions_t config_select_value_actions = {&config_value_down, &config_value_up};

void config_select_digit_left(const uint8_t state) {
    if (!state) {
        module_flags.config_changed = true;
        config_place++;
        if (config_place > CONFIG_DIGITS) {
            config_place = 0;
        }
    }
}

void config_select_digit_right(const uint8_t state) {
    if (!state) {
        module_flags.config_changed = true;
        config_place--;
        if (config_place > CONFIG_DIGITS) {
            config_place = CONFIG_DIGITS;
        }
    }
}

const module_actions_t config_select_digit_actions = {&config_select_digit_left, &config_select_digit_right};

const module_actions_t* config_select(const uint8_t state) {
    if (!state) {
        module_flags.config_changed = true;
        if (config_level == 0) {
            config_place = CONFIG_DIGITS;
            config_level++;
            return &config_select_digit_actions;
        } else if (config_level == 1) {
            if (config_place != 0) {
                config_level++;
                return &config_select_value_actions;
            } else {
                config_save();
                config_level = 0;
                return &default_actions;
            }
        } else { // config_level == 2
            config_level--;
            return &config_select_digit_actions;
        }
    } else {
        return NULL;
    }
}

void config_redraw(const uint8_t force) {
    if (force || module_flags.config_changed) {
        module_flags.config_changed = false;
        upoint_t position = {0, 4};
        upoint_t glyph_size = {8, 8};
        
        // erase old arrows
        erase_module_screen(); // makes screen flicker, but this is not important - not used often
        print_number((uint16_t)config_pulse_distance, position, glyph_size, 1, (number_display_t) {CONFIG_DIGITS, 0});
        if (config_level > 0) {
            position.y += 1;
            position.x = (CONFIG_DIGITS - config_place) * 9;
            set_row(position.y);
            set_column(position.x);
            char stamp = 0b111;
            if (config_level == 2) {
                stamp = 0b1;
            }
            for (uint8_t i = 0; i < 7; i++) {
                send_raw_byte(stamp, true);
            }
        }
    }
}

void config_on_pulse_distance_change(void) {
    config_pulse_distance = millimeters_pulse_distance;
}

#define config_signature {0b00111000, 0b00010100, 0b10010100, 0b11100100, 0b10000010, 0b01110001, 0b00001000, 0b00000100}

#define config_record {&config_redraw, &config_select, config_signature}
