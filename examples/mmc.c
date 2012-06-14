#define F_CPU 1000000UL  /* 1 MHz CPU clock */

#include <util/delay.h>
#include <avr/io.h>

#include "mmc/mmc_if.h"

/* EXAMPLE ?
Plain MMC write.
Will write data straight to the card (forever). WARNING! This is a low-level write. It WILL DESTROY the beginning of the card, where your partition table most likely lies, rendering your data inaccessible.
Diode will shine when writing, blink when stopped early, not shine when ended successfully.

Connect MMC-compatible card to controller's SPI ports. Connect a diode to pin PB1. No inputs, no interrupts.
*/

void blink_forever(void) {
    while (1) {                      /* blink forever */
        /* set PB1 on PORTB (digital high) and delay */
        PORTB |= 1<<PB1;
        _delay_ms(200);
        
        /* clear PB1 on PORTB (digital 0) and delay */
        PORTB &= ~(1<<PB1);
        _delay_ms(500);
    }
}

uint8_t write_buffer[512];

void fill_buffer(void) {
    uint8_t i = 0;
    do {
        write_buffer[i] = i;
    } while (i != 0);
}

void main(void) {
    DDRB |= 1<<PB1; /* set PB1 to output */
    PORTB |= 1<<PB1; // Start shining for the show
    
    uint8_t failed = 0;
    uint32_t sector = 0;
    
    failed = mmc_init();
    if (failed) {
        blink_forever();
    }
    
    do {
        fill_buffer();
        failed = mmc_writesector(sector, write_buffer);
        if (failed) {
            blink_forever();
        }
        sector++;
    } while (sector != 0); // this condition will become true again when sector 0xffff was just processed
    
    if (failed) {
        blink_forever();
    }

    PORTB &= ~(1<<PB1); // stop shining
    while (1) { // forever
    }
}



