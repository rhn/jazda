/*
    Copyright 2011 rhn <gihu.rhn@porcupinefactory.org>
    Copyright 2013 Paweł Czaplejewicz

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

/* functions and defines to use in modules */

#ifndef _LIB_MODULES_H_
#define _LIB_MODULES_H_
#include "../lib/plots.h"
#include "../actions.h"

#define MODULE_BASE_LINE 4
#define MODULE_SIGNATURE_SIZE 8

typedef struct module_record {
    void (*redraw)(uint8_t force);
    const module_actions_t* (*select_button)(uint8_t state); // function pointer
    char signature[MODULE_SIGNATURE_SIZE]; // module logo for display
} module_record_t;

// clears count columns in front of cursor
void erase_screen(uint8_t count);

void erase_module_screen(void);

// convenience function for drawing the whole plot in module area
void draw_circular_buffer_plot(const circular_buffer_t buffer);

// null action for modules on select
const module_actions_t* module_select_null(const uint8_t state);
#endif
