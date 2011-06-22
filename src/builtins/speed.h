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
#ifdef HIGH_PRECISION_SPEED
    #define SPEED_FACTOR (uint32_t)(PULSE_DIST * ONE_SECOND * 36 * 10) // T and N excluded as variable
#else
    #define SPEED_TRUNCATION_BITS 1
    #define SPEED_FACTOR (uint16_t)((PULSE_DIST * 36 * 10) >> (FRAC_BITS - TIMER_BITS + SPEED_TRUNCATION_BITS)) // T and N excluded as variable
#endif

#ifdef SPEED_VS_DISTANCE_PLOT
    #define SVDPLOT_SPEED_TRUNC 5
    #define SVDPLOT_FRAME_PULSES (SVDPLOT_LENGTH_KM * 1000000L) / (SVDPLOT_SIZE * METRIC_PULSE_DIST)
#endif


// TODO: optimize: move 0 to variable on ts own and see if size decreases
// TODO: check if volatile is necessary with tables
volatile uint16_t pulse_table[PULSE_TABLE_SIZE + 1];
// index of the oldest pulse in the table
volatile uint8_t oldest_pulse_index = 0;

#ifdef HIGH_PRECISION_SPEED
    uint16_t get_int_average(const uint16_t time_amount, const uint8_t pulse_count) {
       uint32_t speed;
       // SPEED_FACTOR is fixed point with FRAC_BITS fractional bits
       // pulse_count is integer
       // bits = small32b: ((32+b: big32b * small8b) / 16b)
       speed = ((uint32_t)(SPEED_FACTOR * pulse_count)) / time_amount;

       // speed is fixed point with FRAC_BITS fractional bits
       // bits = 16b: small32b >> ~10
       speed >>= FRAC_BITS;
       return speed;
    }
#else
    uint16_t get_int_average(const uint16_t time_amount, const uint8_t pulse_count) {
       // SPEED_FACTOR is fixed point with TIMER_BITS fractional bits truncated by SPEED_TRUNCATION_BITS bits
       // pulse_time is fixed point with TIMER_BITS fractional bits
       // to get correct division, pulse_time needs to be truncated by SPEED_TRUNCATION_BITS
       return ((uint16_t)(SPEED_FACTOR * (oldest_pulse_index - 1))) / (pulse_time >> SPEED_TRUNCATION_BITS);
       // calculation error: 1% at 30 km/h and proportional to square of speed
    }
#endif

inline void speed_on_trigger(void) {
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

void speed_on_pulse(uint16_t now) {
  pulse_table[0] = now;

  for (uint8_t i = PULSE_TABLE_SIZE; i > 0; --i) {
    pulse_table[i] = pulse_table[i - 1];
  }

  if (oldest_pulse_index > 1) {
    uint16_t ahead = now - pulse_table[2];
    // NOTE: remove ahead / 4 to save 10 bytes
    set_trigger_time(now + ahead + (ahead / 4));
  }

  if (oldest_pulse_index < PULSE_TABLE_SIZE) {
    oldest_pulse_index++;
  }
}

void speed_redraw() {
   uint16_t speed;
   upoint_t position;
   upoint_t glyph_size;

   position.x = 0;
   glyph_size.x = 10;

   if (oldest_pulse_index > 1) {
     // speed going down when no pulses present
     uint16_t newest_pulse = pulse_table[0];
     uint16_t *table = pulse_table;
     if (newest_pulse != pulse_table[1]) { // if pulse 0 is artificial
       table -= 1; // pretend it is a real pulse by moving it to place 1
     }
     uint16_t pulse_time = newest_pulse - table[oldest_pulse_index];
    
     speed = get_int_average(pulse_time, (oldest_pulse_index - 1));
   } else {
       speed = 0;
   }
   position.y = 0;
   glyph_size.y = 16;
   print_number(speed, position, glyph_size, 2, SPEED_DIGITS);
}
