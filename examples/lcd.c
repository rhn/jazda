#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

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
#include "../src/display/pcd8544.c"

/* EXAMPLE 4
LCD test pattern

Features: Serial output

Connect a PCD8544 screen.
*/

int main(void) {
    uint8_t stamp; // uninitialized - the actual value at start doesn't matter
    lcd_setup();
    lcd_init();
    for (;;stamp+=7) {
        _delay_ms(20);
        send_raw_byte(stamp, true);
    }
}

