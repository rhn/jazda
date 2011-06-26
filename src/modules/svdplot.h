/* Speed versus distance plot module.
Inserts blanks at each stop (determined by speed module with STOPPED_TIMEOUT)
REQUIRES: speed builtin

svd_on_pulse() MUST be called BEFORE speed_on_pulse, since speed module signals
a stop in a timer interrupt and svd detects a stop in pulse interrupt. At first
pulse after stop, the stopped flag is cleared by speed_on_pulse

CONSTANTS:
SVDPLOT_SIZE - number of bars (frames) to record speed
SVDPLOT_FRAME_PULSES - number of pulses composing an elementary distance (1 bar)
SVDPLOT_LENGTH_KM - length of the visible area of the plot. It is the base for
                    calculating SVDPLOT_FRAME_PULSES. Actual visible area will
                    be longer than this
SVDPLOT_SPEED_TRUNC - decreasing factor to fit speed into uint8_t TODO: eliminate
*/

#define SVDPLOT_LENGTH_KM 1 // distance axis length
#define SVDPLOT_SPEED_TRUNC 5
//#define SVDPLOT_FRAME_PULSES 4
#define SVDPLOT_FRAME_PULSES (SVDPLOT_LENGTH_KM * 1000000L) / (PLOT_SIZE * METRIC_PULSE_DIST)

volatile circular_buffer_t svd_averages = { .next_index = 0, .num_values = 0 };

volatile uint8_t svd_pulse_number; // in-frame number of current pulse
volatile uint16_t svd_previous_frame_time; // the last recorded pulse time

void svd_insert_average(const uint8_t value) {
    circular_buffer_insert_value(&svd_averages, value);
}

void svd_on_pulse(const uint16_t now) {
    if (oldest_pulse_index == 0) { // if first pulse after a stop
        svd_previous_frame_time = now; // set a new start time
        svd_pulse_number = 0; // clear counter
        svd_insert_average(0); // insert a space
    } else if (svd_pulse_number == SVDPLOT_FRAME_PULSES - 1) { // if last pulse of a frame
        uint16_t avg = get_int_average(now - svd_previous_frame_time, SVDPLOT_FRAME_PULSES); // TODO: move away to main loop
        svd_previous_frame_time = now;
        svd_pulse_number = 0; // clear counter
        
        // TRUNCATING AVERAGE
        avg >>= SVDPLOT_SPEED_TRUNC;
        
        // ROUNDING AVERAGE
        /*avg >>= SVDPLOT_SPEED_TRUNC - 1;
        if (avg & 1) {
          avg |= 0x00000010; // avg++; works better for longer types
        }
        avg >>= 1;*/
        
        svd_insert_average(avg);
    } else {
        svd_pulse_number++;
    }
}

void svd_redraw(const uint8_t force) {
    if ((svd_pulse_number == 0) || force) { // there's been a change, redraw
       draw_circular_buffer_plot(svd_averages);
    }
}

#define svd_signature {0b10000010, 0b01010101, 0b00101000, 0b00010000, 0b00001000, 0b11110100, 0b10010010, 0b01100001}

#define svd_record {&svd_redraw, &module_select_null, svd_signature}
