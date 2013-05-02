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

#define F_CPU 1000000L
#include <avr/io.h>
#include <avr/sleep.h>
#include <math.h>

#include "common.h"

#include "preferences.h"

#ifdef ATTINY2313
  #include "avr/attiny2313.c"
#else
  #ifdef ATMEGA8
    #include "avr/atmega8.c"
  #endif
#endif

/* imports depending on constants */
#include "display/pcd8544.c"
#include "display/bcd.c"
#include "display/drawing.c"


#include "lib/calculations.c"
#include "lib/timer.c"
#include "lib/plots.c"
#include "lib/modules.c"

// subsystems
#include "builtins/distance.c"
#ifdef STORAGE
    #include "builtins/storage.c"
#endif
#ifdef BACKLIGHT
    #include "dev/backlight.c"
#endif

// sensors (data/event capture)
#include "sensors/wheel.c"

#ifdef CRANK
    #include "sensors/crank.c"
#endif

// data display
#ifdef CADENCE
    #include "builtins/cadence.c"
#endif

#ifdef CURRENT_SPEED
    #include "builtins/speed.c"
#endif

// modules

#include "modules/base.c"

// debug builtin placed last to have access to all variables
#ifdef DEBUG
    #include "builtins/events.c"
#endif

#include "actions.c"
#include "signals.c" // must be included after its all callbacks

void main() __attribute__ ((noreturn));
void main(void) {
  setup_wheel_pulse();
  setup_buttons();
  setup_cpu();
  #ifdef STORAGE
      storage_setup();
      storage_load();
  #endif
  lcd_setup();
  lcd_init();
  
  #ifdef BACKLIGHT
      backlight_init();
  #endif
  #ifdef BACKLIGHT_VOLTAGE
     adc_init();
  #endif
  #ifdef LCD_CLEAN
     lcd_clean();
  #endif
  
  timer_initialize();

  // sleep enable bit
  MCUCR |= 1 << SE;
  // sleep mode
  MCUCR &= ~((1 << SM1) | (1 << SM0));

  sei();
  for (; ; ) {
    #ifdef CURRENT_SPEED
       speed_redraw();
    #endif
    #ifdef CADENCE
       cadence_redraw();
    #endif
    #ifdef DEBUG
       events_redraw();
    #endif
    module_redraw();
    nop_screen(); // FIXME: the screen doesn't immediately accept the last command

    /*
    send_raw_byte(emuintpinstate, true);
        send_raw_byte(emuintpinstate, true);
            send_raw_byte(emuintpinstate, true);
                send_raw_byte(emuintpinstate, true);*/
    sleep_mode();
  }
}

