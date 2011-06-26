/* Speed versus time plot

In the current form, it's intended to span less than a few hours. A single frame
calculation: take a pulse, wait until SVTPLOT_FRAME_TIME passes and a pulse
occurs OR until stopped, calculate speed during that time.

Possible optimizations: pulse_count starts from -1 and becomes rotation_count
*/

#define SVTPLOT_LENGTH_SEC 84*5 // time axis length. Recommended at least size * 5, the slower the more for accurate calculations
#define SVTPLOT_SPEED_TRUNC 4
//#define SVDPLOT_FRAME_TIME 4
#define SVTPLOT_FRAME_TIME (SVTPLOT_LENGTH_SEC * ONE_SECOND) / PLOT_SIZE

volatile circular_buffer_t svt_averages = { .next_index = 0, .num_values = 0 };

volatile uint8_t svt_pulse_count = 0; // number of pulses caught in current frame
volatile uint16_t svt_previous_frame_start_time; // the first recorded pulse time from the last frame

void svt_insert_average(const uint8_t value) {
    circular_buffer_insert_value(&svt_averages, value);
}

void svt_update(const uint16_t now) {
    uint8_t rotations = svt_pulse_count - 1;
    if (rotations > 0) {
        uint16_t avg = get_int_average(now - svt_previous_frame_start_time, rotations);
        svt_pulse_count = 0; // clear counter
        
        // TRUNCATING AVERAGE
        avg >>= SVTPLOT_SPEED_TRUNC;
        
        // ROUNDING AVERAGE
        /*avg >>= SVDPLOT_SPEED_TRUNC - 1;
        if (avg & 1) {
          avg |= 0x00000010; // avg++; works better for longer types
        }
        avg >>= 1;*/
        
        svt_insert_average(avg);
    }
}

void svt_on_pulse(const uint16_t now) {
    if (svt_pulse_count == 0) { // initialize new frame
        svt_previous_frame_start_time = now;
        // pulse_count already initialized to 0
    }
    // if previous is true, this will never happen. ergo TODO: possible speed optimization
    if (now - svt_previous_frame_start_time > SVTPLOT_FRAME_TIME) {
        svt_update(now);
    } else {
        svt_pulse_count++;
    }
}

void svt_on_stop(const uint16_t now) {
   svt_update(now);
}

void svt_redraw(const uint8_t force) {
    if (force) {
        erase_module_screen();
    }
    if ((svt_pulse_count == 0) || force) { // there's been a change, redraw
       draw_circular_buffer_plot(svt_averages);
    }
}

module_actions_t *svt_select(const uint8_t state) {
    return NULL;
}

#define svt_signature {0b10000010, 0b01010101, 0b00101000, 0b00010000, 0b00001000, 0b00010100, 0b11110010, 0b00010001}

#define svt_record {&svt_redraw, &svt_select, svt_signature}
