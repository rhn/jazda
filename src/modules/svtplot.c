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

/* Speed versus time plot

In the current form, it's intended to span less than a few hours. A single frame
calculation: take a pulse, wait until SVTPLOT_FRAME_TIME passes and a pulse
occurs OR until stopped, calculate speed during that time and display it.
therefore, the speed is always less than actual after a stop. FIXME: The less
pulses in last frame, the bigger the difference from actual speed.

Screen updates are not timer-driven, but they should tend to the desired timer
value on average.

Possible optimizations: pulse_count starts from -1 and becomes rotation_count
*/

#define SVTPLOT_LENGTH_SEC 84*2 // time axis length.
#define SVTPLOT_SPEED_TRUNC 5
//#define SVTPLOT_FRAME_TIME 2000
#define SVTPLOT_FRAME_TIME (uint16_t)(((uint32_t)SVTPLOT_LENGTH_SEC * ONE_SECOND) / PLOT_SIZE)

volatile circular_buffer_t svt_averages = { .next_index = 0, .num_values = 0 };

volatile uint8_t svt_pulse_count = 0;
volatile uint16_t svt_frame_start_time; // the first recorded pulse time from the last frame

void svt_insert_average(const uint8_t value) {
    circular_buffer_insert_value(&svt_averages, value);
}

void svt_update(const uint16_t now) {
    uint8_t rotations = svt_pulse_count - 1;
    // rotations will NOT always be > 0: the only possibility is that an
    // on_stop event occurs when rotations == 0 (2 consecutive updates). This
    // can happen when the timer just hits (last pulse is committed) and on_stop
    // fires
    if (rotations > 0) {
        uint16_t avg = get_average_speed(now - svt_frame_start_time, rotations);
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
    if (svt_pulse_count == 0) {
        // new frame just started, be if after a stop (update) or timer (update)
        svt_frame_start_time = now;
    }
    svt_pulse_count++;
    
    if (now - svt_frame_start_time > SVTPLOT_FRAME_TIME) {
        svt_update(now);
        svt_pulse_count = 1;
        svt_frame_start_time = now;
    }
}

void svt_on_stop(const uint16_t now) {
   svt_update(now);
   svt_insert_average(0);
   svt_pulse_count = 0;
}

void svt_redraw(const uint8_t force) {
    if ((svt_pulse_count <= 1) || force) { // there's been a change, redraw
       draw_circular_buffer_plot(svt_averages);
    }
}


#define svt_signature {0b10000010, 0b01010101, 0b00101000, 0b00010000, 0b00001000, 0b00010100, 0b11110010, 0b00010001}

#define svt_record {&svt_redraw, &module_select_null, svt_signature}
