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

#define CLKDIR DDRB
#define CLKPORT PORTB
#define CLKPIN PB1

#define RSTDIR DDRB
#define RSTPORT PORTB
#define RSTPIN PB2

#define SCEDIR DDRB
#define SCEPORT PORTB
#define SCEPIN PB3

#define SDADIR DDRB
#define SDAPORT PORTB
#define SDAPIN PB4

#define D_CDIR DDRB
#define D_CPORT PORTB
#define D_CPIN PB5

#include "avr.h"

void setup_cpu(void) {
// makes CPU run at 1 MHz
// clock prescaler, device clock must be 8 MHz
  clock_prescale_set(clock_div_8);
}

