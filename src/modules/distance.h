#define DIST_SIGNIFICANT_DIGITS 3 // 999km is good enough
#define DIST_FRACTION_DIGITS 2 // same as my sigma

volatile uint32_t distance = 0;

inline void distance_on_pulse(const uint16_t now) {
  distance += PULSE_DIST;
  // TODO: asm this to use 1 tmp reg?
}

void distance_redraw(void) {
   upoint_t position;
   upoint_t glyph_size;

   position.x = 0; position.y = 5;
   glyph_size.x = 8;
   glyph_size.y = 8;
   print_number(distance >> FRAC_BITS, position, glyph_size, 1, NIBBLEPAIR(DIST_SIGNIFICANT_DIGITS, DIST_FRACTION_DIGITS));
}

module_actions_t *distance_select(const uint8_t state) {
   return NULL;
}

#define distance_record {&distance_redraw, &distance_select}
