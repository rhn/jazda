/* ADVANCED OPTIONS */

/* REQUIRES:
preferences.h
*/

#ifdef CURRENT_SPEED
    #define SPEED_SIGNIFICANT_DIGITS 2 // 99km/h is good enough
    #define SPEED_FRACTION_DIGITS 1 // better than my sigma
    #define PULSE_TABLE_SIZE 3
    #define STOPPED_TIMEOUT 3 // seconds
#endif

#ifdef DISTANCE
    #define DIST_SIGNIFICANT_DIGITS 3 // 999km is good enough
    #define DIST_FRACTION_DIGITS 2 // same as my sigma
#endif

#ifdef SPEED_VS_DISTANCE_PLOT
    // TODO: compile-time notification that CURRENT_SPEED is required
    // TODO: vertical size. Currently hard-coded to 2 * 8
    // TODO: speed axis adjusting to maximum value
    #define SVDPLOT_SIZE 84 // size in pixels (distance axis)
    #define SVDPLOT_LENGTH_KM 4 // distance axis length
#endif
