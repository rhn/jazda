/*
File responsible for combined reset of moules. THIS IS NOT A MODULE on its own.
TODO
This file should be made into a module grouping all affected submodules, by
exposing own module_actions_t on select and own menu redraw function (or icons,
depending on future API).
*/

#ifdef MAXSPEED
    void maxspeed_reset(void);
#endif
#ifdef AVGSPEED
    void avgspeed_reset(void);
#endif
#ifdef DISTANCE
    void distance_reset(void);
#endif
#ifdef STOPWATCH
    void stopwatch_reset(void);
#endif

module_actions_t *combined_reset_on_select(uint8_t state) {
    #ifdef MAXSPEED
        maxspeed_reset();
    #endif
    #ifdef AVGSPEED
        avgspeed_reset();
    #endif
    #ifdef DISTANCE
        distance_reset();
    #endif
    #ifdef STOPWATCH
        stopwatch_reset();
    #endif
    return NULL;
}
