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

/* DISTANCE + CURRENT SPEED PROGRAM
distance stored in 10s of meters, fraction never displayed, fake decimal point
time stored in ? of s, fraction never displayed, fake decimal point

LOG
06.08 2010
    moved digit display functions to screen.h
    started adding current speed
    added ifdefs
08.08
    changed print_number to print integers only
    basic current speed display works
    optimized display speed calculation
09.08
    added speed update after braking
    added stop timeout
10.08
    added flexible vertical size for glyphs
    created common.h
14.08
    initial support for atmega8
18.06 2011
    moved files from examples to main part
    
TODO
- power usage
- move all function const to programspace
- graphical glitches (vert line after all drawing)
- separate configuration, code and defaults
- clean up code
- implement module interface
- hide ugly timers, interrupts and bit assumptions/optimizations etc to ./avr
- near-vertical lines
- fix low-precision speed calculation
*/

#include "modules/common.h"

#ifdef DISTANCE
    #include "modules/distance.h"
#endif

#ifdef CURRENT_SPEED
    #include "builtins/speed.h"
    #ifdef SPEED_VS_DISTANCE_PLOT
        #include "modules/svdplot.h"
    #endif
    #ifdef MAXSPEED
        #include "modules/maxspeed.h"
    #endif
#endif

#ifdef STOPWATCH
    #include "modules/stopwatch.h"
#endif

#ifdef AVGSPEED
    #include "modules/avgspeed.h"
#endif

#ifdef DEBUG
    #include "builtins/events.h"
#endif

#ifdef DEBUG
    #include "modules/debug.h"
#endif

// modules

#include "modules/base.h"

volatile module_actions_t *current_actions = &default_actions;

// TODO: if no function defined, do nothing. see if space is saved
void on_select_button(uint8_t state) {
   module_actions_t *actions = (*(modules[current_module].select_button))(state);
   if (actions == NULL) {
      current_actions = &default_actions;
   } else {
      current_actions = actions;
   }
}
// TODO: merge the 2 functions into 1 with 2 calls to it, using macro offsetof(struct, elem);
void on_right_button(uint8_t state) {
   (*(current_actions->button_right))(state);
}

void on_left_button(uint8_t state) {
   (*(current_actions->button_left))(state);
}

void module_redraw() {
   (*(modules[current_module].redraw))(current_module_switched);
   if (current_module_switched) {
      module_redraw_menu();
      current_module_switched = false;
   }
}

#include "emuint.h"

/* FUNCTIONS */

inline void on_pulse(void) {
// speed interrupt
  uint16_t now = get_time();
  #ifdef DISTANCE
    distance_on_pulse();
  #endif
  #ifdef CURRENT_SPEED
    speed_on_pulse(now);
  #endif
  #ifdef AVGSPEED
    avgspeed_on_pulse(now);
  #endif
}

inline void on_each_second(void) {
   #ifdef STOPWATCH
      stopwatch_on_each_second();
   #endif
}

inline void on_stop(uint16_t now) {
    
}

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

