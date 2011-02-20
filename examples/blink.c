#define F_CPU 1000000UL  /* 1 MHz CPU clock */

#include <util/delay.h>
#include <avr/io.h>

/* EXAMPLE 1
Blinking diodes
Connect diodes to pins PB1-5 or PC0-7. No inputs, no interrupts.
*/

void main(void) {
    DDRB |= 1<<PB1; /* set PB1 to 5 to output */
    DDRB |= 1<<PB2; 
    DDRB |= 1<<PB3; 
    DDRB |= 1<<PB4; 
    DDRB |= 1<<PB5; 
    DDRC = _BV (PC0);               /* PC0 is digital output */
         
    while (1)                       /* loop forever */
    {
        /* set PC0 on PORTC (digital high) and delay for 500mS */
        PORTC &= ~_BV(PC0);                
        PORTB = 0;
        _delay_ms(200);
        
        /*  PC0 on PORTC (digital 0) and delay for 500mS */
        PORTC |= _BV(PC0);
        PORTB = 0xFF;
        _delay_ms(500);
    }

}


