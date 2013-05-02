/*
    Copyright 2013 Paweł Czaplejewicz

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

#include "bcd.h"

bcd32_t int_to_bcd32(uint32_t value) {
    bcd32_t bcd = {.bin=0};
    
    // BCD conversion
    for (int8_t i = 31; ; i--) { //32 should be size of int - FRACBITS
        asm("lsl    %A0" "\n"
            "rol    %B0" "\n"
            "rol    %C0" "\n"
            "rol    %D0" "\n"
            "rol    %A1" "\n" // reduce this shit
            "rol    %B1" "\n"
            "rol    %C1" "\n"
            "rol    %D1" "\n" : "+r" (value), "+r" (bcd.bin));
        if (i == 0) {
            break;
        }
        
        // WARNING: Endianness
        // while ptr >= bcd.nibbles
        for (bcd_nibble_t* n_ptr = &(bcd.nibbles[3]); n_ptr >= &(bcd.nibbles[0]); n_ptr--) { // ptr points to MSB
        // roll two parts into one to save space. example below with printing
            uint8_t* ptr = (uint8_t*)n_ptr;
            if (((*ptr) + 0x03) & _BV(3)) {
                *ptr += 0x03;
            }
            if (((*ptr) + 0x30) & _BV(7)) {
                *ptr += 0x30;
            }
        }
        /*
        uint8_t tmp;
        asm("mov r30, %3" "\n"
            "inc r30" "\n"
"incloops%=: ld __tmp_reg__, -Z" "\n" // Z!!!
            "ldi %2, 0x03" "\n"
            "add __tmp_reg__, %2" "\n"
            "sbrc __tmp_reg__, 3" "\n"
            "st Z, __tmp_reg__" "\n"
            "ld __tmp_reg__, -Z" "\n" // Z!!!
            "ldi %2, 0x30" "\n"
            "add __tmp_reg__, %2" "\n"
            "sbrc __tmp_reg__, 7" "\n"
            "st Z, __tmp_reg__" "\n"
            "cp r30, %3" "\n"
            "brne incloops%=" "\n" : "+r" (bcd), "+z" (ptr), "=d" (tmp) : "x" (resptr)); */
    }    
    return bcd;
}
