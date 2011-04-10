#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>

/* EXAMPLE 2
Switching diodes

Features: Pin input, pin output

Connect a diode to pin PD5.
Connect buttons to pins PB0-7.
*/

int main(void) {
  DDRD |= 1<<PD5; /* set Pd5 to output */
  PORTD |= 1<<PD5; /* LED on */
  DDRB = 1<<PB0; /* set port b to input */
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

