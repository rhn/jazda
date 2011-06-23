/* Speed versus distance plot module.
Inserts blanks at each stop (determined by speed module with STOPPED_TIMEOUT)
REQUIRES: speed module

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

// TODO: vertical size. Currently hard-coded to 2 * 8
// TODO: speed axis adjusting to maximum value
#define SVDPLOT_SIZE 84 // size in pixels (distance axis)
#define SVDPLOT_LENGTH_KM 1 // distance axis length
#define SVDPLOT_SPEED_TRUNC 3
//#define SVDPLOT_FRAME_PULSES 4
#define SVDPLOT_FRAME_PULSES (SVDPLOT_LENGTH_KM * 1000000L) / (SVDPLOT_SIZE * METRIC_PULSE_DIST)
#define SVDPLOT_BASE_LINE 4


volatile uint8_t svd_averages[SVDPLOT_SIZE]; // newer frames have higher number
// speeds are truncated, 1 bit shift
// circular buffer
volatile uint8_t svd_next_average = 0; // index of the next average to be recorded
volatile uint8_t svd_average_frames = 0; // number of recorded frames

volatile uint8_t svd_pulse_number; // in-frame number of current pulse
volatile uint16_t svd_previous_frame_time; // the last recorded pulse time


void svd_insert_average(const uint8_t speed) {
    svd_averages[svd_next_average] = speed;
    svd_next_average++;
    if (svd_average_frames < SVDPLOT_SIZE) {
      svd_average_frames++;
    }
    if (svd_next_average == SVDPLOT_SIZE) {
      svd_next_average = 0;
    }
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

/**
Draws the plot over 2 lines. Performs no scaling of values - value of speed in
svd_averages will become the height of a bar in pixels.
*/

void svd_redraw(uint8_t force) {
    if ((svd_pulse_number == 0) || force) { // there's been a change, redraw
       for (uint8_t line = 0; line < 2; line++) {
         uint8_t maxheight = (2 - line) * 8;
         set_column(84 - svd_average_frames);
         set_row(line + SVDPLOT_BASE_LINE);
         
         // finds first valid frame number in the circular buffer
         int8_t current_frame = svd_next_average - svd_average_frames;
         if (current_frame < 0) {
             current_frame = SVDPLOT_SIZE + current_frame;
         }
         
         // prints the desired part of the plot (horizontally)
         for (uint8_t i = 0; i < svd_average_frames; i++)
         {
           uint8_t height = svd_averages[current_frame];
           current_frame++;
           if (current_frame == SVDPLOT_SIZE) {
               current_frame = 0;
           }

           uint8_t bar = 0xFF;
           if (height < maxheight) {
             bar <<= maxheight - height;
           }
           send_raw_byte(bar, true);
         }
       }
    }
}

module_actions_t *svd_select(const uint8_t state) {
    return NULL;
}

#define svd_signature {0b10000010, 0b01010101, 0b00101000, 0b00010000, 0b00001000, 0b11110100, 0b10010010, 0b01100001}

#define svd_record {&svd_redraw, &svd_select, svd_signature}
