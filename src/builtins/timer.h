/*
Timer management. Dummy ATM.
*/

void set_timer_callback(const uint16_t time, void (*callback)(void)) {
    set_trigger_time(time);
}

void speed_on_timeout(void);

inline void on_trigger(void) {
    clear_trigger();
    speed_on_timeout();
}
