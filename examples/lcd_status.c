#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "common/common.h"

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
#include "display/pcd8544.h"

#define MAXBUFFERX 8
#include "display/drawing.h"

/* EXAMPLE 7
LCD button press indicator

Features: Serial output, hardware interrupt, split interrupts, pin input, digit display

Connect a PCD8544 screen.
Connect interrupt splitter interrupt output to PD2 (INT0)
Connect interrupt splitter outputs to PB1-PB5
*/

volatile int state[5] = {0, 0, 0, 0, 0};
volatile char emuintpinstate = 0;
volatile uint8_t interrupts = 0;

#define EMUINT0PIN PB1
#define EMUINT0PORT PINB

#define EMUINT1PIN PB2
#define EMUINT1PORT PINB
/*
#define EMUINT2PIN PB3
#define EMUINT2PORT PINB

#define EMUINT3PIN PB3
#define EMUINT3PORT PINB

#define EMUINT4PIN PB4
#define EMUINT4PORT PINB
*/
#ifdef EMUINT0PIN
void EMUINT0_handler(void);
#endif
#ifdef EMUINT1PIN
void EMUINT1_handler(void);
#endif
#ifdef EMUINT2PIN
void EMUINT2_handler(void);
#endif
#ifdef EMUINT3PIN
void EMUINT3_handler(void);
#endif
#ifdef EMUINT4PIN
void EMUINT4_handler(void);
#endif
#ifdef EMUINT5PIN
void EMUINT5_handler(void);
#endif
#ifdef EMUINT6PIN
void EMUINT6_handler(void);
#endif
#ifdef EMUINT7PIN
void EMUINT7_handler(void);
#endif


void update_emuint_state(void) {
    #ifdef EMUINT0PIN
    if (EMUINT0PORT & 1<<EMUINT0PIN) {
        emuintpinstate |= 0b00000001;
    } else {
        emuintpinstate &= ~0b00000001;
    }
    #endif
    
    #ifdef EMUINT1PIN
    if (EMUINT1PORT & 1<<EMUINT1PIN) {
        emuintpinstate |= 0b00000010;
    } else {
        emuintpinstate &= ~0b00000010;
    }
    #endif
    
    #ifdef EMUINT2PIN
    if (EMUINT2PORT & 1<<EMUINT2PIN) {
        emuintpinstate |= 0b00000100;
    } else {
        emuintpinstate &= ~0b00000100;
    }
    #endif
    
    #ifdef EMUINT3PIN
    if (EMUINT3PORT & 1<<EMUINT3PIN) {
        emuintpinstate |= 0b00001000;
    } else {
        emuintpinstate &= ~0b00001000;
    }
    #endif
    
    #ifdef EMUINT4PIN
    if (EMUINT4PORT & 1<<EMUINT4PIN) {
        emuintpinstate |= 0b00010000;
    } else {
        emuintpinstate &= ~0b00010000;
    }
    #endif
    
    #ifdef EMUINT5PIN
    if (EMUINT5PORT & 1<<EMUINT5PIN) {
        emuintpinstate |= 0b00100000;
    } else {
        emuintpinstate &= ~0b00100000;
    }
    #endif
    
    #ifdef EMUINT6PIN
    if (EMUINT6PORT & 1<<EMUINT6PIN) {
        emuintpinstate |= 0b01000000;
    } else {
        emuintpinstate &= ~0b01000000;
    }
    #endif
    
    #ifdef EMUINT7PIN
    if (EMUINT7PORT & 1<<EMUINT7PIN) {
        emuintpinstate |= 0b10000000;
    } else {
        emuintpinstate &= ~0b10000000;
    }
    #endif
}


ISR(INT0_vect) {
    char oldstate = emuintpinstate;
    interrupts++;

    update_emuint_state();    
        
    char statechange = oldstate ^ emuintpinstate;
    
    #ifdef EMUINT0PIN
    if (statechange & 0b00000001) {
        EMUINT0_handler();
    }
    #endif
    
    #ifdef EMUINT1PIN
    if (statechange & 0b00000010) {
        EMUINT1_handler();
    }
    #endif

    #ifdef EMUINT2PIN
    if (statechange & 0b00000100) {
        EMUINT2_handler();
    }
    #endif

    #ifdef EMUINT3PIN
    if (statechange & 0b00001000) {
        EMUINT3_handler();
    }
    #endif

    #ifdef EMUINT4PIN
    if (statechange & 0b00010000) {
        EMUINT4_handler();
    }
    #endif

    #ifdef EMUINT5PIN
    if (statechange & 0b00100000) {
        EMUINT5_handler();
    }
    #endif
    
    #ifdef EMUINT6PIN
    if (statechange & 0b01000000) {
        EMUINT6_handler();
    }
    #endif
    
    #ifdef EMUINT7PIN
    if (statechange & 0b10000000) {
        EMUINT7_handler();
    }
    #endif
}

void EMUINT0_handler(void) {
    state[0] += 1;
}

void EMUINT1_handler(void) {
    state[1] += 1;
}

int main(void) {
    // Set Pin 6 (PD2) as the pin to use for hardware interrupts
    DDRD &= ~(1<<PD2);
    PORTD |= (1<<PD2);
    
    // set PB1 to receive emulated interrupts
    DDRB &= ~(1<<PB1);
    PORTB |= (1<<PB1);
    
    DDRB &= ~(1<<PB2);
    PORTB |= (1<<PB2);
    
    update_emuint_state();
    
    // interrupt on INT0 pin falling and rising edge (needed for the interrupt splitter)
    MCUCR |= 1<<ISC00;
    MCUCR &= ~(1<<ISC01);
    
    // turn on interrupts!
    GIMSK |= (1<<INT0);
    sei();
    lcd_setup();
    lcd_init();
    upoint_t position;
    upoint_t glyph_size;
    
    for (;;) {
        glyph_size.x = 6;
        glyph_size.y = 8;
        position.x = 0;
        position.y = 0;
        print_number(interrupts, position, glyph_size, 1, NIBBLEPAIR(3, 0));
        
        position.y = 1;
        print_number(state[0], position, glyph_size, 1, NIBBLEPAIR(3, 0));
        
        position.y = 2;
        glyph_size.x = 10;
        glyph_size.y = 12;
        print_number(state[1], position, glyph_size, 1, NIBBLEPAIR(3, 0));
        
        
        send_raw_byte(0xff, true);
        if (PIND & 1<<PD2) {
            send_raw_byte(0xff, true);
        } else {
            send_raw_byte(0b00100100, true);
        }
        send_raw_byte(0xfc, true);
        
        send_raw_byte(0xff, true);
        if (PINB & 1<<PB1) {
            send_raw_byte(0xff, true);
        } else {
            send_raw_byte(0b00100100, true);
        }
        send_raw_byte(0xfc, true);
        
        send_raw_byte(0xff, true);
        if (PINB & 1<<PB2) {
            send_raw_byte(0xff, true);
        } else {
            send_raw_byte(0b00100100, true);
        }
        send_raw_byte(0xfc, true);
        send_raw_byte(0x00, true);
        send_raw_byte(0xfF, true);
        send_raw_byte(emuintpinstate, true);
        send_raw_byte(0xfc, true);
    }
}

