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

#define DIST_SIGNIFICANT_DIGITS 3 // 999km is good enough
#define DIST_FRACTION_DIGITS 2 // same as my sigma

volatile uint32_t distance = 0;

inline void distance_on_wheel_pulse(void) {
  distance += PULSE_DIST;
  // TODO: asm this to use 1 tmp reg?
  module_flags.distance_changed = true;
}

void distance_redraw(const uint8_t force) {
   if (module_flags.distance_changed || force) {
      upoint_t position = {0, 5};
      upoint_t glyph_size = {8, 8};
      module_flags.distance_changed = false;
      print_number(distance >> FRAC_BITS, position, glyph_size, 1, NIBBLEPAIR(DIST_SIGNIFICANT_DIGITS, DIST_FRACTION_DIGITS));
   }
}

void distance_reset(void) {
   distance = 0;
   module_flags.distance_changed = true;
}

module_actions_t *distance_select(const uint8_t state) {
   if (!state) {
      distance_reset();
   }
   return NULL;
}

#define distance_signature {0b01111110, 0b01000010, 0b00111100, 0, 0b01111110, 0, 0b01001100, 0b00110010}

#ifdef COMBINED_RESET
    #define distance_record {&distance_redraw, &combined_reset_on_select, distance_signature}
#else
    #define distance_record {&distance_redraw, &distance_select, distance_signature}
#endif
