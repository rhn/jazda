#include "preferences.h"
#include "advopt.h"

/* GENERATED VALUES */

/* generates numeric data but also interrupt defines for the CPU header */

#define PULSE_DIST (uint64_t)((uint64_t)(METRIC_PULSE_DIST << FRAC_BITS) / 10000L) // TODO: power 10 ^ (6 - DIST_DIGITS) : 6 = mm->km

#ifdef DISTANCE
    // for CPU
    #define PULSEINT
    
    // numbers
    #define DIST_DIGITS NIBBLEPAIR(DIST_SIGNIFICANT_DIGITS, DIST_FRACTION_DIGITS)
#endif

#ifdef CURRENT_SPEED
    // defines for CPU
    #ifndef PULSEINT
        #define PULSEINT
    #endif
    #define MSTIMER
    
    // numbers
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
