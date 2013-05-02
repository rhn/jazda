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

#ifndef __BCD_H__
#define __BCD_H__

#include <stdlib.h>

typedef struct bcd_nibble_struct {
    int high:4;
    int low:4;
} bcd_nibble_t;

typedef union bcd32_union {
    bcd_nibble_t nibbles[4];
    uint32_t bin;
} bcd32_t;

bcd32_t int_to_bcd32(uint32_t value);

#endif // __BCD_H__
