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
Timer management, common to all architectures
*/

#define MAX_TIMERS 7
#define timer_handle_t int8_t

inline void timer_initialize(void);

inline void timer_dispatch(void);

timer_handle_t timer_set_callback(const uint16_t time, void (*callback)(void));

void timer_clear_callback(const timer_handle_t identifier);
