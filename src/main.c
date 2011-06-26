#define F_CPU 1000000L
#include <avr/io.h>
#include <avr/sleep.h>
#include <math.h>

#include "common.h"

#include "values.h"

#ifdef ATTINY2313
  #include "avr/attiny2313.h"
#else
  #ifdef ATMEGA8
    #include "avr/atmega8.h"
  #endif
#endif

/* advanced options */
#define MAXBUFFERX 10 // used for drawing, defines maximum width of a character // TODO: move somewhere else
#define FRAC_BITS 10 // Fixed-point mantissa for PULSE_DIST and SPEED_FACTOR. WARNING! modifying may lead to overflow errors and better precision (most likely just the former). Less then 8 is not recommended if LONG_SPEED is used

/* imports depending on constants */
#include "display/pcd8544.h"
#include "display/drawing.h"

// basic builtins

#include "builtins/timer.h"
#ifdef CURRENT_SPEED
    #include "builtins/speed.h"
#endif

#include "builtins/plots.h"

// modules

#include "modules/base.h"

// debug builtin placed last to have access to all variables
#ifdef DEBUG
    #include "builtins/events.h"
#endif

#include "actions.h"

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

