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

/* EXAMPLE 9
ADC used to measure battery voltage

Features: Serial output, analog comparator, number printing.

Connect a PCD8544 screen.
Connect PC5 to a voltage source and use as a probe.
*/


/* initializes adc with no interrupts */
void adc_init(void) {
    ADMUX |= (1 << REFS0); // use Vcc as reference
    // select Vbg=1.3V as voltage
    ADMUX |= (1 << MUX3) | (1 << MUX2) | (1 << MUX1);
}

/* turns on adc */
void adc_enable(void) {
    ADCSRA |= (1 << ADEN);
}

/* turns off adc */
void adc_disable(void) {
    ADCSRA &= ~(1 << ADEN);
}

uint16_t adc_read(int8_t channel) {
    uint8_t highmask = (uint8_t)(0xff << (MUX0 + 4));
    uint8_t lowmask = (uint8_t)(~(0xff << MUX0));
    uint8_t mask = highmask | lowmask;
    uint8_t bits = (((uint8_t)channel) << MUX0) & ~mask;
    ADMUX = (ADMUX & mask) | bits;
    ADCSRA |= (1 << ADSC);
    
    while (ADCSRA & (1<<ADSC)) {
    }

    return ADC;
}

/* blocking voltage measurement. returns reading in units of 10mV. Resolution: 40mV.
TODO: think about better returned units */
uint16_t get_voltage(void) {
    uint16_t raw_reading = adc_read(-2); // -2 == Vbg source @ 1.3V
    
    // R - reading
    // V_{BG} - 1.3V source
    // V_{CC} - battery, reference
    // R = {V_{BG}\cdot1024}\over{V_{CC}}
    // V_{CC} = {V_{BG}\cdot1024}\overR
    if (raw_reading == 0) {
        return 0;
    }
    
    uint16_t voltage = ((130 << 8) / raw_reading) << 2;
    return voltage;
}

int main(void) {
    upoint_t position;
    upoint_t glyph_size;
    number_display_t number_display;
    number_display.integer = 1;
    number_display.fractional = 2;
    position.x = 1;
    glyph_size.x = 8;
    glyph_size.y = 8;
    DDRC = 1 << PC5; /* set pin B0 to input */
    lcd_setup();
    lcd_init();
    adc_init();
    adc_enable();
    // clearing a portion of the screen
    lcd_clean();
    for (;;) {
        uint16_t raw = adc_read(-2);
        // printing digits to screen. voltage is a fraction of Vcc
        position.y = 1;
        print_number(raw, position, glyph_size, 1, number_display);
        
        uint16_t voltage = get_voltage();
        // printing digits to screen. voltage is a fraction of Vcc
        position.y = 2;
        print_number(voltage, position, glyph_size, 1, number_display);        
//        send_raw_byte(voltage, true);
        send_raw_byte(0, true);
    }
}

