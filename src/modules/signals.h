/*
Signal dispatchers.
Must be included after modules to be able to reference them.
*/

/* MODULE HOOKS */

inline void on_pulse(void) {
// speed interrupt
  uint16_t now = get_time();
  #ifdef DISTANCE
    distance_on_pulse();
  #endif
  #ifdef CURRENT_SPEED
    speed_on_pulse(now);
  #endif
  #ifdef SPEED_VS_TIME_PLOT
    svt_on_pulse(now);
  #endif
}

inline void on_each_second(void) {
   #ifdef STOPWATCH
      stopwatch_on_each_second();
   #endif
}

inline void on_stop(uint16_t now) {
    #ifdef SPEED_VS_TIME_PLOT
        svt_on_stop(now);
    #endif
}
