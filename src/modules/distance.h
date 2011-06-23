#define DIST_SIGNIFICANT_DIGITS 3 // 999km is good enough
#define DIST_FRACTION_DIGITS 2 // same as my sigma

volatile uint32_t distance = 0;

inline void distance_on_pulse(const uint16_t now) {
  distance += PULSE_DIST;
  // TODO: asm this to use 1 tmp reg?
}

void distance_redraw(uint8_t force) {
   upoint_t position = {0, 5};
   upoint_t glyph_size = {8, 8};
   if (force) {
      set_row(4);
      set_column(0);
      module_erase_screen(84*2);
   }
   print_number(distance >> FRAC_BITS, position, glyph_size, 1, NIBBLEPAIR(DIST_SIGNIFICANT_DIGITS, DIST_FRACTION_DIGITS));
}

module_actions_t *distance_select(const uint8_t state) {
   return NULL;
}

#define distance_signature {0b01111110, 0b01000010, 0b00111100, 0, 0b01111110, 0, 0b01001100, 0b00110010}

#define distance_record {&distance_redraw, &distance_select, distance_signature}
