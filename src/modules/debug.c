/* Dummy display module

Tuned for pcd8544 display width */

#include "../common.h"
#include "../actions.h"

#define BACKLIGHTDIR DDRC
#define BACKLIGHTPORT PORTC
#define BACKLIGHTPIN PC5

void debug_redraw(const uint8_t force) {
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


module_actions_t *debug_select(const uint8_t state) {
   if (state) {
       enable_backlight();
   }
   return NULL;
}

#define debug_signature {0b10011001, 0b01011010, 0b00100100, 0b00011000, 0b00100100, 0b01011010, 0b10011001}

#define debug_record {&debug_redraw, &debug_select, debug_signature}
