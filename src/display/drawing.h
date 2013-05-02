/*
    Copyright 2011 rhn <gihu.rhn@porcupinefactory.org>
    Copyright 2013 Paweł Czaplejewicz

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
DEFINES: MAXBUFFERX: maximum allowed width of a character in a viewport. Wider characters can still be drawn if additional calls are used for further parts (TODO) Takes up MAXBUFFERX * 1B RAM
*/

#ifndef _DRAWING_H_
#define _DRAWING_H_
#include <avr/pgmspace.h>
#include <stdint.h>

#define MAXBUFFERX 10 // used for drawing, defines maximum width of a character

typedef struct glyph_point_struct { // watch out for ordering
    int y:4;
    int x:3;
    int cont:1;
} glyph_point_t;


#define GPOINT(x, y) {y, x, 1}
#define GSTOP(x, y) {y, x, 0}
#define GEND {0, 0, 0}
__attribute__((progmem)) const glyph_point_t drawing_glyphs[] = { GPOINT(2, 0), GPOINT(5, 0), GPOINT(7, 2), GPOINT(7, 13), GPOINT(5, 15), GPOINT(2, 15), GPOINT(0, 13), GPOINT(0, 2), GSTOP(2, 0), GEND, // 0
                           GPOINT(1, 4), GPOINT(5, 0), GPOINT(5, 15), GEND, // 1
                           GPOINT(0, 3), GPOINT(2, 0), GPOINT(5, 0), GPOINT(7, 3), GPOINT(0, 15), GSTOP(7, 15), GEND, // 2
                           GPOINT(0, 3), GPOINT(2, 0), GPOINT(5, 0), GPOINT(7, 2), GPOINT(4, 7), GPOINT(7, 9), GPOINT(7, 13), GPOINT(5, 15), GPOINT(2, 15), GSTOP(0, 12), GEND, // 3
                           GPOINT(7, 10), GPOINT(0, 10), GPOINT(5, 0), GSTOP(5, 15), GEND, // 4
                           GPOINT(7, 0), GPOINT(0, 0), GPOINT(0, 7), GPOINT(5, 7), GPOINT(7, 9), GPOINT(7, 13), GPOINT(5, 15), GSTOP(0, 15), GEND, // 5
                           GPOINT(7, 2), GPOINT(5, 0), GPOINT(2, 0), GPOINT(0, 3), GPOINT(0, 13), GPOINT(2, 15), GPOINT(5, 15), GPOINT(7, 13), GPOINT(7, 10), GPOINT(5, 8), GSTOP(0, 8), GEND, // 6
                           GPOINT(0, 0), GPOINT(7, 0), GSTOP(0, 15), GEND, // 7
                           GPOINT(2, 0), GPOINT(5, 0), GPOINT(7, 2), GPOINT(7, 5), GPOINT(0, 10), GPOINT(0, 13), GPOINT(2, 15), GPOINT(5, 15), GPOINT(7, 13), GPOINT(7, 10), GPOINT(0, 5), GPOINT(0, 2), GSTOP(2, 0), GEND, // 8
                           GPOINT(0, 15), GPOINT(5, 15), GPOINT(7, 12), GPOINT(7, 2), GPOINT(5, 0), GPOINT(2, 0), GPOINT(0, 2), GPOINT(0, 6), GPOINT(2, 8), GSTOP(7, 8), GEND, // 9
                           }; 

//digit table
const uint8_t drawing_digit[] = { 0, 10, 14, 21, 32, 37, 46, 58, 62, 76};


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
// glyph is in programspace
void draw_glyph(uint8_t *buffer, const glyph_point_t *glyph, const upoint_t glyph_size, const uint8_t width, const int8_t xoffset, const int8_t yoffset);

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
