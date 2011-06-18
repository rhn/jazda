#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "common/common.h"


#define CLKDIR DDRC
#define CLKPORT PORTC
#define CLKPIN PC4

#define SDADIR DDRC
#define SDAPORT PORTC
#define SDAPIN PC3

#define D_CDIR DDRC
#define D_CPORT PORTC
#define D_CPIN PC2

#define SCEDIR DDRC
#define SCEPORT PORTC
#define SCEPIN PC0
   
#define RSTDIR DDRC
#define RSTPORT PORTC
#define RSTPIN PC1

#include "display/pcd8544.h"

#define MAXBUFFERX 8
#include "display/drawing.h"

/* EXAMPLE 6
LCD button press counter

Features: Serial output, hardware interrupt, pin input, digit display

Connect a PCD8544 screen.
Connect a switch to PD2 (INT0)
*/

volatile int state = 0;

ISR(INT0_vect) {
    state += 1;
}


int main(void) {
    // Set Pin 6 (PD2) as the pin to use for interrupts
    // XXX isn't this invalid?
    DDRD &= ~(1<<PD2);
    PORTD |= (1<<PD2);
    
    // interrupt on INT0 pin falling edge (sensor triggered)
    MCUCR = (1<<ISC01) | (1<<ISC00);
    
    // turn on interrupts!
    GIMSK |= (1<<INT0);
    sei();
    lcd_setup();
    lcd_init();
    upoint_t position;
    position.x = 0;
    position.y = 0;
    upoint_t glyph_size;
    glyph_size.x = 10;
    glyph_size.y = 16;
    
    for (;;) {
        print_number(state, position, glyph_size, 2, NIBBLEPAIR(5, 0));
    }
}

