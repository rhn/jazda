#include <util/delay.h>

#define LETTERX _BV(3)
#define LETTERY _BV(4)
#define POINT(x, y, break) break << 7 | x << 4 | y

#define FIXED_POINT_BITS 3

__attribute__((progmem)) const uint8_t glyphs[] = { POINT(2, 0, 1), POINT(5, 0, 1), POINT(7, 2, 1), POINT(7, 13, 1), POINT(5, 15, 1), POINT(2, 15, 1), POINT(0, 13, 1), POINT(0, 2, 1), POINT(2, 0, 1), 0, // 0
                           POINT(1, 4, 1), POINT(5, 0, 1), POINT(5, 15, 1), 0, // 1
                           POINT(0, 3, 1), POINT(2, 0, 1), POINT(5, 0, 1), POINT(7, 3, 1), POINT(0, 15, 1), POINT(7, 15, 1), 0, // 2
                           POINT(0, 3, 1), POINT(2, 0, 1), POINT(5, 0, 1), POINT(7, 2, 1), POINT(4, 7, 1), POINT(7, 9, 1), POINT(7, 13, 1), POINT(5, 15, 1), POINT(2, 15, 1), POINT(0, 12, 1), 0, // 3
                           POINT(7, 10, 1), POINT(0, 10, 1), POINT(5, 0, 1), POINT(5, 15, 1), 0, // 4
                           POINT(7, 0, 1), POINT(0, 0, 1), POINT(0, 7, 1), POINT(5, 7, 1), POINT(7, 9, 1), POINT(7, 13, 1), POINT(5, 15, 1), POINT(0, 15, 1), 0, // 5
                           POINT(7, 2, 1), POINT(5, 0, 1), POINT(2, 0, 1), POINT(0, 3, 1), POINT(0, 13, 1), POINT(2, 15, 1), POINT(5, 15, 1), POINT(7, 13, 1), POINT(7, 10, 1), POINT(5, 8, 1), POINT(0, 8, 1), 0, // 6
                           POINT(0, 0, 1), POINT(7, 0, 1), POINT(0, 15, 1), 0, // 7
                           POINT(2, 0, 1), POINT(5, 0, 1), POINT(7, 2, 1), POINT(7, 5, 1), POINT(0, 10, 1), POINT(0, 13, 1), POINT(2, 15, 1), POINT(5, 15, 1), POINT(7, 13, 1), POINT(7, 10, 1), POINT(0, 5, 1), POINT(0, 2, 1), POINT(2, 0, 1), 0, // 8
                           POINT(0, 15, 1), POINT(5, 15, 1), POINT(7, 12, 1), POINT(7, 2, 1), POINT(5, 0, 1), POINT(2, 0, 1), POINT(0, 2, 1), POINT(0, 6, 1), POINT(2, 8, 1), POINT(7, 8, 1), 0, // 9
                           }; 

typedef struct glyph_desc {
    upoint_t size;
    uint8_t line_width;
} glyphdesc_t;


//digit table
const uint8_t letter[] = { 0, 10, 14, 21, 32, 37, 46, 58, 62, 76};

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
       if (data) {
           LOW(D_CPORT, D_CPIN);
       }
       mask = mask / 2;
   }
}

#define blank_screen() send_raw_byte(0b00001000, false);
#define darken_screen() send_raw_byte(0b00001001, false);
#define normal_screen() send_raw_byte(0b00001100, false);
#define inverse_screen() send_raw_byte(0b00001101, false);

void set_column(const uint8_t col) {
    send_raw_byte(col | _BV(7), false);
}

void set_row(const uint8_t row) {
    send_raw_byte(row | _BV(6), false);
}

void lcd_init() {
  send_raw_byte(0b00100001, false); // extended instruction set
//  send_raw_byte(0b11000101, false); // set voltage to what some other guy did
  send_raw_byte(0xC2, false);		// Set LCD Voltage to about 7V.
  send_raw_byte(0b00010011, false); // set bias
  send_raw_byte(0b00100000, false); //poweron, horz addr, basic instruction set
//  send_raw_byte(0b00100010, false); //poweron, vert addr, basic instruction set
//  darken_screen();
  normal_screen();      
}

void draw_line(uint8_t *buffer, int8_t fromx, int8_t fromy, int8_t tox, int8_t toy, const int8_t width) {
// draws a line into the buffer
   uint8_t stamp = 0;
   int8_t i;
   int8_t incr;
   if (fromx > tox) { // swap
       int8_t tmp;
       tmp = tox;
       tox = fromx;
       fromx = tmp;
       tmp = toy;
       toy = fromy;
       fromy = tmp;
   } else if (fromx == tox) {
//       print_vert_line(buffer, fromx, fromy, toy, width);
       int8_t tmp;
       if (fromy > toy) {
           tmp = toy;
           toy = fromy;
           fromy = tmp;
       }
       for (i = fromy; i < toy + width; i++) {
           stamp |= 1 << i;
       }
       for (i = fromx; i < fromx + width; i++) {
           if ((i >= 0) && (i < MAXBUFFERX)) {
               buffer[i] |= stamp;
           }
  //         send_raw_byte(i, true);
       }    
       send_raw_byte(0b01000011, true);
       return;
   }
   incr = ((toy - fromy) << FIXED_POINT_BITS) / (int8_t)(tox - fromx); // guaranteed positive
   for (i = 0; i < width; i++) { // LSB is x=0
       stamp <<= 1;
       stamp |= 1;
   }
   fromy <<= FIXED_POINT_BITS; // fromy becomes fixed-point, might be negative
   // from starting point to ending point + line width
   for (i = fromx; i < tox + width; i++, fromy += incr) {
       if ((i >= 0) && (i < MAXBUFFERX) && (fromy >= 0)) { //   && (fromy < (8 << FIXED_POINT_BITS))) { // 
           buffer[i] |= (uint8_t)(stamp << (uint8_t)(fromy >> FIXED_POINT_BITS)); //  / _BV(FIXED_POINT_BITS))); //
       }
   }
}

void scale(upoint_t *point, const upoint_t pxsize) {
   uint16_t tmp;
   tmp = (*point).x * pxsize.x;
   (*point).x = tmp / LETTERX;
/*   if (tmp & (LETTERX / 2)) { // rounding
      (*x)++;
   }*/
   tmp = (*point).y * pxsize.y;
   (*point).y = tmp / LETTERY;
/*   if (tmp & (LETTERY / 2)) {
      (*y)++;
   }*/
}

void draw_glyph(uint8_t *buffer, const uint8_t *glyph, const upoint_t glyph_size, const uint8_t width, const int8_t xoffset, const int8_t yoffset) {
   upoint_t prev;
   upoint_t next;
   upoint_t pxsize = {glyph_size.x - width + 1, glyph_size.y - width + 1};
   int8_t i;

   prev.x = pgm_read_byte(&(glyph[0]));
   prev.y = prev.x & 0b00001111;
   prev.x >>= 4;
   prev.x &= 0b00000111;
   prev.x -= xoffset;
   prev.y -= yoffset;
     
   scale(&prev, pxsize);
   for (i = 1; pgm_read_byte(&(glyph[i])); i++) {
      next.x = pgm_read_byte(&(glyph[i]));
      next.y = next.x & 0b00001111; 
      next.x >>= 4;
      next.x &= 0b00000111;
      next.x -= xoffset;
      next.y -= yoffset;
      scale(&next, pxsize);
      draw_line(buffer, prev.x, prev.y, next.x, next.y, width);
      prev = next;
   }
}

void print_digit(const uint8_t digit, const upoint_t glyph_size, const uint8_t width, upoint_t position) {
   // TODO: draw into viewport
   uint8_t buffer [MAXBUFFERX]; // for drawing characters
   const uint8_t *glyph = glyphs + letter[digit];
   uint8_t line;
   int8_t i;
   
   for (line = 0; line <= (glyph_size.y - 1) / 8; line++) {
       for (i = 0; i < glyph_size.x; i++) { // commented out because GCC disabled interrupts and wrote to reserved space...
          buffer[i] = 0;
       }
              /*  asm volatile("movw r30, %A0" "\n"
            "adiw r30, " NUMSTR(MAXBUFFERX) "\n" // constraint M!!!
            "eor __tmp_reg__, __tmp_reg__" "\n"
    "zero%=: st -Z, __tmp_reg__" "\n"
            "cp r30, %A0" "\n"
            "brne zero%=" "\n"
            "cp r31, %B0" "\n"
            "brne zero%=" "\n" : : "r" (buffer) : "r30", "r31");
    */
       if (digit < 10) {
           draw_glyph(buffer, glyph, glyph_size, width, 0, line * 8);    
       }
       set_column(position.x);
       set_row(position.y + line);
       for (i = 0; i < glyph_size.x; i++) {
           send_raw_byte(buffer[i], true);
       }
   }
}

