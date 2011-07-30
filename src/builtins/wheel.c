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

#include "wheel.h"
#include "../lib/timer.h"
#include "../modules/signals.h" // this file here implements on_wheel_stop detection

// TODO: check if volatile is necessary with tables
volatile uint16_t wheel_pulse_table[WHEEL_PULSE_TABLE_SIZE];
volatile uint8_t wheel_pulse_count = 0;
volatile int8_t wheel_timer_handle = -1;

void wheel_on_timeout(void) {
   uint16_t now = get_time();
   wheel_timer_handle = -1;
   on_wheel_stop(now);
}

void on_wheel_stop_collect_data(void) {
   wheel_pulse_count = 0;
}

void on_wheel_pulse_collect_data(const uint16_t now) {
  for (uint8_t i = WHEEL_PULSE_TABLE_SIZE - 1; i > 0; --i) {
    wheel_pulse_table[i] = wheel_pulse_table[i - 1];
  }
  wheel_pulse_table[0] = now;

  if (wheel_pulse_count < WHEEL_PULSE_TABLE_SIZE) {
    wheel_pulse_count++;
  }
  
  /* timeouts */
  // clear old timeout for stop detection (or do nothing if one not set)
  timer_clear_callback(wheel_timer_handle);
  // set new timeout for stop detection
  uint16_t ahead = WHEEL_STOPPED_TIMEOUT * ONE_SECOND;
  wheel_timer_handle = timer_set_callback(now + ahead, &wheel_on_timeout);
}
