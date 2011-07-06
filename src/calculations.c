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
    uint16_t get_rot_speed_long(const uint32_t pulse_distance, uint32_t time_amount, const uint16_t pulse_count) { 
       uint64_t distance;
       uint32_t speed;
       // pulse_distance is fixed point with FRAC_BITS fractional bits
       // pulse_count is integer
       // bits = big48b64b: big32b * big16b
       distance = ((uint64_t)pulse_distance) * pulse_count;
       
       // truncate values to be able to perform 32b division: find most
       // significant bit of distance and shift both time_amount and distance
       // to fit distance to 32b. time_amount can stay 32b
       
       // TODO: ASM optimization potential
       uint16_t mask = 0x8000; // most significant bit
       int8_t shift;
       // detect msb
       for (shift = 16; shift > 0; shift--) {
           if (mask & (distance >> 32)) {
              break;
           }
           mask >>= 1;
       }
       
       // TODO: possibly can be optimized
       time_amount >>= shift;
       distance >>= shift; 
       
       // distance is now 32b, time_amount is now 32b
       
       // bits = 32b: big32b / 32b
       speed = (uint32_t)distance / time_amount;

       // speed is fixed point with FRAC_BITS fractional bits       
       return speed >> FRAC_BITS;
    }
    
    uint16_t get_rot_speed(const uint32_t pulse_distance, const uint16_t time_amount, const uint8_t pulse_count) {
       return get_rot_speed_long(pulse_distance, time_amount, pulse_count);
    }
#else
    #ifdef HIGH_PRECISION_CALCULATIONS
        uint16_t get_rot_speed(const uint32_t pulse_distance, const uint16_t time_amount, const uint8_t pulse_count) {
           uint32_t speed;
           // pulse_distance is fixed point with FRAC_BITS fractional bits
           // pulse_count is integer
           // bits = small32b: ((32+b: big32b * small8b) / 16b)
           speed = ((uint32_t)(pulse_distance * pulse_count)) / time_amount;

           // speed is fixed point with FRAC_BITS fractional bits
           // bits = 16b: small32b >> ~10
           speed >>= FRAC_BITS;
           return speed;
        }
    #else
        uint16_t get_int_average(const uint32_t pulse_distance, const uint16_t time_amount, const uint8_t pulse_count) {
           // SPEED_FACTOR is fixed point with TIMER_BITS fractional bits truncated by SPEED_TRUNCATION_BITS bits
           // pulse_time is fixed point with TIMER_BITS fractional bits
           // to get correct division, pulse_time needs to be truncated by SPEED_TRUNCATION_BITS
           return ((uint16_t)(pulse_distance * (pulse_count))) / (time_amount >> SPEED_TRUNCATION_BITS);
           // calculation error: 1% at 30 km/h and proportional to square of speed
        }
    #endif
#endif

