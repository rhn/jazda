#include "common.h"

#ifdef LITTLE_RAM
    struct module_flags_struct {
        unsigned int distance_changed :1;
        unsigned int maxspeed_changed :1;
        unsigned int stopwatch_changed :1;
        unsigned int avgspeed_changed :1;
    };
#else
    struct module_flags_struct {
        uint8_t distance_changed;
        uint8_t maxspeed_changed;
        uint8_t stopwatch_changed;
        uint8_t avgspeed_changed;
    };
#endif

volatile struct module_flags_struct module_flags = {0, 0, 0, 0};

#ifdef COMBINED_RESET
    #include "combined_reset.h"
#endif

#ifdef DISTANCE
    #include "distance.h"
#endif

#ifdef CURRENT_SPEED
    #ifdef SPEED_VS_DISTANCE_PLOT
        #include "svdplot.h"
    #endif
    #ifdef MAXSPEED
        #include "maxspeed.h"
    #endif
#endif

#ifdef SPEED_VS_TIME_PLOT
    #include "svtplot.h"
#endif

#ifdef STOPWATCH
    #include "stopwatch.h"
#endif

#ifdef AVGSPEED
    #include "avgspeed.h"
#endif

#ifdef DEBUG
    #include "debug.h"
#endif

#include "signals.h"

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

#define MODULES_NUMBER sizeof(modules)/sizeof(module_record_t)

// currently displayed module
volatile uint8_t current_module = 0;
volatile uint8_t current_module_switched = 1;

/* ------ ACTIONS ----- */
void module_switch_right(uint8_t state) {
   if (state) {
       return;
   }
   if (current_module < MODULES_NUMBER - 1) {
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
       current_module = MODULES_NUMBER - 1;
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
   for (module_index = 0; module_index < MODULES_NUMBER; module_index++) {
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

