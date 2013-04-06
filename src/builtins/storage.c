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

#include "storage.h"
#include "../builtins/distance.h"
// #include "../arch/eeprom.h" // future sane include system

// offsets in bytes
#define STORAGE_VERSION_OFFSET ((uint8_t*)0)
#define STORAGE_PULSE_DISTANCE_OFFSET ((uint16_t*)1)

// TODO: define storage structure and offsets based on it

void storage_setup(void) {
    // Nothing to do... if MMC is in use, initialize it here
}

void storage_reset(void) {
    eeprom_update_byte(STORAGE_VERSION_OFFSET, STORAGE_VERSION);
    storage_save_pulse_distance();
}

void storage_load(void) {
    if (eeprom_read_byte(STORAGE_VERSION_OFFSET) != STORAGE_VERSION) {
        // FIXME: should display some message/dialog instead
        storage_reset();
    }
    #ifndef CONSTANT_PULSE_DISTANCE
        pulse_dist_mm_t mmpd = eeprom_read_word(STORAGE_PULSE_DISTANCE_OFFSET);
        pulse_distance_set_metric(mmpd);
    #endif
}

void storage_save_pulse_distance(void) {
    eeprom_update_word(STORAGE_PULSE_DISTANCE_OFFSET, millimeters_pulse_distance);
}
