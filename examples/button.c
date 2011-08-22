#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>

/* EXAMPLE 2
Switching diodes // XXX: broken?

Features: Pin input, pin output

Connect a diode to pin PD5.
Connect a button to pin PB0.
*/

int main(void) {
  DDRD |= 1<<PD5; /* set Pd5 to output */
  PORTD |= 1<<PD5; /* LED on */
  DDRB = 1<<PB0; /* set pin B0 to input */
  PORTB = 1<<PB0; /* enable pullup resistors */
  while(1) {
    if (PINB & 1<<PB0)
    {
        PORTD &= ~(1<<PD5); /* LED on */
    } else {
        PORTD |= 1<<PD5; /* LED off */
    }
  }
}

