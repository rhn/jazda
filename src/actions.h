/*
Global signal handling. Not part of the module API
*/
/* MODULE ACTIONS MANAGEMENT */

const module_actions_t default_actions = {&module_switch_left,
                                          &module_switch_right};

volatile module_actions_t *current_actions = &default_actions;

// TODO: if no function defined, do nothing. see if space is saved
void on_select_button(uint8_t state) {
   module_actions_t *actions = (*(modules[current_module].select_button))(state);
   if (actions == NULL) {
      current_actions = &default_actions;
   } else {
      current_actions = actions;
   }
}
// TODO: merge the 2 functions into 1 with 2 calls to it, using macro offsetof(struct, elem);
void on_right_button(uint8_t state) {
   (*(current_actions->button_right))(state);
}

void on_left_button(uint8_t state) {
   (*(current_actions->button_left))(state);
}

#include "emuint.h"

void module_redraw() {
   uint8_t switched = current_module_switched;
   if (switched) {
      current_module_switched = false;
      module_redraw_menu();
      erase_module_screen();
   }
   (*(modules[current_module].redraw))(switched);
}

