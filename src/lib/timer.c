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
    timer_handle_t id;
} timer_data_t;

volatile timer_data_t timer_array[MAX_TIMERS];
volatile uint8_t timer_count = 0;

timer_handle_t current_id = 0;

inline void timer_initialize(void) {
    setup_timer();
}

void timer_clear(uint8_t index) {
    timer_count--;
    for (uint8_t i = index; i < timer_count; i++) {
        timer_array[i] = timer_array[i + 1];
    }
    if (index == 0) {
        set_trigger_time(timer_array[0].time);
    }
}


// Assumption: at any time when the interrupt comes, timer_first_index points at a valid request (valid callback)
inline void timer_dispatch(void) {
    if (timer_count > 0) {
        timer_data_t request = timer_array[0];
        
        timer_clear(0);
        // timer state update finished
        
        /* STATE CONSISTENT */
        (*(request.callback))(); // set/clear timer may occur here but Assumption is satisfied so it's safe
    } else {
        // BAD NEWS: timer fired without a valid request registered
    }
}

int8_t timer_find_id(timer_handle_t id) {
    for (uint8_t i = 0; i < timer_count; i++) {
        if (timer_array[i].id == id) {
            return i;
        }
    }
    return -1;
}


// FIXME: this code could be executed either from interrupts only (safe) or from
// normal code and interrupts (RACE CONDITIONS!)
// returns < 0 if can't register timeout
timer_handle_t timer_set_callback(const uint16_t time, void (*callback)(void)) {
    if (timer_count == MAX_TIMERS) {
        return -1; // failed to find a slot to place the timer in
    }
    timer_data_t timer;
    timer.time = time;
    timer.callback = callback;
    // find the position
    uint16_t now = get_time();
    uint16_t ahead = time - now;
    
    uint8_t index;
    // find index
    for (index = 0; index < timer_count; index++) {
        uint16_t next_ahead = timer_array[index].time - now;
        if (next_ahead > ahead) { // next index is next indeed
            break;
        }
    }

    // rewrite following timers
    for (uint8_t i = timer_count; i > index; i--) {
        timer_array[i] = timer_array[i - 1];
    }
    
    // find valid id
    do {
        timer.id = current_id;
        current_id += 2; // to avoid ever getting the value of -1
    } while (timer_find_id(timer.id) >= 0);
    
    // insert new timer request
    timer_array[index] = timer;
    timer_count++;
    
    if (index == 0) {
        set_trigger_time(timer.time);
    }
    
    return timer.id;
}

void timer_clear_callback(const timer_handle_t identifier) {
    int8_t index = timer_find_id(identifier);
    if (index >= 0) {
        timer_clear(index);
    }
}
