/* Module displaying max speed
REQUIRES: current speed module

Finds average speed based on the rolling average of PULSE_TABLE_SIZE last pulses
(similar algorithm as speed).

Same algorithm as speed isn't used due to size/speed overhead when comparing
time differences of different pulse counts.
*/


// TODO: -1 and no special code or 0 and special case? size optimization
volatile uint16_t maxspeed_min_difference = -1;
volatile uint8_t maxspeed_modified = false;

void maxspeed_on_pulse(void) {
    if (oldest_pulse_index < PULSE_TABLE_SIZE) { // rolling average of PULSE_TABLE_SIZE pulses
        return;
    }
    uint16_t pulse_difference = pulse_table[1] - pulse_table[PULSE_TABLE_SIZE];
    
    if (maxspeed_min_difference > pulse_difference) {
        maxspeed_min_difference = pulse_difference;
        maxspeed_modified = true;
    }
}

void maxspeed_reset(void) {
    maxspeed_min_difference = -1;
    maxspeed_modified = true;
}

module_actions_t *maxspeed_select(const uint8_t state) {
    if (state) {
        maxspeed_reset();
    }
    return NULL;
}

void maxspeed_redraw(const uint8_t force) {
    if (force || maxspeed_modified) {
        if (force) {
           erase_module_screen();
        }
        uint16_t speed;
        if (maxspeed_min_difference == (uint16_t)-1) {
            speed = 0;
        } else {
            speed = get_int_average(maxspeed_min_difference, PULSE_TABLE_SIZE - 1);
        }
        upoint_t position = {0, 5};
        upoint_t glyph_size = {8, 8};
        print_number(speed, position, glyph_size, 1, SPEED_DIGITS);
        maxspeed_modified = false;
    }
}

#define maxspeed_signature {0b10000000, 0b01100000, 0b00011000, 0b00000110, 0b00000001, 0b00000110, 0b00011000, 0b11100000}
#ifdef COMBINED_RESET
    #define maxspeed_record {&maxspeed_redraw, &combined_reset_on_select, maxspeed_signature}
#else
    #define maxspeed_record {&maxspeed_redraw, &maxspeed_select, maxspeed_signature}
#endif
