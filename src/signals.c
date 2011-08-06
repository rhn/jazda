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

#include "signals.h"

/*
Signal dispatchers.
Must be included after modules to be able to reference them.
The only includes in this file should be standard data collectors callbacks.
*/

#include "builtins/wheel.h"

#ifdef CRANK
   #include "builtins/crank.h"
   
   inline void on_crank_pulse(void) {
      uint16_t now = get_time();
      on_crank_pulse_collect_data(now);
      #ifdef CADENCE
        cadence_on_crank_pulse();
      #endif
   }
   
   void on_crank_stop(void) {
      on_crank_stop_collect_data();
      #ifdef CADENCE
        cadence_on_crank_stop();
      #endif
   }
#endif

inline void on_wheel_pulse(void) {
// speed interrupt
  uint16_t now = get_time();
  on_wheel_pulse_collect_data(now);
  #ifdef DISTANCE
    distance_on_wheel_pulse();
  #endif
  #ifdef CURRENT_SPEED
    speed_on_wheel_pulse(now);
  #endif
  
  // Modules that need start pulse notification
  #ifdef SPEED_VS_DISTANCE_PLOT
    svd_on_wheel_pulse(now);
  #endif
  #ifdef AVGSPEED
    avgspeed_on_wheel_pulse();
  #endif
  #ifdef MAXSPEED
    maxspeed_on_wheel_pulse();
  #endif

  #ifdef SPEED_VS_TIME_PLOT
    svt_on_wheel_pulse(now);
  #endif
  #ifdef STOPWATCH
    stopwatch_on_wheel_pulse(now);
  #endif
}

inline void on_each_second(void) {
}

inline void on_wheel_stop(uint16_t now) {
    on_wheel_stop_collect_data();
    #ifdef CURRENT_SPEED
        speed_on_wheel_stop();
    #endif
    #ifdef SPEED_VS_TIME_PLOT
        svt_on_wheel_stop(now);
    #endif
    #ifdef STOPWATCH
        stopwatch_on_wheel_stop();
    #endif
}
