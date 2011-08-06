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

/* Crank builtin. Performs data collection only
*/

#include "crank.h"
#include "../lib/timer.h"
#include "../signals.h"

volatile uint16_t crank_pulse_times[CRANK_PULSES]; // extern
volatile uint8_t crank_pulse_count = 0; // extern
volatile timer_handle_t crank_timer_handle = -1;

void crank_on_timeout(void) {
   crank_timer_handle = -1;
   on_crank_stop();
}

void on_crank_stop_collect_data(void) {
   crank_pulse_count = 0;
}

void on_crank_pulse_collect_data(uint16_t now) {
  // shift the whole array of pulses towards the old end
  for (uint8_t i = CRANK_PULSES - 1; i > 0; --i) {
    crank_pulse_times[i] = crank_pulse_times[i - 1];
  }
  // insert the current time in the beginning
  crank_pulse_times[0] = now;
  
  // increment pulse counter
  if (crank_pulse_count < CRANK_PULSES) {
    crank_pulse_count++;
  }

  /* timeouts */
  // clear old timeout for stop detection (or do nothing if one not set)
  timer_clear_callback(crank_timer_handle);
  // set new timeout for stop detection
  uint16_t ahead = CRANK_STOPPED_TIMEOUT;
  crank_timer_handle = timer_set_callback(now + ahead, &crank_on_timeout); 
}
