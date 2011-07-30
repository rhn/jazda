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

/*
Timer management
This portion is architecture-independent, but not architecture-suited. Perhaps
should be moved to some sort of arch/lib place.
*/

#include "timer.h"


/* linked-list containing timer entries
    callback=NULL, next>=0: part of the chain but not active
    callback=NULL, next<0: not part of the chain
*/

typedef struct timer_data {
    uint16_t time;
    void (*callback)(void);
    timer_handle_t next;
} timer_data_t;

volatile timer_data_t timer_array[MAX_TIMERS];
volatile timer_handle_t timer_first_index = -1;


inline void timer_initialize(void) {
    for (uint8_t i = 0; i < MAX_TIMERS; i++) {
        timer_array[i].next = -1;
    }
    setup_timer();
}

// removes invalid/expired requests from the beginning of the chain and sets a new timeout request
void timer_update_chain(void) {
    timer_handle_t index = timer_first_index;

    while (index >= 0) {
        if (timer_array[index].callback != NULL) {
            break;
        } else {
            timer_handle_t next_index = timer_array[index].next;
            timer_array[index].next = -1; // remove the entry permanently
            index = next_index;
        }
    }
    
    if (index >= 0) {
        // FIXME: detect timer requests between timer interrupt and this point
        set_trigger_time(timer_array[index].time); // can't be set earlier...
    }
    
    timer_first_index = index;
}

// Assumption: at any time when the interrupt comes, timer_first_index points at a valid request (valid callback)
extern volatile uint8_t event_count;
extern volatile uint16_t event_value;
inline void timer_dispatch(void) {
    timer_handle_t index = timer_first_index;

    if (index >= 0) {
        timer_data_t request = timer_array[index];
        
        timer_array[index].callback = NULL;
        
        // cleanup to point at a valid callback function to satisfy Assumption
        timer_update_chain();
        // timer state update finished
        
        /* STATE CONSISTENT */
        (*(request.callback))(); // set/clear timer may occur here but Assumption is satisfied so it's safe
    } else {
        // BAD NEWS: timer fired without a valid request registered
    }
}

timer_handle_t timer_find_empty_slot(void) {
    for (uint8_t i = 0; i < MAX_TIMERS; i++) {
        if ((timer_array[i].callback == NULL) && (timer_array[i].next < 0)) {
            return i;
        }
    }
    return -1;
}
void enable_backlight(void);
// FIXME: this code could be executed either from interrupts only (safe) or from
// normal code and interrupts (RACE CONDITIONS!)
// returns < 0 if can't register timeout
timer_handle_t timer_set_callback(const uint16_t time, void (*callback)(void)) {
    timer_handle_t index = timer_find_empty_slot();
    event_count++;
    if (index < 0) {
        return -1; // failed to find a slot to place the timer in
    }
    timer_data_t timer;
    timer.time = time;
    timer.callback = callback;
    // find the position
    uint16_t now = get_time();
    uint16_t ahead = time - now;

    timer_handle_t prev_index = -1;
    timer_handle_t next_index = timer_first_index;
    uint8_t i = 0;
    while (next_index >= 0) { // statements separate to achieve predictable evaluation order
        uint16_t next_ahead = timer_array[next_index].time - now;
        if (next_ahead > ahead) { // next index is next indeed
            break;
        }
        i++;
        if (i > 10) {
            event_value = timer_array[0].next;
            return -1;
        }
        prev_index = next_index;
        next_index = timer_array[prev_index].next;
    }
    timer.next = next_index;
    timer_array[index] = timer;
    if (prev_index < 0) {
        set_trigger_time(time);
        timer_first_index = index;
    } else {
        timer_array[prev_index].next = index;
    }

    return index;
}

void timer_clear_callback(const timer_handle_t identifier) {
    if (identifier >= 0) {
        timer_array[identifier].callback = NULL;
        if (identifier == timer_first_index) {
            timer_update_chain();
        }
    }
}
