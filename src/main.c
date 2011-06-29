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

#define F_CPU 1000000L
#include <avr/io.h>
#include <avr/sleep.h>
#include <math.h>

#include "common.h"

#include "values.h"

#ifdef ATTINY2313
  #include "avr/attiny2313.c"
#else
  #ifdef ATMEGA8
    #include "avr/atmega8.c"
  #endif
#endif

/* advanced options */
#define MAXBUFFERX 10 // used for drawing, defines maximum width of a character // TODO: move somewhere else
#define FRAC_BITS 10 // Fixed-point fraction for PULSE_DIST and SPEED_FACTOR. WARNING! modifying may lead to overflow errors and better precision (most likely just the former). Less then 8 is not recommended if LONG_SPEED is used

/* imports depending on constants */
#include "display/pcd8544.c"
#include "display/drawing.c"

// basic builtins

#include "builtins/timer.c"
#ifdef CURRENT_SPEED
    #include "builtins/speed.c"
#endif

#include "builtins/plots.c"

// modules

#include "modules/base.c"

// debug builtin placed last to have access to all variables
#ifdef DEBUG
    #include "builtins/events.c"
#endif

#include "actions.c"

void main() __attribute__ ((noreturn));
void main(void) {
  setup_pulse();
  setup_buttons();
  setup_cpu();
  lcd_setup();
  lcd_init();
  setup_timer();

  // sleep enable bit
  MCUCR |= 1 << SE;
  // sleep mode
  MCUCR &= ~((1 << SM1) | (1 << SM0));

  sei();
  for (; ; ) {
    #ifdef CURRENT_SPEED
       speed_redraw();
    #endif
    #ifdef DEBUG
       events_redraw();
    #endif
    module_redraw();
    #ifdef DEBUG
        send_raw_byte(0, true);
    #endif
    /*
    send_raw_byte(emuintpinstate, true);
        send_raw_byte(emuintpinstate, true);
            send_raw_byte(emuintpinstate, true);
                send_raw_byte(emuintpinstate, true);*/
    sleep_mode();
  }
}

