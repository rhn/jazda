#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define CLKDIR DDRC
#define CLKPORT PORTC
#define CLKPIN PC5

#define SDADIR DDRC
#define SDAPORT PORTC
#define SDAPIN PC4

#define D_CDIR DDRC
#define D_CPORT PORTC
#define D_CPIN PC3

#define SCEDIR DDRC
#define SCEPORT PORTC
#define SCEPIN PC2
   
#define RSTDIR DDRC
#define RSTPORT PORTC
#define RSTPIN PC0
#include "../src/display/pcd8544.c"
#include "../src/display/drawing.c"

/* EXAMPLE 7
LCD drawing example

Features: Serial output, line drawing, glyph drawing, digit printing

Connect a PCD8544 screen.
*/

void draw_buffer(uint8_t* buffer) {
    int i;
    for (i = 0; i < MAXBUFFERX; i++) {
        send_raw_byte(buffer[i], true);
    }
}

int main(void) {
    uint8_t buffer[MAXBUFFERX];
    uint8_t i;
    upoint_t position;
    upoint_t glyph_size;
    position.x = 0;
    position.y = 2;
    glyph_size.x = 8;
    glyph_size.y = 16;

    lcd_setup();
    lcd_init();
    
    // clearing a portion of the screen
    for (i = 1; i != 0; i++) {
        send_raw_byte(0, true);
    }
    for (;;) {
        // drawing lines to buffer
        for (i = 0; i < MAXBUFFERX; i++) {
            buffer[i] = 0;
        }
        draw_line(buffer, 7, 3-8, 0, 15-8, 2);
        set_row(0);
        set_column(0);
        draw_buffer(buffer);
        
        // drawing glyphs into buffer
        for (i = 0; i < MAXBUFFERX; i++) {
            buffer[i] = 0;
        }
        draw_glyph(buffer, glyphs + letter[2], glyph_size, 2, 0, 8);
        set_row(1);
        set_column(0);
        draw_buffer(buffer);
        
        // printing digits to screen
        print_digit(2, glyph_size, 2, position);
    }
}

