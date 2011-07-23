#define F_CPU 1000000UL  /* 1 MHz CPU clock */

#include <util/delay.h>
#include <avr/io.h>

/* EXAMPLE 1
Blinking diodes
Connect diodes to pins PB1-5 or PC0. No inputs, no interrupts.
*/

void main(void) {
    DDRB |= 1<<PB1; /* set PB1 to 5 to output */
    DDRB |= 1<<PB2; 
    DDRB |= 1<<PB3; 
    DDRB |= 1<<PB4; 
    DDRB |= 1<<PB5; 
    DDRD = _BV (PD0);               /* PD0 is digital output */
         
    while (1)                       /* loop forever */
    {
        /* set PC0 on PORTC (digital high) and delay for 500ms */
        PORTD &= ~_BV(PD0);                
        PORTB = 0b00001000;
        _delay_ms(200);
        
        /*  PC0 on PORTC (digital 0) and delay for 500ms */
        PORTD |= _BV(PD0);
        PORTB = 0b00000000;
        _delay_ms(500);
    }

}


