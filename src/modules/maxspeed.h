/* Module displaying max speed
REQUIRES: current speed module

Finds average speed based on the rolling average of PULSE_TABLE_SIZE last pulses
(same algorithm as speed, but diferent code).

TODO: integrate maxspeed_on_pulse, pulse_difference and pulses into normal speed
calculation & display (reducing main loop)?
*/


// TODO: -1 and no special code or 0 and special case? size optimization
volatile uint16_t maxspeed_min_difference = -1;
volatile uint8_t maxspeed_pulses = -1;
volatile uint8_t maxspeed_modified = false;

void maxspeed_on_pulse(void) {
    if (oldest_pulse_index < 2) { // rolling average of at least 2 pulses
        return;
    }
    uint16_t pulse_difference = pulse_table[1] - pulse_table[oldest_pulse_index];
    
    if (maxspeed_min_difference > pulse_difference) {
        maxspeed_min_difference = pulse_difference;
        maxspeed_pulses = oldest_pulse_index - 1;
        maxspeed_modified = true;
    }
}

module_actions_t *maxspeed_select(const uint8_t state) {
    if (state) {
        maxspeed_min_difference = -1;
        maxspeed_pulses = -1;
        maxspeed_modified = true;
    }
    return NULL;
}

void maxspeed_redraw(const uint8_t force) {
    if (force || maxspeed_modified) {
        if (force) {
          set_row(4);
          set_column(0);
          module_erase_screen(84*2);
        }

        uint16_t speed = get_int_average(maxspeed_min_difference, maxspeed_pulses);
        upoint_t position = {0, 5};
        upoint_t glyph_size = {8, 8};
        print_number(speed, position, glyph_size, 1, SPEED_DIGITS);
        maxspeed_modified = false;
    }
}

#define maxspeed_signature {0b10000000, 0b01100000, 0b00011000, 0b00000110, 0b00000001, 0b00000110, 0b00011000, 0b11100000}

#define maxspeed_record {&maxspeed_redraw, &maxspeed_select, maxspeed_signature}
