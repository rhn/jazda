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


/* Library for implementing pin-change interrupts utilizing a single interrupt
and a parity generator.
*/

volatile char emuintpinstate = 0;

void update_emuint_state(void) {
    #ifdef EMUINT0PIN
    if (EMUINT0PORT & 1<<EMUINT0PIN) {
        emuintpinstate |= 0b00000001;
    } else {
        emuintpinstate &= ~0b00000001;
    }
    #endif
    
    #ifdef EMUINT1PIN
    if (EMUINT1PORT & 1<<EMUINT1PIN) {
        emuintpinstate |= 0b00000010;
    } else {
        emuintpinstate &= ~0b00000010;
    }
    #endif
    
    #ifdef EMUINT2PIN
    if (EMUINT2PORT & 1<<EMUINT2PIN) {
        emuintpinstate |= 0b00000100;
    } else {
        emuintpinstate &= ~0b00000100;
    }
    #endif
    
    #ifdef EMUINT3PIN
    if (EMUINT3PORT & 1<<EMUINT3PIN) {
        emuintpinstate |= 0b00001000;
    } else {
        emuintpinstate &= ~0b00001000;
    }
    #endif
    
    #ifdef EMUINT4PIN
    if (EMUINT4PORT & 1<<EMUINT4PIN) {
        emuintpinstate |= 0b00010000;
    } else {
        emuintpinstate &= ~0b00010000;
    }
    #endif
    
    #ifdef EMUINT5PIN
    if (EMUINT5PORT & 1<<EMUINT5PIN) {
        emuintpinstate |= 0b00100000;
    } else {
        emuintpinstate &= ~0b00100000;
    }
    #endif
    
    #ifdef EMUINT6PIN
    if (EMUINT6PORT & 1<<EMUINT6PIN) {
        emuintpinstate |= 0b01000000;
    } else {
        emuintpinstate &= ~0b01000000;
    }
    #endif
    
    #ifdef EMUINT7PIN
    if (EMUINT7PORT & 1<<EMUINT7PIN) {
        emuintpinstate |= 0b10000000;
    } else {
        emuintpinstate &= ~0b10000000;
    }
    #endif
}

void emuint_dispatch(void) {
    char oldstate = emuintpinstate;
    update_emuint_state();    
        
    char statechange = oldstate ^ emuintpinstate;
    
    #ifdef EMUINT0PIN
    if (statechange & 0b00000001) {
        pulses++;
        EMUINT0_handler(emuintpinstate & 0b00000001);
    }
    #endif
    
    #ifdef EMUINT1PIN
    if (statechange & 0b00000010) {
        EMUINT1_handler(emuintpinstate & 0b00000010);
    }
    #endif

    #ifdef EMUINT2PIN
    if (statechange & 0b00000100) {
        EMUINT2_handler(emuintpinstate & 0b00000100);
    }
    #endif

    #ifdef EMUINT3PIN
    if (statechange & 0b00001000) {
        EMUINT3_handler(emuintpinstate & 0b00001000);
    }
    #endif

    #ifdef EMUINT4PIN
    if (statechange & 0b00010000) {
        EMUINT4_handler(emuintpinstate & 0b00010000);
    }
    #endif

    #ifdef EMUINT5PIN
    if (statechange & 0b00100000) {
        EMUINT5_handler(emuintpinstate & 0b00100000);
    }
    #endif
    
    #ifdef EMUINT6PIN
    if (statechange & 0b01000000) {
        EMUINT6_handler(emuintpinstate & 0b01000000);
    }
    #endif
    
    #ifdef EMUINT7PIN
    if (statechange & 0b10000000) {
        EMUINT7_handler(emuintpinstate & 0b10000000);
    }
    #endif
}
