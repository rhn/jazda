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

volatile uint8_t event_count = 0; // extern
extern volatile int8_t timer_first_index; // extern
volatile uint16_t event_value = 0;
void events_redraw(void) {
   upoint_t position = {0, 2};
   upoint_t glyph_size = {8, 8};
   print_number(event_value, position, glyph_size, 1, 4<<4);
   position.x = 48;
   uint8_t display;
   if (timer_first_index < 0) {
      display = 8;
   } else {
      display = timer_first_index;
   }
   print_number(display, position, glyph_size, 1, 4<<4);
}
