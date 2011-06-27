/* Average speed module
REQUIRES: speed, LONG_SPEED
Doesn't add single pulses to average when bicycle is stopped. Average counted
from first pulse in streak to stopped time.
*/

volatile uint32_t avgspeed_total_time = 0;
volatile uint16_t avgspeed_pulses = 0;

void avgspeed_on_pulse(void) {
    if (oldest_pulse_index) {
        // pulse index not incremented yet, but data already in the table!
        avgspeed_total_time += pulse_table[0] - pulse_table[2];
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
        uint32_t speed;
        upoint_t position = {0, 5};
        upoint_t glyph_size = {8, 8};
        module_flags.avgspeed_changed = false;
        if (avgspeed_pulses) {
            uint32_t time_difference = avgspeed_total_time;
    //        speed = ((uint32_t)(((uint64_t)SPEED_FACTOR * ((avgspeed_pulses - 1))) >> FRAC_BITS)) / time_difference;
            speed = get_int_average_long(time_difference, avgspeed_pulses);
            print_number(avgspeed_pulses, position, glyph_size, 1, 4<<4);
        } else {
            speed = 0;
        }
        position.y = 4;
        print_number(speed, position, glyph_size, 1, SPEED_DIGITS);
    }
}

#define avgspeed_signature {0b10000000, 0b01100000, 0b00011000, 0b00000110, 0b00000001, 0b00000110, 0b00011000, 0b11100000}
#ifdef COMBINED_RESET
    #define avgspeed_record {&avgspeed_redraw, &combined_reset_on_select, avgspeed_signature}
#else
    #define avgspeed_record {&avgspeed_redraw, &avgspeed_select, avgspeed_signature}
#endif
