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
    int8_t next;
} timer_data_t;

volatile timer_data_t timer_array[MAX_TIMERS];
volatile int8_t timer_first_index = -1;


inline void timer_initialize(void) {
    for (uint8_t i = 0; i < MAX_TIMERS; i++) {
        timer_array[i].next = -1;
    }
    setup_timer();
}

// removes invalid/expired requests from the beginning of the chain and sets a new timeout request
void timer_update_chain(void) {
    int8_t index = timer_first_index;
    void (*callback)(void);
    do {
        if (index < 0) {
            break;
        }
        callback = timer_array[index].callback;
        int8_t next_index = timer_array[index].next;

        timer_array[index].next = -1; // remove the entry permanently

        index = next_index;
    } while (callback == NULL);
    
    if (index >= 0) {
        // FIXME: detect timer requests between timer interrupt and this point
        set_trigger_time(timer_array[index].time); // can't be set earlier...
    }
    
    timer_first_index = index;
}

// Assumption: at any time when the interrupt comes, timer_first_index points at a valid request (valid callback)
extern volatile uint8_t event_count;

inline void timer_dispatch(void) {
    int8_t index = timer_first_index;
    event_count++;
    if (index >= 0) {
        timer_data_t request = timer_array[index];
        
        timer_array[index].callback = NULL;
        
        /* STATE CONSISTENT */
        (*(request.callback))(); // WARNING: set/clear timer may occur here
        /* STATE CONSISTENT */
        
        // cleanup to point at a valid callback function
        timer_update_chain();
    } else {
        // BAD NEWS: timer fired without a valid request registered
    }
}

int8_t timer_find_empty_slot(void) {
    for (uint8_t i = 0; i < MAX_TIMERS; i++) {
        if ((timer_array[i].callback == NULL) && (timer_array[i].next < 0)) {
            return i;
        }
    }
    return -1;
}

// FIXME: this code could be executed either from interrupts only (safe) or from
// normal code and interrupts (RACE CONDITIONS!)
// returns < 0 if can't register timeout
int8_t timer_set_callback(const uint16_t time, void (*callback)(void)) {
    int8_t index = timer_find_empty_slot();
    if (index < 0) {
        return -1; // failed to find a slot to place the timer in
    }

    timer_data_t timer;
    timer.time = time;
    timer.callback = callback;
    // find the position

    int8_t prev_index = -1;
    int8_t next_index = timer_first_index;
    
    do {
        if (next_index < 0) {
            break;
        }
        prev_index = next_index;
        next_index = timer_array[prev_index].next;
    } while (timer_array[next_index].time < time);

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

void timer_clear_callback(const int8_t identifier) {
    if (identifier >= 0) {
        timer_array[identifier].callback = NULL;
        if (identifier == timer_first_index) {
            timer_update_chain();
        }
    }
}
