// currently displayed module
volatile uint8_t current_module = 0;

// table of module records
const module_record_t modules[] = {
    #ifdef DISTANCE
        distance_record,
    #endif
    #ifdef DEBUG
        debug_record,
    #endif
    };

#define MODULES_NUMBER sizeof(modules)/sizeof(module_record_t)


/* ------ ACTIONS ----- */
void module_switch_right(uint8_t state) {
   if (!state) {
       return;
   }
   if (current_module < MODULES_NUMBER - 1) {
       current_module++;
   } else {
       current_module = 0;
   }
}

void module_switch_left(uint8_t state) {
   if (!state) {
       return;
   }
   if (current_module == 0) {
       current_module = MODULES_NUMBER - 1;
   } else {
       current_module--;
   }
}

const module_actions_t default_actions = {&module_switch_left,
                                          &module_switch_right};

