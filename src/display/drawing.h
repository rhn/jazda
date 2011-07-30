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

/* Drawing on a monochrome line-based 8-bit height screen procedures */

/* REQUIRES:
pcd8544.h or compatible
common.h (upoint_t, nibblepair_t)
DEFINES: MAXBUFFERX: maximum allowed width of a character in a viewport. Wider characters can still be drawn if additional calls are used for further parts (TODO) Takes up MAXBUFFERX * 1B RAM
*/

#ifndef _DRAWING_H_
#define _DRAWING_H_

typedef struct unsigned_point {
    uint8_t x;
    uint8_t y;
} upoint_t;

typedef struct signed_point {
    int8_t x;
    int8_t y;
} point_t;

typedef struct glyph_desc {
    upoint_t size;
    uint8_t line_width;
} glyphdesc_t;

typedef struct number_display {
    uint8_t integer:4;
    uint8_t fractional:4;
} number_display_t;

// draws a line into the buffer
void draw_line(uint8_t *buffer, int8_t fromx, int8_t fromy, int8_t tox, int8_t toy, const int8_t width);

// user must take care that characters have proper line width
void draw_glyph(uint8_t *buffer, const uint8_t *glyph, const upoint_t glyph_size, const uint8_t width, const int8_t xoffset, const int8_t yoffset);

void print_digit(const uint8_t digit, const upoint_t glyph_size, const uint8_t width, upoint_t position);

/* bin: raw number to display
   position: row, column
   glyph_size: size in pixels
   width: line width of the glyph
   digits: total digits (to be spaced), fractional part digits
*/
void print_number(uint32_t bin, upoint_t position, const upoint_t glyph_size, const uint8_t width, const number_display_t digits);
// prints a number, aligned to right, throwing in 0's when necessary

#endif
