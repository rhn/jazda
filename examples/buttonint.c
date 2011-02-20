#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* EXAMPLE 3
Switching diodes

Features: Hardware interrupt, pin input, pin output

Connect a diode to pin PD5 (hardware interrupt).
Connect a diode to pin PD6 (button pressed).
Connect a switch to PD2 (INT0)
Connect a switch to PB0.
*/

int state = 0;

ISR(INT0_vect) {
  if (state)
  {
    PORTD &= ~(1<<PD5); /* LED on */
    state = 0;
  } else {
    PORTD |= 1<<PD5; /* LED off */
    state = 1;
  }
}

int main(void) {
  DDRD |= 1<<PD5; /* set Pd5 to output */
  DDRD |= 1<<PD6; /* set Pd6 to output */
  DDRB &= ~(1<<PB0); /* set Pb0 to input */

  // Set Pin 6 (PD2) as the pin to use for interrupts
  // XXX isn't this invalid?
  PCMSK |= (1<<PIND2);
  
  // interrupt on INT0 pin falling edge (sensor triggered)
  MCUCR = (1<<ISC01) | (1<<ISC00);
  
  // turn on interrupts!
  GIMSK |= (1<<INT0);
  sei();
  while(1) {
    if (PORTB & (1<<PB0)) {
        PORTD &= ~(1<<PD6); /* LED on */
    } else {
        PORTD |= 1<<PD6; /* LED off */
    }
  }
}

