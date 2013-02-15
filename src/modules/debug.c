/* Dummy display module

Tuned for pcd8544 display width */

#include "../common.h"
#include "../actions.h"

#ifdef BACKLIGHT_VOLTAGE
    #include "../display/drawing.h"
#endif

#define BACKLIGHTDIR DDRC
#define BACKLIGHTPORT PORTC
#define BACKLIGHTPIN PC5

#ifdef BACKLIGHT_VOLTAGE
    uint16_t m_debug_voltage = 0;
    // FIXME: make glyphs work
//    __attribute__((progmem)) const uint8_t m_debug_symbol_V[] = {POINT(0, 0, 1), POINT(4, 15, 1), POINT(7, 0, 1), 0};
#endif

void debug_redraw(const uint8_t force) {
   #ifdef BACKLIGHT_VOLTAGE
       uint16_t voltage = get_battery_voltage();
       if ((voltage != m_debug_voltage) || force ) {
           uint8_t buffer[8] = {0b00000111,
                                0b00111000,
                                0b11000000,
                                0b00111000,
                                0b00000111,
                                0, 0, 0};
           m_debug_voltage = voltage;
           upoint_t position = {0, 4};
           upoint_t glyph_size = {8, 8};
           print_number(voltage, position, glyph_size, 1, (number_display_t){.integer=1, .fractional=2});
           
           // draw dot
           set_column(position.x + glyph_size.x);
           set_row(position.y + 1);
           send_raw_byte(0b1, true);
           
           // render V
//           draw_glyph(buffer, m_debug_symbol_V, glyph_size, 1, 0, 0);
           
           // display V
           position.x += 3 * (glyph_size.x + 1);
           set_row(position.y);
           set_column(position.x);
           
           for (uint8_t i = 0; i < 8; i++) {
               send_raw_byte(buffer[i], true);
           }
       }
       
   #endif
}

void enable_backlight(void) {
  static int state = 0;
  BACKLIGHTDIR |= 1<<BACKLIGHTPIN;
  if (state++ % 2) {
    LOW(BACKLIGHTPORT, BACKLIGHTPIN);
  } else {
    HIGH(BACKLIGHTPORT, BACKLIGHTPIN);
  }
}


const module_actions_t* debug_select(const uint8_t state) {
   if (state) {
       enable_backlight();
   }
   return NULL;
}

#define debug_signature {0b10011001, 0b01011010, 0b00100100, 0b00011000, 0b00100100, 0b01011010, 0b10011001}

#define debug_record {&debug_redraw, &debug_select, debug_signature}
