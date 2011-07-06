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

#include "avr.c"

#include "../actions.h"

#define EMUINT0PIN LEFTPIN
#define EMUINT0PORT LEFTPORT
#define EMUINT0DIR LEFTDIR
void EMUINT0_handler(uint8_t state) {
    on_left_button(state);
}

#define EMUINT1PIN RIGHTPIN
#define EMUINT1PORT RIGHTPORT
#define EMUINT1DIR RIGHTDIR
void EMUINT1_handler(uint8_t state) {
    on_right_button(state);
}

#define EMUINT2PIN SELECTPIN
#define EMUINT2PORT SELECTPORT
#define EMUINT2DIR SELECTDIR
void EMUINT2_handler(uint8_t state) {
    on_select_button(state);
}

#ifdef CRANK
    #define EMUINT3PIN CRANKPIN
    #define EMUINT3PORT CRANKPORT
    #define EMUINT3DIR CRANKDIR
    void EMUINT3_handler(uint8_t state) {
        if (!state) {
            on_crank_pulse();
        }
    }
#endif

#include "emuint.c"

void setup_cpu(void) {
   // makes CPU clock 1 MHz
   // can't be done on this device
}
