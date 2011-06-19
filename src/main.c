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
#define FRAC_BITS 14 // WARNING! increasing may lead to overflow errors and better precision. TODO

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
- graphical glitches
- separate configuration, code and defaults
- clean up code
- implement module interface
- hide ugly timers, interrupts and bit assumptions/optimizations etc to ./avr
- near-vertical lines
- fix low-precision speed calculation
*/

#ifdef DISTANCE
    #include "modules/distance.h"
#endif

#ifdef CURRENT_SPEED
    #ifdef SPEED_VS_DISTANCE_PLOT
        volatile uint8_t svd_averages[SVDPLOT_SIZE]; // newer frames have higher number
        // speeds are truncated, 1 bit shift
        // circular buffer
        volatile uint8_t svd_next_average = 0; // index of the next average to be recorded
        volatile uint8_t svd_average_frames = 0; // number of recorded frames

        volatile uint8_t svd_pulse_number; // validity of the last pulse is checked by other means
        volatile uint16_t previous_frame_time; // the last recorded pulse time


        void svd_insert_average(const uint8_t speed) {
            svd_averages[svd_next_average] = speed;
            svd_next_average++;
            if (svd_average_frames < SVDPLOT_SIZE) {
              svd_average_frames++;
            }
            if (svd_next_average == SVDPLOT_SIZE) {
              svd_next_average = 0;
            }
        }
    #endif
    #include "builtins/speed.h"
#endif

#ifdef DEBUG
    #include "modules/debug.h"
#endif

/* DATA DECLARATIONS */
// modules

// currently displayed module
volatile uint8_t current_module = 0;

// table of module records
const module_record_t modules[] = {
    #ifdef DISTANCE
        {&distance_redraw},
    #endif
    #ifdef DEBUG
        {&debug_redraw}
    #endif
    };

#define MODULES_NUMBER sizeof(modules)/sizeof(module_record_t)


/* FUNCTIONS */

inline void on_pulse(void) {
// speed interrupt
   uint16_t now = get_time();
#ifdef DISTANCE
    distance_on_pulse(now);
#endif
#ifdef CURRENT_SPEED
    speed_on_pulse(now);
#endif
}

void on_select_button(uint8_t state) {
   return;
}

void on_right_button(uint8_t state) {
   if (current_module < MODULES_NUMBER - 1) {
       current_module++;
   } else {
       current_module = 0;
   }
}

void on_left_button(uint8_t state) {
   return;
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
/*     upoint_t position = {0, 2};
     upoint_t glyph_size = {8, 8};
     print_number((uint32_t)modules[current_module].redraw, position, glyph_size, 1, 5<<4);*/
    (*modules[current_module].redraw)();
    sleep_mode();
  }
}

