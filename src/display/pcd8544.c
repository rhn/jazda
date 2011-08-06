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

#include <util/delay.h>
#include "pcd8544.h"

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
void lcd_setup() {
  RSTDIR |= 1 << RSTPIN;
  D_CDIR |= 1 << D_CPIN;
  CLKDIR |= 1 << CLKPIN;
  SCEDIR |= 1 << SCEPIN;
  SDADIR |= 1 << SDAPIN;
  LOW(RSTPORT, RSTPIN);
  LOW(D_CPORT, D_CPIN);
  LOW(CLKPORT, CLKPIN);
  LOW(SDAPORT, SDAPIN);
  LOW(SCEPORT, SCEPIN);
  _delay_ms(200);
  HIGH(RSTPORT, RSTPIN);
}

/* low-level function to send data to the chip. mostly used internally */
void send_raw_byte(const uint8_t payload, const uint8_t data) {
   uint8_t mask = 1 << 7;
   uint8_t i;
   for (i = 0; i < 8; i++) {
       if (mask & payload) {
           HIGH(SDAPORT, SDAPIN);
       } else {
           LOW(SDAPORT, SDAPIN);
       }
       
       if ((i == 1) && data) {
           HIGH(D_CPORT, D_CPIN);
       }
       HIGH(CLKPORT, CLKPIN);
       LOW(CLKPORT, CLKPIN);
       
       mask = mask >> 1;
   }
   LOW(SDAPORT, SDAPIN);
   LOW(D_CPORT, D_CPIN);
}

void set_column(const uint8_t col) {
    if (col < 84) {
        send_raw_byte(col | 0b10000000, false);
    }
}

void set_row(const uint8_t row) {
    if (row < 6) {
        send_raw_byte(row | 0b01000000, false);
    }
}

/* prepares the display for operation */
void lcd_init() {
  send_raw_byte(0b00100001, false); // extended instruction set
  send_raw_byte(0b10000000 | 66, false); // set voltage to just under 7V (3V+66*0.06V steps)
  send_raw_byte(0b00000100, false); // set temp coeff
  send_raw_byte(0b00010011, false); // set bias
  send_raw_byte(0b00100000, false); //poweron, horz addr, basic instruction set

  normal_screen();
}
