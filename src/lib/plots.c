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

/* builtin managing plots
// TODO: vertical size. Currently hard-coded to 2 * 8
// TODO: speed axis adjusting to maximum value
// TODO: plot position. Currently hardcoded to use with modules
*/


#include "plots.h"

#define PLOT_LINE_HEIGHT 2

void circular_buffer_insert_value(circular_buffer_t *buffer, const uint8_t value) {
    (*buffer).values[(*buffer).next_index] = value;
    (*buffer).next_index++;
    if ((*buffer).num_values < PLOT_SIZE) {
      (*buffer).num_values++;
    }
    if ((*buffer).next_index == PLOT_SIZE) {
      (*buffer).next_index = 0;
    }
}

void draw_circular_buffer_plot_line(const circular_buffer_t buffer, const uint8_t line) {
  uint8_t maxheight = (PLOT_LINE_HEIGHT - line) * 8;
  
  // finds first valid frame number in the circular buffer
  int8_t current_frame = buffer.next_index - buffer.num_values;
  if (current_frame < 0) {
      current_frame = PLOT_SIZE + current_frame;
  }
  
  // prints the desired part of the plot (horizontally)
  for (uint8_t i = 0; i < buffer.num_values; i++)
  {
    uint8_t height = buffer.values[current_frame];
    current_frame++;
    if (current_frame == PLOT_SIZE) {
        current_frame = 0;
    }
 
    uint8_t bar = 0xFF;
    if (height < maxheight) {
      bar <<= maxheight - height;
    }
    send_raw_byte(bar, true);
  }
}
