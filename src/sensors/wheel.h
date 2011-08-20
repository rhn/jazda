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

/* Builtin managing wheel information

pulse_table - a queue of pulse times (indices: 1..PULSE_TABLE_SIZE)
              index 1: the newest pulse
              index 0: last event (may be pulse or timer event)

At each pulse, the new pulse is put on the queue and last element discarded if
queue full.
If no pulse occurs within WHEEL_STOPPED_TIMEOUT seconds, then a stop signal is
dispatched.
*/

/* ---------- PREFERENCES --------------- */

#define WHEEL_PULSE_TABLE_SIZE 3
#define WHEEL_STOPPED_TIMEOUT 3 // seconds

extern volatile uint16_t wheel_pulse_table[WHEEL_PULSE_TABLE_SIZE];
// index of the oldest pulse in the table
extern volatile uint8_t wheel_pulse_count;

void on_wheel_pulse_collect_data(const uint16_t now);
void on_wheel_stop_collect_data(void);
