// currently displayed module
volatile uint8_t current_module = 0;
volatile uint8_t current_module_switched;

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
    };

#define MODULES_NUMBER sizeof(modules)/sizeof(module_record_t)


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

const module_actions_t default_actions = {&module_switch_left,
                                          &module_switch_right};

