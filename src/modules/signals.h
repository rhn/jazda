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
Signal dispatchers.
Must be included after modules to be able to reference them.
*/

/* MODULE HOOKS */

#ifdef CRANK
   inline void on_crank_pulse(void);
   
   inline void on_crank_stop(void);
#endif

inline void on_wheel_pulse(void);

inline void on_each_second(void);

inline void on_wheel_stop(uint16_t now);
