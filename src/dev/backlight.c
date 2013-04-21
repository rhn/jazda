 /*
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

/* Backlight state management
 Requires:
 BACKLIGHT{DIR|PORT|PIN} defines
*/

#include "../common.h"
#include "backlight.h"

volatile uint8_t backlight_state;

void backlight_init(void) {
    HIGH(BACKLIGHTDIR, BACKLIGHTPIN);
    backlight_off();
}

void backlight_switch(void) {
    if (backlight_state == 0) {
        backlight_on();
    } else {
        backlight_off();
    }
}

// TODO: see if can read output state

void backlight_off(void) {
    LOW(BACKLIGHTPORT, BACKLIGHTPIN);
    backlight_state = 0;
}

void backlight_on(void) {
    HIGH(BACKLIGHTPORT, BACKLIGHTPIN);
    backlight_state = 1;
}
