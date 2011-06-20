// currently displayed module
volatile uint8_t current_module = 0;

// table of module records
const module_record_t modules[] = {
    #ifdef DISTANCE
        {&distance_redraw},
    #endif
    #ifdef DEBUG
        {&debug_redraw}
    #endif
    };

#define MODULES_NUMBER sizeof(modules)/sizeof(module_record_t)

void module_redraw(void) {
    (*modules[current_module].redraw)();
}

module_actions_t *module_on_select_button(uint8_t state) {
   return NULL;
}

void module_on_right_button(uint8_t state) {
   if (current_module < MODULES_NUMBER - 1) {
       current_module++;
   } else {
       current_module = 0;
   }
}

void module_on_left_button(uint8_t state) {
   return;
}

const module_actions_t default_actions = {&module_redraw,
                                          &module_on_left_button,
                                          &module_on_right_button,
                                          &module_on_select_button};

