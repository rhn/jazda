#include <avr/pgmspace.h>

/* Drawing on a monochrome line-based 8-bit height screen procedures */

/* REQUIRES:
pcd8544.h or compatible
*/

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
       }    
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

void print_number(uint32_t bin, upoint_t position, const upoint_t glyph_size, const uint8_t width, const nibblepair_t digits) {
// prints a number, aligned to right, throwing in 0's when necessary
 // TODO: fake decimal point?
// TODO: move to 16-bit (ifdef 32?)
    // integer only
    uint32_t bcd = 0;
    register uint8_t *ptr;
    uint8_t frac_digits = digits & 0x0F;
    uint8_t all_digits = (digits >> 4) + frac_digits;

    for (int8_t i = 31; ; i--) { //32 should be size of int - FRACBITS
        asm("lsl    %A0" "\n"
            "rol    %B0" "\n"
            "rol    %C0" "\n"
            "rol    %D0" "\n"
            "rol    %A1" "\n" // reduce this shit
            "rol    %B1" "\n"
            "rol    %C1" "\n"
            "rol    %D1" "\n" : "+r" (bin), "+r" (bcd));
        if (i == 0) {
            break;
        }
        
        // WARNING: Endianness
        // while (ptr > &bcd)
        for (ptr = ((uint8_t*)&bcd) + 3; ptr >= ((uint8_t*)&bcd); ptr--) { //ptr points to MSB
        // roll two parts into one to save space. example below with printing
//            print_digit(ptr - (uint8_t*)&bcd, 8, 8, 1);
            if (((*ptr) + 0x03) & _BV(3)) {
                *ptr += 0x03;
            }
            if (((*ptr) + 0x30) & _BV(7)) {
                *ptr += 0x30;
            }
        }
        /*
        uint8_t tmp;
        asm("mov r30, %3" "\n"
            "inc r30" "\n"
"incloops%=: ld __tmp_reg__, -Z" "\n" // Z!!!
            "ldi %2, 0x03" "\n"
            "add __tmp_reg__, %2" "\n"
            "sbrc __tmp_reg__, 3" "\n"
            "st Z, __tmp_reg__" "\n"
            "ld __tmp_reg__, -Z" "\n" // Z!!!
            "ldi %2, 0x30" "\n"
            "add __tmp_reg__, %2" "\n"
            "sbrc __tmp_reg__, 7" "\n"
            "st Z, __tmp_reg__" "\n"
            "cp r30, %3" "\n"
            "brne incloops%=" "\n" : "+r" (bcd), "+z" (ptr), "=d" (tmp) : "x" (resptr)); */
    }    
 
    uint8_t tmp;
    uint8_t print = 0; // 0: don't print
                       // 1: leave space
                       // 2: print
    
    ptr = ((uint8_t*)&bcd) + 3;
    
    for (uint8_t i = 8; i > 0; i--) {
        if (i & 1) {
            tmp = (*ptr) & 0x0F;
            ptr--;
        } else {
            tmp = (*ptr) >> 4;
        }
        if (tmp) {
            print = 2;
        }
        if (i == frac_digits + 1) { // a pre-point number hit
            print = 2;
        }
        if ((i <= all_digits) && (print < 2)) {
            print = 1;
            tmp = 10;
        }
        if (print) {
            print_digit(tmp, glyph_size, width, position);
            position.x += glyph_size.x + width;
        }
    } // 1452 bytes
    /*
    for (ptr = ((uint8_t*)&bcd) + 3; ptr >= ((uint8_t*)&bcd); ptr--) { //ptr points to MSB
        tmp = *ptr;
        if (tmp) {
            print = true;
        }
        if (print) {
            if (tmp >> 4) {
                print_digit(tmp >> 4, 8, 8, 1);
            }
            if (tmp & 0x0F) {
                print_digit(tmp & 0x0F, 8, 8, 1);
            }
        }
    } 1474 bytes*/
}
