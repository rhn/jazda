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

#ifndef _ACTIONS_H_
#define _ACTIONS_H_
typedef struct module_actions {
    void (*button_left)(uint8_t state);
    void (*button_right)(uint8_t state);
} module_actions_t;

extern const module_actions_t default_actions;

void on_select_button(uint8_t state);

void on_right_button(uint8_t state);

void on_left_button(uint8_t state);

void module_redraw();
#endif
