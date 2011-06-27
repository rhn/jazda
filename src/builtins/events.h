void events_redraw(void) {
   static uint16_t count = 0;
   upoint_t position = {0, 2};
   upoint_t glyph_size = {8, 8};
   print_number(debug_variable, position, glyph_size, 1, 4<<4);
   count++;
}
