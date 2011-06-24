/* Average speed module
REQUIRES: speed, LONG_SPEED
*/

volatile uint32_t avgspeed_start_time;
volatile uint16_t avgspeed_pulses = 0;

void avgspeed_on_pulse(const uint16_t now) {
    if (avgspeed_pulses == 0) {
        avgspeed_start_time = ((uint32_t)extended_time << 16) | now;
    }
    avgspeed_pulses++;
}

module_actions_t *avgspeed_select(const uint8_t state) {
    if (state) {
        avgspeed_pulses = 0;
    }
    return NULL;
}

void avgspeed_redraw(const uint8_t force) {
    if (force) {
       erase_module_screen();
    }
    uint32_t speed;
    upoint_t position = {0, 5};
    upoint_t glyph_size = {8, 8};
    if (avgspeed_pulses > 1) {
        uint32_t time_difference = get_extended_time() - avgspeed_start_time;
//        speed = ((uint32_t)(((uint64_t)SPEED_FACTOR * ((avgspeed_pulses - 1))) >> FRAC_BITS)) / time_difference;
        speed = get_int_average_long(time_difference, avgspeed_pulses);
        print_number(avgspeed_pulses, position, glyph_size, 1, 4<<4);
    } else {
        speed = 0;
    }
    position.y = 4;
    print_number(speed, position, glyph_size, 1, SPEED_DIGITS);
}

#define avgspeed_signature {0b10000000, 0b01100000, 0b00011000, 0b00000110, 0b00000001, 0b00000110, 0b00011000, 0b11100000}

#define avgspeed_record {&avgspeed_redraw, &avgspeed_select, avgspeed_signature}
