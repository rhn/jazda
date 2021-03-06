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

#include "drawing.h"
#include <stdlib.h>
/* Drawing on a monochrome line-based 8-bit height screen procedures */

/* REQUIRES:
pcd8544.h or compatible
common.h (upoint_t, nibblepair_t)
DEFINES: MAXBUFFERX: maximum allowed width of a character in a viewport. Wider characters can still be drawn if additional calls are used for further parts (TODO) Takes up MAXBUFFERX * 1B RAM
*/

// for asm routines
#define STRVALUE(arg) #arg
#define NUMSTR(num) STRVALUE(num)

#define LETTERX _BV(3)
#define LETTERY _BV(4)

#define FIXED_POINT_BITS 8

typedef union glyph_point_conv {
    uint8_t bin;
    glyph_point_t glyph_point;
} glyph_point_conv_t;

void draw_line(uint8_t *buffer, int8_t fromx, int8_t fromy, int8_t tox, int8_t toy, const int8_t width) {
// draws a line into the buffer
   uint8_t stamp = 0;
   int8_t i;
   int16_t incr;
   int16_t vert_position;
   if (fromx > tox) { // swap
       int8_t tmp;
       tmp = tox;
       tox = fromx;
       fromx = tmp;
       tmp = toy;
       toy = fromy;
       fromy = tmp;
   } else if (fromx == tox) {
       int8_t tmp;
       if (fromy > toy) {
           tmp = toy;
           toy = fromy;
           fromy = tmp;
       }
       for (i = fromy; i < toy + width; i++) {
           stamp |= 1 << i;
       }
       for (i = fromx; i < fromx + width; i++) {
           if ((i >= 0) && (i < MAXBUFFERX)) {
               buffer[i] |= stamp;
           }
       }    
       return;
   }
   incr = ((toy - fromy) << FIXED_POINT_BITS) / (uint8_t)(tox - fromx); // tox-fromx guaranteed positive
   for (i = 0; i < width; i++) { // LSBit is x=0, that is upper row
       stamp <<= 1;
       stamp |= 1;
   }
   vert_position = fromy << FIXED_POINT_BITS; // fixed-point, might be negative
   // from starting point to ending point + line width
   // NOTE: the first stamp is placed when whole stamp is inside viewport (vert_position >= 0). For wide lines this might cause disconnects on upper viewport boundary. It saves a lot of program space though
   for (i = fromx; i < tox + width; i++, vert_position += incr) {
       if ((i >= 0) && (i < MAXBUFFERX) && (vert_position >= 0)) {
           buffer[i] |= (uint8_t)(stamp << (uint8_t)(vert_position >> FIXED_POINT_BITS));
       }
   }
}


void scale(point_t *point, const upoint_t pxsize) {
// Scales point inside a character assuming that new character size is pxsize
   int16_t tmp;
   tmp = (*point).x * pxsize.x;
   (*point).x = tmp / LETTERX;
   tmp = (*point).y * pxsize.y;
   (*point).y = tmp / LETTERY;
}

point_t read_point(const glyph_point_t *glyph_point) {
    glyph_point_conv_t conv;
    point_t ret;
    conv.bin = pgm_read_byte(glyph_point);
    ret.y = conv.bin & 0b00001111;
    ret.x = (conv.bin >> 4) & 0b00000111;
    return ret;
}

void draw_glyph(uint8_t *buffer, const glyph_point_t *glyph, const upoint_t glyph_size, const uint8_t width, const int8_t xoffset, const int8_t yoffset) {
// user must take care that characters have proper line width 
   point_t prev;
   point_t next;
   upoint_t pxsize = {glyph_size.x - width + 1, glyph_size.y - width + 1};
   int8_t i;

   prev = read_point(&(glyph[0]));
   
   prev.x -= xoffset;
   prev.y -= yoffset;
   scale(&prev, pxsize);
   for (i = 1; pgm_read_byte(&(glyph[i])); i++) {
      next = read_point(&(glyph[i]));
      next.x -= xoffset;
      next.y -= yoffset;
      scale(&next, pxsize);

      draw_line(buffer, prev.x, prev.y, next.x, next.y, width);
      prev = next;
   }
}

void print_digit(const uint8_t digit, const upoint_t glyph_size, const uint8_t width, upoint_t position) {
   // TODO: draw into viewport
   uint8_t buffer [MAXBUFFERX]; // for drawing characters
   const glyph_point_t *glyph = drawing_glyphs + drawing_digit[digit];
   uint8_t line;
   int8_t i;
   
   for (line = 0; line <= (glyph_size.y - 1) / 8; line++) {
       for (i = 0; i < glyph_size.x; i++) { // commented out because GCC disabled interrupts and wrote to reserved space...
          buffer[i] = 0;
       }
              /*  asm volatile("movw r30, %A0" "\n"
            "adiw r30, " NUMSTR(MAXBUFFERX) "\n" // constraint M!!!
            "eor __tmp_reg__, __tmp_reg__" "\n"
    "zero%=: st -Z, __tmp_reg__" "\n"
            "cp r30, %A0" "\n"
            "brne zero%=" "\n"
            "cp r31, %B0" "\n"
            "brne zero%=" "\n" : : "r" (buffer) : "r30", "r31");
    */
       if (digit < 10) {
           draw_glyph(buffer, glyph, glyph_size, width, 0, line * 8);    
       }
       set_column(position.x);
       set_row(position.y + line);
       for (i = 0; i < glyph_size.x; i++) {
/*           if (i == 0) {
            send_raw_byte(buffer[i], true);
           }*/
           send_raw_byte(buffer[i], true);
       }
   }
}

/* bin: raw number to display
   position: row, column
   glyph_size: size in pixels
   width: line width of the glyph
   digits: total digits (to be spaced), fractional part digits
*/
void print_number(uint32_t bin, upoint_t position, const upoint_t glyph_size, const uint8_t width, const number_display_t digits) {
    
// prints a number, aligned to right, throwing in 0's when necessary
 // TODO: fake decimal point?
// TODO: move to 16-bit (ifdef 32?)
    // integer only
    uint8_t frac_digits = digits.fractional;
    uint8_t all_digits = digits.integer + frac_digits;

    bcd32_t bcd = int_to_bcd32(bin);

    uint8_t tmp; // should be bcd_nibble_t

    for (int8_t i = 7; i >= 0; i--) {
        if (i >= all_digits) {
            continue;
        } // HACK: gcc likes it better size-wise than i = min(all_digits, 8); (approx. 50
        tmp = ((uint8_t*)bcd.nibbles)[i >> 1];
        // iterate tmp over BCD chars
        if (i & 1) {
            tmp = tmp >> 4; // should be tmp = nibble.high, but gcc tries that using shifts 
        } else {
            tmp = tmp & 0x0F; // tmp = nibble.low
        }
        
        if (tmp || (i <= frac_digits)) {
            // either real digits to display or padding 0
            print_digit(tmp, glyph_size, width, position);
        }

        position.x += glyph_size.x + width;
    }
}
