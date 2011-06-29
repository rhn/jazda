/*
    Copyright 2011 rhn <gihu.rhn@porcupinefactory.org>

    This file is part of Jazda.

    Jazda is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Jazda is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Jazda.  If not, see <http://www.gnu.org/licenses/>.
*/

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
