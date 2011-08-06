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

/* STOPWATCH MODULE
resettable stopwatch.
Time is updated on each second interrupt, thanks to that it's displayed
instantly with little jitter, but offset in relation to reset time.
*/

#include "../builtins/wheel.h"
#include "../display/drawing.h"
#include "../lib/timer.h"

typedef struct time_storage {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
} time_storage_t;

volatile time_storage_t stopwatch_time = {0, 0, 0};
volatile uint16_t stopwatch_next_call;
volatile timer_handle_t stopwatch_timer_handle = -1;

void stopwatch_redraw(uint8_t force) {
   if ((force) || module_flags.stopwatch_changed) {
       upoint_t position = {0, 5};
       upoint_t glyph_size = {8, 8};
       module_flags.stopwatch_changed = false;
       print_number(stopwatch_time.hours, position, glyph_size, 1, (number_display_t){.integer=4, .fractional=0});
       position.x += 4 * 9 + 2; // 4 chars + 2 extra pixels space
       print_number(stopwatch_time.minutes, position, glyph_size, 1, (number_display_t){.integer=1, .fractional=1}); // 1 fractional digit to preserve the "0" in front
       position.x += 2 * 9 + 2; // 2 chars + 2 extra pixels space
       print_number(stopwatch_time.seconds, position, glyph_size, 1, (number_display_t){.integer=1, .fractional=1}); // 1 fractional digit to preserve the "0" in front
   }
}

void stopwatch_reset(void) {
   stopwatch_time.hours = 0;
   stopwatch_time.minutes = 0;
   stopwatch_time.seconds = 0;
   module_flags.stopwatch_changed = true;
}

module_actions_t *stopwatch_select(uint8_t state) {
   if (state) {
      stopwatch_reset();
   }
   return NULL;
}

void stopwatch_increment(void) {
   time_storage_t new_time = stopwatch_time;
   if (new_time.seconds == 59) {
       new_time.seconds = 0;
       if (new_time.minutes == 59) {
           new_time.minutes = 0;
           new_time.hours++;
       } else {
           new_time.minutes++;
       }
   } else {
       new_time.seconds++;
   }
   stopwatch_time = new_time;
   module_flags.stopwatch_changed = true;
}

void stopwatch_on_timeout(void);
void stopwatch_on_timeout(void) {
   stopwatch_increment();
   stopwatch_next_call += ONE_SECOND;
   stopwatch_timer_handle = timer_set_callback(stopwatch_next_call, &stopwatch_on_timeout);
}

void stopwatch_on_wheel_pulse(const uint16_t now) {
   if (wheel_pulse_count == 1) {
       stopwatch_next_call = now + ONE_SECOND;
       stopwatch_timer_handle = timer_set_callback(stopwatch_next_call, &stopwatch_on_timeout);
   }
}

void stopwatch_on_wheel_stop(void) {
   timer_clear_callback(stopwatch_timer_handle);
   stopwatch_timer_handle = -1;
}

#define stopwatch_signature {0b00111000, 0b01000100, 0b10000011, 0b10011111, 0b10001011, 0b01000100, 0b00111000, 0}

#ifdef COMBINED_RESET
    #define stopwatch_record {&stopwatch_redraw, &combined_reset_on_select, stopwatch_signature}    
#else
    #define stopwatch_record {&stopwatch_redraw, &stopwatch_select, stopwatch_signature}
#endif
