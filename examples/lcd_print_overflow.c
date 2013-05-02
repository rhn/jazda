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
#include "../src/display/drawing.c"

/* Test
Drawing numbers
    1st line - number below character limit
    2nd line - number within the desired character count
    3rd line - number not fitting within character limit

Desired behaviour: not sure... Truncate?

Connect a PCD8544 screen.
*/


int main(void) {
    upoint_t position;
    position.x = 16;
    position.y = 0;
    
    number_display_t number_display;
    number_display.integer = 2;
    number_display.fractional = 2;

    upoint_t glyph_size;
    glyph_size.x = 8;
    glyph_size.y = 16;

    lcd_setup();
    lcd_init();
    
    lcd_clean();

    print_number(123, position, glyph_size, 1, number_display);
    position.y += 2;
    print_number(1243, position, glyph_size, 1, number_display);
    position.y += 2;
    print_number(12354, position, glyph_size, 1, number_display);
        
    for (;;) {
    }
}

