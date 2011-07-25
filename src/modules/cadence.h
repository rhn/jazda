/*
    Copyright 2011 Paweł Czaplejewicz

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

/* Cadence module
REQUIRES: crank
*/

#define CRANK_FACTOR ((((uint32_t)(ONE_SECOND)) << FRAC_BITS) * 60)
#define CADENCE_DIGITS NIBBLEPAIR(3, 0)

void cadence_on_crank_pulse(void);
void cadence_on_stop(void);
void cadence_redraw(const uint8_t force);

#define cadence_signature {0b10000000, 0b01100000, 0b00011000, 0b00000110, 0b00000001, 0b00000110, 0b00011000, 0b11100000}
#define cadence_record {&cadence_redraw, &module_select_null, cadence_signature}
