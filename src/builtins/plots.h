/* builtin managing plots
// TODO: vertical size. Currently hard-coded to 2 * 8
// TODO: speed axis adjusting to maximum value
// TODO: plot position. Currently hardcoded to use with modules
*/

#define PLOT_SIZE 84
#define PLOT_BASE_LINE 4
#define PLOT_LINE_HEIGHT 2

typedef struct circular_buffer {
    uint8_t values[PLOT_SIZE];
    uint8_t next_index = 0; // index of the next value to be recorded
    uint8_t num_values = 0; // number of currently stored value
} circular_buffer_t;

void circular_buffer_insert_value(circular_buffer_t *buffer, const uint8_t value) {
    (*buffer).values[(*buffer).next_index] = value;
    (*buffer).next_index++;
    if ((*buffer).num_values < PLOT_SIZE) {
      (*buffer).num_values++;
    }
    if ((*buffer).next_index == PLOT_SIZE) {
      (*buffer).next_index = 0;
    }
}

/**
Draws the plot over 2 lines. Performs no scaling of values - value of speed in
svd_averages will become the height of a bar in pixels.
*/
void draw_circular_buffer_plot_line(const circular_buffer_t buffer, const uint8_t line) {
  uint8_t maxheight = (PLOT_LINE_HEIGHT + 1 - line) * 8;
  
  // finds first valid frame number in the circular buffer
  int8_t current_frame = buffer.next_index - buffer.num_values;
  if (current_frame < 0) {
      current_frame = PLOT_SIZE + current_frame;
  }
  
  // prints the desired part of the plot (horizontally)
  for (uint8_t i = 0; i < buffer.num_values; i++)
  {
    uint8_t height = buffer.values[current_frame];
    current_frame++;
    if (current_frame == PLOT_SIZE) {
        current_frame = 0;
    }
 
    uint8_t bar = 0xFF;
    if (height < maxheight) {
      bar <<= maxheight - height;
    }
    send_raw_byte(bar, true);
  }
}
