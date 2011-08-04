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

#include "common.c"

struct module_flags_struct {
    uint8_t distance_changed;
    uint8_t maxspeed_changed;
    uint8_t stopwatch_changed;
    uint8_t avgspeed_changed;
    uint8_t config_changed;
};

volatile struct module_flags_struct module_flags = {0, 0, 0, 0, 0};

#ifdef COMBINED_RESET
    #include "combined_reset.c"
#endif

#ifdef DISTANCE
    #include "distance.c"
#endif

#ifdef CURRENT_SPEED
    #ifdef SPEED_VS_DISTANCE_PLOT
        #include "svdplot.c"
    #endif
    #ifdef MAXSPEED
        #include "maxspeed.c"
    #endif
#endif

#ifdef SPEED_VS_TIME_PLOT
    #include "svtplot.c"
#endif

#ifdef STOPWATCH
    #include "stopwatch.c"
#endif

#ifdef AVGSPEED
    #include "avgspeed.c"
#endif

#ifdef DEBUG
    #include "debug.c"
#endif

#include "signals.c"

// table of module records
const module_record_t modules[] = {
    #ifdef DISTANCE
        distance_record,
    #endif
    #ifdef SPEED_VS_DISTANCE_PLOT
        svd_record,
    #endif
    #ifdef STOPWATCH
        stopwatch_record,
    #endif
    #ifdef DEBUG
        debug_record,
    #endif
    #ifdef MAXSPEED
        maxspeed_record,
    #endif
    #ifdef AVGSPEED
        avgspeed_record,
    #endif
    #ifdef SPEED_VS_TIME_PLOT
        svt_record,
    #endif
    };

#define MODULES_COUNT sizeof(modules)/sizeof(module_record_t)

// currently displayed module
volatile uint8_t current_module = 0;
volatile uint8_t current_module_switched = 1;

/* ------ ACTIONS ----- */
void module_switch_right(uint8_t state) {
   if (state) {
       return;
   }
   if (current_module < MODULES_COUNT - 1) {
       current_module++;
   } else {
       current_module = 0;
   }
   current_module_switched = true;
}

void module_switch_left(uint8_t state) {
   if (state) {
       return;
   }
   if (current_module == 0) {
       current_module = MODULES_COUNT - 1;
   } else {
       current_module--;
   }
   current_module_switched = true;
}

// TODO: crop & scroll if too many modules
// TODO: huge size optimization potential
void module_redraw_menu(void) {
   uint8_t module_index;
   set_row(3);
   set_column(0);
   for (module_index = 0; module_index < MODULES_COUNT; module_index++) {
      uint8_t sig_index;
      for (sig_index = 0; sig_index < MODULE_SIGNATURE_SIZE; sig_index++) {
        char stamp = modules[module_index].signature[sig_index];
        if (module_index == current_module) {
          stamp = ~stamp;
        }
        send_raw_byte(stamp, true);
      }
      send_raw_byte(0, true);
   }
}

