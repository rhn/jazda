#include <util/delay.h>

/* Monochrome line-based display, each line is 8 bits high */
/* HARDWARE
 * Nokia 5110
*/

void lcd_setup() {
  //DDRB |= 1<<PB1 | 1<<PB2 | 1<<PB3 | 1<<PB4 | 1<<PB5; /* set Pb1-5 to output */
  RSTDIR |= 1 << RSTPIN;
  D_CDIR |= 1 << D_CPIN;
  CLKDIR |= 1 << CLKPIN;
  SCEDIR |= 1 << SCEPIN;
  SDADIR |= 1 << SDAPIN;
  LOW(RSTPORT, RSTPIN);
  LOW(D_CPORT, D_CPIN);
  LOW(CLKPORT, CLKPIN);
  LOW(SCEPORT, SCEPIN);
  LOW(SDAPORT, SDAPIN);
  _delay_ms(200);
  HIGH(RSTPORT, RSTPIN);
}

void send_raw_byte(const uint8_t payload, const uint8_t data) {
   uint8_t mask = 1 << 7;
   uint8_t i;
   for (i = 0; i < 8; i++) {
       if (mask & payload) {
           HIGH(SDAPORT, SDAPIN);
       } else {
           LOW(SDAPORT, SDAPIN);
       }
       
       if ((i == 7) && data) {
           HIGH(D_CPORT, D_CPIN);
       }
       HIGH(CLKPORT, CLKPIN);
       LOW(CLKPORT, CLKPIN);
       
       LOW(D_CPORT, D_CPIN);
       
       mask = mask / 2;
   }
}

#define blank_screen() send_raw_byte(0b00001000, false);
#define darken_screen() send_raw_byte(0b00001001, false);
#define normal_screen() send_raw_byte(0b00001100, false);
#define inverse_screen() send_raw_byte(0b00001101, false);

void set_column(const uint8_t col) {
    if (col < 84) {
        send_raw_byte(col | 0b10000000, false);
    }
}

void set_row(const uint8_t row) {
    if (row < 6) {
        send_raw_byte(row | 0b01000000, false);
    }
}

void lcd_init() {
  send_raw_byte(0b00100001, false); // extended instruction set
//  send_raw_byte(0b11000101, false); // set voltage to what some other guy did
  send_raw_byte(0xC2, false);       // Set LCD Voltage to about 7V.
  send_raw_byte(0b00010011, false); // set bias
  send_raw_byte(0b00100000, false); //poweron, horz addr, basic instruction set
//  send_raw_byte(0b00100010, false); //poweron, vert addr, basic instruction set
//  darken_screen();
  normal_screen();      
}
