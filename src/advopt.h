/* ADVANCED OPTIONS */

/* REQUIRES:

*/

#ifdef SPEED_VS_DISTANCE_PLOT
    // TODO: compile-time notification that CURRENT_SPEED is required
    // TODO: vertical size. Currently hard-coded to 2 * 8
    // TODO: speed axis adjusting to maximum value
    #define SVDPLOT_SIZE 84 // size in pixels (distance axis)
    #define SVDPLOT_LENGTH_KM 4 // distance axis length
#endif
