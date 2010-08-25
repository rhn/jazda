#define F_CPU 1000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <math.h>

#include "common.h"
#ifdef ATTINY2313
  #include "avr/attiny2313.h"
#else
  #ifdef ATMEGA8
    #include "avr/atmega8.h"
  #endif
#endif

#include "preferences.h"


/* advanced options */
#define MAXBUFFERX 10
#define FRAC_BITS 14

/* imports depending on constants */

#include "screen.h"

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
    
TODO
- hide ugly timers, interrupts and bit assumptions/optimizations etc to ./avr
- separate screen and drawing to buffer
- near-vertical lines
- fix low-precision speed calculation
*/

/* ADVANCED OPTIONS */

#ifdef CURRENT_SPEED
    #define SPEED_SIGNIFICANT_DIGITS 2 // 99km/h is good enough
    #define SPEED_FRACTION_DIGITS 1 // better than my sigma
    #define PULSE_TABLE_SIZE 3
    #define STOPPED_TIMEOUT 3 // seconds
#endif

#ifdef DISTANCE
    #define DIST_SIGNIFICANT_DIGITS 3 // 999km is good enough
    #define DIST_FRACTION_DIGITS 2 // same as my sigma
#endif

#ifdef SPEED_VS_DISTANCE_PLOT
    // TODO: compile-time notification that CURRENT_SPEED is required
    // TODO: vertical size. Currently hard-coded to 2 * 8
    // TODO: speed axis adjusting to maximum value
    #define SVDPLOT_SIZE 84 // size in pixels (distance axis)
    #define SVDPLOT_LENGTH_KM 4 // distance axis length
#endif

/* GENERATED VALUES */
#define PULSE_DIST (uint64_t)((uint64_t)(METRIC_PULSE_DIST << FRAC_BITS) / 10000L) // TODO: power 10 ^ (6 - DIST_DIGITS) : 6 = mm->km

#ifdef DISTANCE
    #define DIST_DIGITS NIBBLEPAIR(DIST_SIGNIFICANT_DIGITS, DIST_FRACTION_DIGITS)
#endif

#ifdef CURRENT_SPEED
    #define SPEED_DIGITS NIBBLEPAIR(SPEED_SIGNIFICANT_DIGITS, SPEED_FRACTION_DIGITS)
    #define ONE_SECOND 986 // in timer ticks // TODO: autocalculate
/* Speed calculations:
  X - rotation distance in internal units
  N - rotation count
  T - rotations time in internal units
  L - internal time units making one second
  Y - speed in internal units
  a - decimal point in distance
  b - decimal point in speed
  
  General:
  \frac{NX}{10^{a}}\cdot\frac{km}{Ts\frac{1}{L}}=\frac{Y}{10^{b}}\cdot\frac{km}{h}

  Solution:
  Y=\frac{NL}{T}36\cdot10^{b-a+2}X

  */
  // TODO: power 10 ** (SPEED_DIGITS - DIST_DIGITS + 2)
    #ifdef HIGH_PRECISION_SPEED
        #define SPEED_FACTOR PULSE_DIST * ONE_SECOND * 36 * 10 // T and N excluded as variable
    #else
        #define SPEED_TRUNCATION_BITS 1
        #define SPEED_FACTOR (uint16_t)((PULSE_DIST * 36 * 10) >> (FRAC_BITS - TIMER_BITS + SPEED_TRUNCATION_BITS)) // T and N excluded as variable
    #endif
#endif

#ifdef SPEED_VS_DISTANCE_PLOT
    #define SVDPLOT_SPEED_TRUNC 5
    #define SVDPLOT_FRAME_PULSES (SVDPLOT_LENGTH_KM * 1000000L) / (SVDPLOT_SIZE * METRIC_PULSE_DIST)
#endif

/* DATA DECLARATIONS */
#ifdef DISTANCE
    volatile uint32_t distance = 0;
#endif

#ifdef CURRENT_SPEED
    // time containing pulse times [1:]
    // 1 is the newest pulse
    // 0 contains newest time to be used for display purposes, thus +1
    // TODO: optimize: move 0 to variable on ts own and see if size decreases
    // TODO: check if volatile is necessary with tables
    volatile uint16_t pulse_table[PULSE_TABLE_SIZE + 1];
    // index of the oldest pulse in the table
    volatile uint8_t oldest_pulse_index = 0;
#endif

#ifdef SPEED_VS_DISTANCE_PLOT
    volatile uint8_t svd_averages[SVDPLOT_SIZE]; // newer frames have higher number
    // speeds are truncated, 1 bit shift
    // circular buffer
    volatile uint8_t svd_next_average = 0; // index of the next average to be recorded
    volatile uint8_t svd_average_frames = 0; // number of recorded frames

    volatile uint8_t svd_pulse_number; // validity of the last pulse is checked by other means
    volatile uint16_t previous_frame_time; // the last recorded pulse time
#endif

/* FUNCTIONS */

#ifdef CURRENT_SPEED
    uint16_t get_int_average(const uint16_t time_amount, const uint8_t pulse_count) {
       uint16_t speed;
       // SPEED_FACTOR is fixed point with FRAC_BITS fractional bits
       // pulse_time is integer
       speed = ((uint32_t)SPEED_FACTOR * pulse_count) / time_amount;
       // speed is fixed point with FRAC_BITS fractional bits
       speed >>= FRAC_BITS;
       return speed;
    }
#endif

#ifdef SPEED_VS_DISTANCE_PLOT
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

void print_number(uint32_t bin, upoint_t position, const upoint_t glyph_size, const uint8_t width, const nibblepair_t digits) {
// prints a number, aligned to right, throwing in 0's when necessary
 // TODO: fake decimal point?
// TODO: move to 16-bit (ifdef 32?)
    // integer only
    uint32_t bcd = 0;
    register uint8_t *ptr;
    uint8_t frac_digits = digits & 0x0F;
    uint8_t all_digits = (digits >> 4) + frac_digits;

    for (int8_t i = 31; ; i--) { //32 should be size of int - FRACBITS
        asm("lsl	%A0" "\n"
	        "rol	%B0" "\n"
	        "rol	%C0" "\n"
	        "rol	%D0" "\n"
	        "rol	%A1" "\n" // reduce this shit
	        "rol	%B1" "\n"
	        "rol	%C1" "\n"
	        "rol	%D1" "\n" : "+r" (bin), "+r" (bcd));
	    if (i == 0) {
	        break;
	    }
	    
	    // WARNING: Endianness
        // while (ptr > &bcd)
        for (ptr = ((uint8_t*)&bcd) + 3; ptr >= ((uint8_t*)&bcd); ptr--) { //ptr points to MSB
        // roll two parts into one to save space. example below with printing
//            print_digit(ptr - (uint8_t*)&bcd, 8, 8, 1);
            if (((*ptr) + 0x03) & _BV(3)) {
                *ptr += 0x03;
            }
            if (((*ptr) + 0x30) & _BV(7)) {
                *ptr += 0x30;
            }
        }
        /*
        uint8_t tmp;
        asm("mov r30, %3" "\n"
            "inc r30" "\n"
"incloops%=: ld __tmp_reg__, -Z" "\n" // Z!!!
            "ldi %2, 0x03" "\n"
            "add __tmp_reg__, %2" "\n"
            "sbrc __tmp_reg__, 3" "\n"
            "st Z, __tmp_reg__" "\n"
            "ld __tmp_reg__, -Z" "\n" // Z!!!
            "ldi %2, 0x30" "\n"
            "add __tmp_reg__, %2" "\n"
            "sbrc __tmp_reg__, 7" "\n"
            "st Z, __tmp_reg__" "\n"
            "cp r30, %3" "\n"
            "brne incloops%=" "\n" : "+r" (bcd), "+z" (ptr), "=d" (tmp) : "x" (resptr)); */
	}    
 
    uint8_t tmp;
    uint8_t print = 0; // 0: don't print
                       // 1: leave space
                       // 2: print
    
    ptr = ((uint8_t*)&bcd) + 3;
    
    for (uint8_t i = 8; i > 0; i--) {
        if (i & 1) {
            tmp = (*ptr) & 0x0F;
            ptr--;
        } else {
            tmp = (*ptr) >> 4;
        }
        if (tmp) {
            print = 2;
        }
        if (i == frac_digits + 1) { // a pre-point number hit
            print = 2;
        }
        if ((i <= all_digits) && (print < 2)) {
            print = 1;
            tmp = 10;
        }
        if (print) {
            print_digit(tmp, glyph_size, width, position);
            position.x += glyph_size.x + width;
        }
    } // 1452 bytes
    /*
    for (ptr = ((uint8_t*)&bcd) + 3; ptr >= ((uint8_t*)&bcd); ptr--) { //ptr points to MSB
        tmp = *ptr;
        if (tmp) {
            print = true;
        }
        if (print) {
            if (tmp >> 4) {
                print_digit(tmp >> 4, 8, 8, 1);
            }
            if (tmp & 0x0F) {
                print_digit(tmp & 0x0F, 8, 8, 1);
            }
        }
    } 1474 bytes*/
}

ISR(TIMER1_COMPA_vect) {
  uint16_t now = get_time();
  pulse_table[0] = now;
  // will never be triggered with 1 pulse in table
  // delay can be actually anything. No possibility of prediction. The following just looks good.
  if (now - pulse_table[1] < STOPPED_TIMEOUT * ONE_SECOND) {
    set_trigger_time(now +  pulse_table[1] - pulse_table[2]);
  } else {
    oldest_pulse_index = 0;
  }
}

ISR(INT0_vect) {
// speed interrupt
#ifdef DISTANCE
  distance += PULSE_DIST;
  // TODO: asm this to use 1 tmp reg?
#endif
#ifdef CURRENT_SPEED
  uint16_t now = get_time();
  pulse_table[0] = now;

  for (uint8_t i = PULSE_TABLE_SIZE; i > 0; --i) {
    pulse_table[i] = pulse_table[i - 1];
  }

  if (oldest_pulse_index > 1) {
    uint16_t ahead = now - pulse_table[2];
    // NOTE: remove ahead / 4 to save 10 bytes
    set_trigger_time(now + ahead + (ahead / 4));
  }

  #ifdef SPEED_VS_DISTANCE_PLOT
    if (oldest_pulse_index == 0) { // if first pulse after a stop
        previous_frame_time = now; // set a new start time
        svd_pulse_number = 0; // clear counter
        svd_insert_average(0); // insert a space
    } else if (svd_pulse_number == SVDPLOT_FRAME_PULSES - 1) { // if last pulse of a count
        uint16_t avg = get_int_average(now - previous_frame_time, SVDPLOT_FRAME_PULSES); // TODO: move away to main loop
        previous_frame_time = now;
        svd_pulse_number = 0; // clear counter
        
        
        // TRUNCATING AVERAGE
        avg >>= SVDPLOT_SPEED_TRUNC;
        
        // ROUNDING AVERAGE
        /*avg >>= SVDPLOT_SPEED_TRUNC - 1;
        if (avg & 1) {
          avg |= 0x00000010; // avg++; works better for longer types
        }
        avg >>= 1;*/
        
        svd_insert_average(avg);
    } else {
        svd_pulse_number++;
    }
  #endif

  if (oldest_pulse_index < PULSE_TABLE_SIZE) {
    oldest_pulse_index++;
  }
#endif
}

void main() __attribute__ ((noreturn));
void main(void) {
  setup_pulse();
  #ifdef DEBUG
   uint8_t loops;
   PORTB &= ~(1 << PB3);
   while (!(PINB & 1<<PB3)) {
       lcd_setup();
       lcd_init();
       for (uint8_t i = 0; i < 4; i++) {
           for (loops = 1; loops != 0; loops++) {
               send_raw_byte(loops, true);
           }
       }
   }
   loops = 0;
  #endif
  setup_cpu();
  lcd_setup();
  lcd_init();
  setup_timer();
/*  for (uint8_t i = 0; ; i++) {
    send_raw_byte(i, true);
  }*/
  // sleep enable bit
  MCUCR |= 1 << SE;
  // sleep mode
  MCUCR &= ~((1 << SM1) | (1 << SM0));
  
  upoint_t position;
  upoint_t glyph_size;
  sei();
  for (; ; ) {
    glyph_size.x = glyph_size.y = 8;
    #ifdef DISTANCE
       position.x = 0; position.y = 5;
       print_number(distance >> FRAC_BITS, position, glyph_size, 1, DIST_DIGITS);
    #endif
    #ifdef CURRENT_SPEED
       uint16_t speed;
       if (oldest_pulse_index > 1) {
         // speed going down when no pulses present
         uint16_t newest_pulse = pulse_table[0];
         uint16_t *table = pulse_table;
         if (newest_pulse != pulse_table[1]) { // if pulse 0 is artificial
           table -= 1; // pretend it is a real pulse by moving it to place 1
         }
         uint16_t pulse_time = newest_pulse - table[oldest_pulse_index];
         
         #ifdef HIGH_PRECISION_SPEED
           speed = get_int_average(pulse_time, (oldest_pulse_index - 1));
         #else
           // SPEED_FACTOR is fixed point with TIMER_BITS fractional bits truncated by SPEED_TRUNCATION_BITS bits
           // pulse_time is fixed point with TIMER_BITS fractional bits
           // to get correct division, pulse_time needs to be truncated by SPEED_TRUNCATION_BITS
           speed = ((uint16_t)(SPEED_FACTOR * (oldest_pulse_index - 1))) / (pulse_time >> SPEED_TRUNCATION_BITS);
           // calculation error: 1% at 30 km/h and proportional to square of speed
         #endif
       } else {
           speed = 0;
       }
       position.x = 0; position.y = 0;
       glyph_size.x = 10;
       glyph_size.y *= 2;
       print_number(speed, position, glyph_size, 2, SPEED_DIGITS);
       
       #ifdef SPEED_VS_DISTANCE_PLOT
         if (svd_pulse_number == 0) { // there's been a change, redraw
           for (uint8_t line = 0; line < 2; line++) { // XXX: lines
             uint8_t maxheight = (2 - line) * 8; // XXX: lines
             set_column(84 - svd_average_frames);
             set_row(line + 2);
             int8_t current_frame = svd_next_average - svd_average_frames;
             if (current_frame < 0) {
                 current_frame = SVDPLOT_SIZE + current_frame;
             }
             
             for (uint8_t i = 0; i < svd_average_frames; i++)
             {
               uint8_t height = svd_averages[current_frame];
               current_frame++;
               if (current_frame == SVDPLOT_SIZE) {
                   current_frame = 0;
               }

               uint8_t bar = 0xFF;
               if (height < maxheight) {
                 bar <<= maxheight - height;
               }
               send_raw_byte(bar, true);
             }
           }
         }
       #endif
    #endif
    #ifdef DEBUG
     position.x = 40; position.y = 0;
     glyph_size.y /= 2;
     print_number(loops++, position, glyph_size, 1, NIBBLEPAIR(3, 0));
    #endif
    sleep_mode();
  }
}

