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

#include "speed.h"
#include "wheel.h"
#include "../lib/timer.h"
#include "../lib/calculations.h"
#include "distance.h"

/* ---------- CONSTANTS --------------- */
// numbers
#define SPEED_DIGITS (number_display_t){.integer=SPEED_SIGNIFICANT_DIGITS, .fractional=SPEED_FRACTION_DIGITS}
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
#ifdef LONG_CALCULATIONS
    #define SPEED_FACTOR_t uint32_t
    #define get_speed_factor(pdist) ((SPEED_FACTOR_t)pdist * ONE_SECOND * 36 * 10) // T and N excluded as variable
#else
    #ifdef HIGH_PRECISION_CALCULATIONS
        #define SPEED_FACTOR_t uint32_t
        #define get_speed_factor(pdist) ((SPEED_FACTOR_t)pdist * ONE_SECOND * 36 * 10) // T and N excluded as variable
    #else
        #define SPEED_FACTOR_t uint16_t
        #define SPEED_TRUNCATION_BITS 1
        #define get_speed_factor(pdist) (((SPEED_FACTOR_t)pdist * 36 * 10) >> (FRAC_BITS - TIMER_BITS + SPEED_TRUNCATION_BITS)) // T and N excluded as variable
    #endif
#endif

#define INITIAL_SPEED_FACTOR (get_speed_factor(INITIAL_PULSE_DIST))

// reading from either wheel pulse or wheel stop event
volatile uint16_t speed_newest_reading;
// flag for notifying about pulse_table and oldest_pulse_index changes
volatile uint8_t speed_pulse_occured = true;

volatile int8_t speed_timer_handle = -1;

#ifdef CONSTANT_PULSE_DISTANCE
    const SPEED_FACTOR_t speed_factor = INITIAL_SPEED_FACTOR;
#else
    volatile SPEED_FACTOR_t speed_factor = INITIAL_SPEED_FACTOR;
#endif

#ifndef CONSTANT_PULSE_DISTANCE
    void speed_update_factor(void) {
        speed_factor = get_speed_factor(pulse_dist);
    }
#endif

#ifdef LONG_CALCULATIONS
    uint16_t get_average_speed_long(uint32_t time_amount, const uint16_t pulse_count) { 
       return get_rot_speed_long(speed_factor, time_amount, pulse_count);
    }
#endif

uint16_t get_average_speed(const uint16_t time_amount, const uint8_t pulse_count) {
    return get_rot_speed(speed_factor, time_amount, pulse_count);
}

void speed_on_timeout(void) {
   uint16_t now = get_time();
   // gradual speed decrease before stopping
   speed_newest_reading = now;
   speed_pulse_occured = true;
   
   speed_timer_handle = timer_set_callback(now + wheel_pulse_table[0] - wheel_pulse_table[1], &speed_on_timeout);
}

void speed_on_wheel_stop(void) {
    speed_pulse_occured = true;
    // no need to set "now" value: during redraw after a stop, speed will be 0 anyway
    
    timer_clear_callback(speed_timer_handle);
    speed_timer_handle = -1;
}

void speed_on_wheel_pulse(uint16_t now) {
  speed_newest_reading = now;
  speed_pulse_occured = true;
  timer_clear_callback(speed_timer_handle);
  
  if (wheel_pulse_count > 1) {
    // set timer for smooth slowdown. only set if 
    uint16_t predicted = now - wheel_pulse_table[1];
    uint16_t ahead = predicted + (predicted / 4);  
    // NOTE: remove ahead / 4 to save 10 bytes
    speed_timer_handle = timer_set_callback(now + ahead, &speed_on_timeout);
  }
}

// OBFUSCATION WARNING
void speed_redraw(void) {
   if (speed_pulse_occured) {
       speed_pulse_occured = false;
       uint16_t speed;
       upoint_t position = {50, 0};
       upoint_t glyph_size = {10, 16};
       
       if (wheel_pulse_count > 1) {
         // speed going down when no pulses present
         uint8_t pulse_index;
         uint16_t newest_pulse;
         uint16_t next_pulse;
         uint16_t oldest_pulse;
         
         do {
             speed_pulse_occured = false;
             newest_pulse = speed_newest_reading;
             next_pulse = wheel_pulse_table[0];
             pulse_index = wheel_pulse_count - 1;
             if (newest_pulse != next_pulse) { // if newest pulse is artificial
                oldest_pulse = wheel_pulse_table[pulse_index - 1];
             } else {
                oldest_pulse = wheel_pulse_table[pulse_index];
             }
         } while (speed_pulse_occured);
         uint8_t interval_count = pulse_index;
         uint16_t intervals_duration = newest_pulse - oldest_pulse;
        
         speed = get_average_speed(intervals_duration, interval_count);
         
       } else {
           speed = 0;
       }

       print_number(speed, position, glyph_size, 2, SPEED_DIGITS);
   }
}
