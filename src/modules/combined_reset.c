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
File responsible for combined reset of modules. THIS IS NOT A MODULE on its own.
TODO
This file should be made into a module grouping all affected submodules, by
exposing own module_actions_t on select and own menu redraw function (or icons,
depending on future API).
*/

#ifdef MAXSPEED
    void maxspeed_reset(void);
#endif
#ifdef AVGSPEED
    void avgspeed_reset(void);
#endif
#ifdef DISTANCE
    void distance_reset(void);
#endif
#ifdef STOPWATCH
    void stopwatch_reset(void);
#endif

module_actions_t *combined_reset_on_select(uint8_t state) {
    if (!state) {
        #ifdef MAXSPEED
            maxspeed_reset();
        #endif
        #ifdef AVGSPEED
            avgspeed_reset();
        #endif
        #ifdef DISTANCE
            distance_reset();
        #endif
        #ifdef STOPWATCH
            stopwatch_reset();
        #endif
    }
    return NULL;
}
