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

/* Builtin calculating and displaying current speed.

HIGH_PRECISION_SPEED governs whether speed counter is 16 or 32-bit

pulse_table - a queue of pulse times (indices: 1..PULSE_TABLE_SIZE)
              index 1: the newest pulse
              index 0: last event (may be pulse or timer event)
              
oldest_pulse_index - current size of the queue

At each pulse, the new pulse is put on the queue and last element discarded if
queue full.
Speed is calculated between first and last pulse in queue on redraw. A timer
event is set some time after the predicted next pulse. If the pulse comes, timer
is set again. If the pulse doesn't come, current speed is lower than displayed.
Difference between event time and oldest pulse is base for updating speed.
Another trigger is set with same timeout until STOPPED_TIMEOUT from newest pulse
is exceeded.

SWITCHES:

HIGH_PRECISION_SPEED: if defined, speed is calculated in 32 bits

TODO: split away svd plot into a module
*/

/* ---------- PREFERENCES --------------- */


#define SPEED_SIGNIFICANT_DIGITS 2 // 99km/h is good enough
#define SPEED_FRACTION_DIGITS 1 // better than my sigma
#define PULSE_TABLE_SIZE 3
#define STOPPED_TIMEOUT 3 // seconds

/* ---------- CONSTANTS --------------- */
// numbers
#define SPEED_DIGITS NIBBLEPAIR(SPEED_SIGNIFICANT_DIGITS, SPEED_FRACTION_DIGITS)
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
    #define SPEED_FACTOR (uint32_t)(PULSE_DIST * ONE_SECOND * 36 * 10) // T and N excluded as variable
#else
    #ifdef HIGH_PRECISION_CALCULATIONS
        #define SPEED_FACTOR (uint32_t)(PULSE_DIST * ONE_SECOND * 36 * 10) // T and N excluded as variable
    #else
        #define SPEED_TRUNCATION_BITS 1
        #define SPEED_FACTOR (uint16_t)((PULSE_DIST * 36 * 10) >> (FRAC_BITS - TIMER_BITS + SPEED_TRUNCATION_BITS)) // T and N excluded as variable
    #endif
#endif

// TODO: optimize: move 0 to variable on ts own and see if size decreases
// TODO: check if volatile is necessary with tables
volatile uint16_t pulse_table[PULSE_TABLE_SIZE + 1];
// index of the oldest pulse in the table
volatile uint8_t oldest_pulse_index = 0;
// flag for notifying about pulse_table and oldest_pulse_index changes
volatile uint8_t speed_pulse_occured = true;

void on_stop(uint16_t now);

#ifdef SPEED_VS_DISTANCE_PLOT
    void svd_on_pulse(uint16_t now);
#endif

#ifdef MAXSPEED
    void maxspeed_on_pulse(void);
#endif

#ifdef AVGSPEED
    void avgspeed_on_pulse(void);
#endif

#ifdef LONG_CALCULATIONS
    uint16_t get_average_speed_long(uint32_t time_amount, const uint16_t pulse_count) { 
       return get_rot_speed_long(SPEED_FACTOR, time_amount, pulse_count);
    }
#endif

uint16_t get_average_speed(const uint16_t time_amount, const uint8_t pulse_count) {
    return get_rot_speed(SPEED_FACTOR, time_amount, pulse_count);
}

void speed_on_timeout(void);
void speed_on_timeout(void) {
  uint16_t now = get_time();
  // will sometimes be triggered with 1 pulse in table, but only after
  // STOPPED_TIMEOUT * ONE_SECOND have passed, never getting into the condition.
  // if you break it, BURN.
  // delay can be actually anything. No possibility of prediction. The following just looks good.
  if (now - pulse_table[1] < STOPPED_TIMEOUT * ONE_SECOND) {
    set_timer_callback(now + pulse_table[1] - pulse_table[2], &speed_on_timeout);
  } else {
    on_stop(now);
  }
}

void speed_on_stop(const uint16_t now) {
    oldest_pulse_index = 0;
    pulse_table[0] = now;
    speed_pulse_occured = true;
}

void speed_on_pulse(uint16_t now) {
  pulse_table[0] = now;

  for (uint8_t i = PULSE_TABLE_SIZE; i > 0; --i) {
    pulse_table[i] = pulse_table[i - 1];
  }

  uint16_t ahead;
  if (oldest_pulse_index == 0) {
    // 100 is added to make sure that on_trigger will detect this as a stop
    ahead = STOPPED_TIMEOUT * ONE_SECOND + 100;
  } else {
    uint16_t predicted = now - pulse_table[2];
    ahead = predicted + (predicted / 4);
  }
  // NOTE: remove ahead / 4 to save 10 bytes
  set_timer_callback(now + ahead, &speed_on_timeout);

  // Modules that need start pulse notification
  #ifdef SPEED_VS_DISTANCE_PLOT
    svd_on_pulse(now);
  #endif
  #ifdef AVGSPEED
    avgspeed_on_pulse();
  #endif
  if (oldest_pulse_index < PULSE_TABLE_SIZE) {
    oldest_pulse_index++;
  }
  #ifdef MAXSPEED
    maxspeed_on_pulse();
  #endif
  speed_pulse_occured = true;
}

// OBFUSCATION WARNING
void speed_redraw() {
   if (speed_pulse_occured) {
       speed_pulse_occured = false;
       uint16_t speed;
       upoint_t position = {0, 0};
       upoint_t glyph_size = {10, 16};
       
       if (oldest_pulse_index > 1) {
         // speed going down when no pulses present
         uint16_t *table = pulse_table;
         uint8_t pulse_index;
         uint16_t newest_pulse;
         uint16_t next_pulse;
         uint16_t oldest_pulse;
         
         do {
             speed_pulse_occured = false;
             newest_pulse = pulse_table[0];
             next_pulse = pulse_table[1];
             pulse_index = oldest_pulse_index;
             oldest_pulse = table[pulse_index];
         } while (speed_pulse_occured);
         
         if (newest_pulse != next_pulse) { // if newest pulse is artificial
           table -= 1; // pretend it is a real pulse by moving it to place 1
         }
         uint16_t pulse_time = newest_pulse - oldest_pulse;
        
         speed = get_average_speed(pulse_time, (pulse_index - 1));
         
       } else {
           speed = 0;
       }

       print_number(speed, position, glyph_size, 2, SPEED_DIGITS);
   }
}
