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

/* this file connects signals from modules/signals.h and button handlers from
actions.h to device-specific hardware using avr.c file for the generic ones.
*/

#include "avr.c"

#include "../actions.h"
#include "../signals.h"

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

/* initializes adc with no interrupts */
void adc_init(void) {
    ADMUX |= (1 << REFS0); // use Vcc as reference
    // select Vbg=1.3V as voltage
    ADMUX |= (1 << MUX3) | (1 << MUX2) | (1 << MUX1);
}

/* turns on adc */
void adc_enable(void) {
    ADCSRA |= (1 << ADEN);
}

/* turns off adc */
void adc_disable(void) {
    ADCSRA &= ~(1 << ADEN);
}

/* blocking read */
uint16_t adc_read(int8_t channel) {
    uint8_t highmask = (uint8_t)(0xff << (MUX0 + 4));
    uint8_t lowmask = (uint8_t)(~(0xff << MUX0));
    uint8_t mask = highmask | lowmask;
    uint8_t bits = (((uint8_t)channel) << MUX0) & ~mask;
    ADMUX = (ADMUX & mask) | bits;
    ADCSRA |= (1 << ADSC);
    
    while (ADCSRA & (1<<ADSC)) {
    }

    return ADC;
}

/* blocking voltage measurement. returns reading in units of 10mV. Resolution: 40mV.
TODO: think about better return units */
uint16_t get_battery_voltage(void) {
    uint16_t raw_reading = adc_read(-2); // -2 == Vbg source @ 1.3V
    
    // R - reading
    // V_{BG} - 1.3V source
    // V_{CC} - battery, reference
    // R = {V_{BG}\cdot1024}\over{V_{CC}}
    // V_{CC} = {V_{BG}\cdot1024}\overR
    if (raw_reading == 0) {
        return 0;
    }
    
    // split precision into 2 parts: 130mV, 10bits resolution
    // apply 8 bits first to get 33280 (fitting in uint16_t)
    // apply 2 bits later to normalize to 10bits
    uint16_t voltage = ((130 << 8) / raw_reading) << 2;
    return voltage;
}
