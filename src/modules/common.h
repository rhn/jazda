void erase_screen(uint8_t count) {
    for (; count > 0; count--) {
        send_raw_byte(0, true);
    }
}

void erase_module_screen(void) {
    set_row(4);
    set_column(0);
    erase_screen(2*84);
}

#define MODULE_BASE_LINE 4

void draw_circular_buffer_plot(const circular_buffer_t buffer) {
    for (uint8_t line = 0; line < PLOT_LINE_HEIGHT; line++) {
        set_column(84 - buffer.num_values);
        set_row(line + MODULE_BASE_LINE);
        draw_circular_buffer_plot_line(buffer, line);
    }
}

module_actions_t *module_select_null(const uint8_t state) {
    return NULL;
}
