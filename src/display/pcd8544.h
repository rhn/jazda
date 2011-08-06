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

#include "../common.h"

/* Monochrome line-based display, each line is 8 bits high */
/* HARDWARE
 * Nokia 5110
 * #defines required:
   * pin/direction/port: (*PIN, *DIR, *PORT)
   RST - reset
   D_C - data/command
   CLK - serial clock
   SCE - select
   SDA - serial data
*/

/* prepares the chip for using the display */
void lcd_setup();

/* low-level function to send data to the chip. mostly used internally */
void send_raw_byte(const uint8_t payload, const uint8_t data);

#define blank_screen() send_raw_byte(0b00001000, false)
#define darken_screen() send_raw_byte(0b00001001, false)
#define normal_screen() send_raw_byte(0b00001100, false)
#define inverse_screen() send_raw_byte(0b00001101, false)

/* set cursor column in pixels - between 0 and 84. Out of bounds requests
ignored */
void set_column(const uint8_t col);

/* set cursor row in lines - between 0 and 6. Out of bounds requests ignored */
void set_row(const uint8_t row);

/* prepares the display for operation, but doesn't blank it. Artifacts may stay
on the screen. */
void lcd_init();
