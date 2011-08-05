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

/*
Global signal handling. Not part of the module API
*/
/* MODULE ACTIONS MANAGEMENT */

#include "actions.h"

const module_actions_t default_actions = {&module_switch_left,
                                          &module_switch_right};

volatile module_actions_t *current_actions = &default_actions;

// TODO: if no function defined, do nothing. see if space is saved
void on_select_button(uint8_t state) {
   module_actions_t *actions = (*(modules[current_module].select_button))(state);
   if (actions != NULL) {
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

void module_redraw() {
   uint8_t switched = current_module_switched;
   if (switched) {
      current_module_switched = false;
      module_redraw_menu();
      erase_module_screen();
   }
   (*(modules[current_module].redraw))(switched);
}

