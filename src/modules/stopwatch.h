/* STOPWATCH MODULE
resettable stopwatch.
Time is updated on each second interrupt, thanks to that it's displayed
instantly with little jitter.
*/

volatile time_storage_t stopwatch_time = {0, 0, 0};

void stopwatch_redraw(uint8_t force) {
   upoint_t position = {0, 5};
   upoint_t glyph_size = {8, 8};
   
   print_number(stopwatch_time.hours, position, glyph_size, 1, NIBBLEPAIR(4, 0));
   position.x += 4 * 9 + 2; // 4 chars + 2 extra pixels space
   print_number(stopwatch_time.minutes, position, glyph_size, 1, NIBBLEPAIR(1, 1)); // 1 fractional digit to preserve the "0" in front
   position.x += 2 * 9 + 2; // 2 chars + 2 extra pixels space
   print_number(stopwatch_time.seconds, position, glyph_size, 1, NIBBLEPAIR(1, 1)); // 1 fractional digit to preserve the "0" in front
}

void stopwatch_reset(void) {
   stopwatch_time.hours = 0;
   stopwatch_time.minutes = 0;
   stopwatch_time.seconds = 0;
}

module_actions_t *stopwatch_select(uint8_t state) {
   if (state) {
      stopwatch_reset();
   }
   return NULL;
}

void stopwatch_on_each_second(void) {
   if (oldest_pulse_index != 0) {
       time_storage_t new_time = stopwatch_time;
       if (new_time.seconds == 59) {
           new_time.seconds = 0;
           if (new_time.minutes == 59) {
               new_time.minutes = 0;
               new_time.hours++;
           } else {
               new_time.minutes++;
           }
       } else {
           new_time.seconds++;
       }
       stopwatch_time = new_time;
   }
}

#define stopwatch_signature {0b00111000, 0b01000100, 0b10000011, 0b10011111, 0b10001011, 0b01000100, 0b00111000, 0}

#ifdef COMBINED_RESET
    #define stopwatch_record {&stopwatch_redraw, &combined_reset_on_select, stopwatch_signature}    
#else
    #define stopwatch_record {&stopwatch_redraw, &stopwatch_select, stopwatch_signature}
#endif
