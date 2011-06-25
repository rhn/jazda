#include <util/delay.h>

/* Monochrome line-based display, each line is 8 bits high */
/* HARDWARE
 * Nokia 5110
 * #defines required:
   * pin/direction/port: (*PIN, *DIR, *PORT)
   RST - reset
   D_C - data/command
   CLK - serial clock
   SCE - select
   SDA - serial data
*/

/* prepares the chip for using the display */
void lcd_setup() {
  RSTDIR |= 1 << RSTPIN;
  D_CDIR |= 1 << D_CPIN;
  CLKDIR |= 1 << CLKPIN;
  SCEDIR |= 1 << SCEPIN;
  SDADIR |= 1 << SDAPIN;
  LOW(RSTPORT, RSTPIN);
  LOW(D_CPORT, D_CPIN);
  LOW(CLKPORT, CLKPIN);
  LOW(SDAPORT, SDAPIN);
  LOW(SCEPORT, SCEPIN);
  _delay_ms(200);
  HIGH(RSTPORT, RSTPIN);
}

/* low-level function to send data to the chip. mostly used internally */
void send_raw_byte(const uint8_t payload, const uint8_t data) {
   uint8_t mask = 1 << 7;
   uint8_t i;
   for (i = 0; i < 8; i++) {
       if (mask & payload) {
           HIGH(SDAPORT, SDAPIN);
       } else {
           LOW(SDAPORT, SDAPIN);
       }
       
       if ((i == 1) && data) {
           HIGH(D_CPORT, D_CPIN);
       }
       HIGH(CLKPORT, CLKPIN);
       LOW(CLKPORT, CLKPIN);
       
       mask = mask >> 1;
   }
   LOW(SDAPORT, SDAPIN);
   LOW(D_CPORT, D_CPIN);
}

#define blank_screen() send_raw_byte(0b00001000, false)
#define darken_screen() send_raw_byte(0b00001001, false)
#define normal_screen() send_raw_byte(0b00001100, false)
#define inverse_screen() send_raw_byte(0b00001101, false)

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

/* prepares the display for operation */
void lcd_init() {
//    for(;;){
  send_raw_byte(0b00100001, false); // extended instruction set
  send_raw_byte(0b10000000 | 66, false); // set voltage to just under 7V (3V+66*0.06V steps)
  send_raw_byte(0b00000100, false); // set temp coeff
  send_raw_byte(0b00010011, false); // set bias
  send_raw_byte(0b00100000, false); //poweron, horz addr, basic instruction set

  normal_screen();
}