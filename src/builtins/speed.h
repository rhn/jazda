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

// TODO: optimize: move 0 to variable on ts own and see if size decreases
// TODO: check if volatile is necessary with tables
volatile uint16_t pulse_table[PULSE_TABLE_SIZE + 1];
// index of the oldest pulse in the table
volatile uint8_t oldest_pulse_index = 0;

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

#ifdef LONG_SPEED
    /* use this to calculate averages over long periods of time/distances
    All this exists to avoid 64b / *b division which costs a whooping 3KB.
    TODO: investigate impact of placing >>FRAC_BITS in distance vs on return
    */
    uint16_t get_int_average_long(uint32_t time_amount, const uint16_t pulse_count) { 
       uint64_t distance;
       uint32_t speed;
       // SPEED_FACTOR is fixed point with FRAC_BITS fractional bits
       // pulse_count is integer
       // bits = big48b64b: big32b * big16b
       distance = ((uint64_t)SPEED_FACTOR) * pulse_count;
       
       // truncate values to be able to perform 32b division: find most
       // significant bit of distance and shift both time_amount and distance
       // to fit distance to 32b. time_amount can stay 32b
       
       // TODO: ASM optimization potential
       uint16_t mask = 0x8000; // most significant bit
       int8_t shift;
       // detect msb
       for (shift = 16; shift > 0; shift--) {
           if (mask & (distance >> 32)) {
              break;
           }
           mask >>= 1;
       }
       
       // TODO: possibly can be optimized
       time_amount >>= shift;
       distance >>= shift; 
       
       // distance is now 32b, time_amount is now 32b
       
       // bits = 32b: big32b / 32b
       speed = (uint32_t)distance / time_amount;

       // speed is fixed point with FRAC_BITS fractional bits
       
       return speed >> FRAC_BITS;
    }
    
    uint16_t get_int_average(const uint16_t time_amount, const uint8_t pulse_count) {
       return get_int_average_long(time_amount, pulse_count);
    }
#else
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
#endif

inline void speed_on_trigger(void) {
  uint16_t now = get_time();
  pulse_table[0] = now;
  // will sometimes be triggered with 1 pulse in table, but only after
  // STOPPED_TIMEOUT * ONE_SECOND have passed, never getting into the condition.
  // if you break it, BURN.
  // delay can be actually anything. No possibility of prediction. The following just looks good.
  if (now - pulse_table[1] < STOPPED_TIMEOUT * ONE_SECOND) {
    set_trigger_time(now + pulse_table[1] - pulse_table[2]);
  } else {
    clear_trigger();
    oldest_pulse_index = 0;
    on_stop(now);
  }
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
  set_trigger_time(now + ahead);

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
}

void speed_redraw() {
   uint16_t speed;
   upoint_t position = {0, 0};
   upoint_t glyph_size = {10, 16};

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

   print_number(speed, position, glyph_size, 2, SPEED_DIGITS);
}
