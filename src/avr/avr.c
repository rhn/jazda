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

#include <avr/interrupt.h>

/* AVR-specific routines.
This module links signals from signals.h to interrupts in generic AVR hardware.

REQUIRES:
AVR #defines regarding interrupts
Signals

TODO: chip-specific functions to chip-specific files
FIXME: race condition in reading extended_time while overflow should be
happening.
*/

#include "../modules/signals.h"
#include "../lib/timer.h"

volatile uint16_t extended_time = 0;

void setup_wheel_pulse(void) {
  /* -----  hwint button */  
  PULSEDIR &= ~(1<<PULSEPIN); /* set PD2 to input */
  PULSEPORT |= 1<<PULSEPIN; // enable pullup resistor
    
  // interrupt on INT0 pin falling edge (sensor triggered)
  MCUCR |= (1<<ISC01);
  MCUCR &= ~(1<<ISC00);
  /* ------ end */

  // turn on interrupts!
  GIMSK |= (1<<INT0);
}

ISR(INT0_vect) {
    on_wheel_pulse();
}

void setup_buttons(void) {
  /* -----  hwint button */  
  BUTTONDIR &= ~(1<<BUTTONPIN); /* set PD3 to input */
  BUTTONPORT |= 1<<BUTTONPIN; // enable pullup resistor
  
  // emuint buttons
  LEFTDIR &= ~(1<<LEFTPIN);
  LEFTPORT |= 1<<LEFTPIN;

  RIGHTDIR &= ~(1<<RIGHTPIN);
  RIGHTPORT |= 1<<RIGHTPIN;
  
  SELECTDIR &= ~(1<<SELECTPIN);
  SELECTPORT |= 1<<SELECTPIN;
  
  #ifdef CRANK
      CRANKDIR &= ~(1<<CRANKPIN);
      CRANKPORT |= 1<<CRANKPIN;
  #endif
  
  // interrupt on INT1 pin both edges (emuint state change)
  MCUCR |= (1<<ISC10);
  MCUCR &= ~(1<<ISC11);
  /* ------ end */

  // turn on interrupts!
  GIMSK |= (1<<INT1);
}

void emuint_dispatch(void);

volatile uint8_t pulses = 0;

ISR(INT1_vect) {
  emuint_dispatch();
}


void setup_timer(void) {
// sets up millisecond precision 16-bit timer
/* TODO: consider using 8-bit timer overflowing every (half) second + 4-bit USI*/
// set up prescaler to /64

/* PRESCALER SETTINGS:
CS12 CS11 CS10 Description
 0    0    0   No clock source. (Timer/Counter stopped)
 0    0    1   clkIO/1 (No prescaling)
 0    1    0   clkIO/8 (From prescaler)
 0    1    1   clkIO/64 (From prescaler)
 1    0    0   clkIO/256 (From prescaler)
 1    0    1   clkIO/1024 (From prescaler)
 1    1    0   External clock source on T1 pin. Clock on falling edge.
 1    1    1   External clock source on T1 pin. Clock on rising edge.
*/
  uint8_t clksrc;
  clksrc = TCCR1B;
// for /1024
//  clksrc |= 1<<CS12 | 1<<CS10;
//  clksrc &= ~(1<<CS11);
// for /64
//  clksrc |= 1<<CS11 | 1<<CS10;
//  clksrc &= ~(1<<CS12);
// for /256
  clksrc |= 1<<CS12;
  clksrc &= ~(1<<CS11) & ~(1<<CS10);

  TCCR1B = clksrc;

  TIMSK |= 1 << TOIE1; // enable overflow interrupt
  
  // set up each second timer
  OCR1B = TCNT1 + ONE_SECOND;
  TIMSK |= 1 << OCIE1B;
}

inline void set_immediate_trigger(void) {
  TIFR |= 1 << OCF1A;
}

inline uint16_t get_time(void) {
  return TCNT1;
}

uint32_t get_extended_time() {
  return (((uint32_t)extended_time) << 16) | TCNT1; // TODO: may be possible to optimize in asm
}

void set_trigger_time(const uint16_t time) { // XXX: optimize: inline
  OCR1A = time;
  TIMSK |= 1 << OCIE1A;
}

void clear_trigger() {
  TIMSK &= ~(1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect) {
    timer_dispatch();
}

ISR(TIMER1_OVF_vect) {
    extended_time++;
}

ISR(TIMER1_COMPB_vect) {
    on_each_second();
    OCR1B += ONE_SECOND;
}
