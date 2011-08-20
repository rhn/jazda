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

Speed is calculated between first and last pulse in queue on redraw. A timer
event is set some time after the predicted next pulse. If the pulse comes, timer
is set again. If the pulse doesn't come, the timeout is considered the next
valid pulse (thus lowering the speed).
*/

/* ---------- PREFERENCES --------------- */
#define SPEED_SIGNIFICANT_DIGITS 2 // 99km/h is good enough
#define SPEED_FRACTION_DIGITS 1 // better than my sigma

#define SPEED_DIGITS (number_display_t){.integer=SPEED_SIGNIFICANT_DIGITS, .fractional=SPEED_FRACTION_DIGITS}

void speed_on_wheel_stop(void);
void speed_on_wheel_pulse(const uint16_t now);
void speed_redraw(void);

#ifdef LONG_CALCULATIONS
    uint16_t get_average_speed_long(uint32_t time_amount, const uint16_t pulse_count);
#endif

uint16_t get_average_speed(const uint16_t time_amount, const uint8_t pulse_count);

#ifndef CONSTANT_PULSE_DISTANCE
    void speed_update_factor(void);
#endif
