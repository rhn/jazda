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

#ifdef LONG_CALCULATIONS
    /* use this to calculate averages over long periods of time/distances
    All this exists to avoid 64b / *b division which costs a whooping 3KB.
    pulse_distance MUST be a fixed point number with FRAC_BITS fractional bits
    TODO: investigate impact of placing >>FRAC_BITS in distance vs on return
    */
    uint16_t get_rot_speed_long(const uint32_t pulse_distance, uint32_t time_amount, const uint16_t pulse_count);
#endif

uint16_t get_rot_speed(const uint32_t pulse_distance, const uint16_t time_amount, const uint8_t pulse_count);
