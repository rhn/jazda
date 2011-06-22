/* Dummy display module

Tuned for pcd8544 display width */

#define BACKLIGHTDIR DDRC
#define BACKLIGHTPORT PORTC
#define BACKLIGHTPIN PC5

void debug_redraw(void) {
   uint8_t i;
   set_row(4);
   set_column(0);
   for (i = 0; i < 84*2; i++) {
      send_raw_byte(0, true);
   }
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
