/*
    Copyright 2011 rhn <gihu.rhn@porcupinefactory.org>
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

/* basic constants and switches, pin defines */

/* REQUIRES: none
*/

// - Inputs

#define PULSEDIR DDRD
#define PULSEPORT PORTD
#define PULSEPIN PD2

#define CRANKDIR DDRD
#define CRANKPORT PIND
#define CRANKPIN PD1

// -- Button interrupt

#define BUTTONDIR DDRD
#define BUTTONPORT PORTD
#define BUTTONPIN PD3

// -- Buttons

#define LEFTPIN PD7
#define LEFTPORT PIND
#define LEFTDIR DDRD

#define RIGHTPIN PD0
#define RIGHTPORT PIND
#define RIGHTDIR DDRD

#define SELECTPIN PD6
#define SELECTPORT PIND
#define SELECTDIR DDRD

#define BACKLIGHTBTNPIN PD4
#define BACKLIGHTBTNPORT PIND
#define BACKLIGHTBTNDIR DDRD

// - Output
// -- Screen

#define CLKDIR DDRC
#define CLKPORT PORTC
#define CLKPIN PC4

#define SDADIR DDRC
#define SDAPORT PORTC
#define SDAPIN PC3

#define D_CDIR DDRC
#define D_CPORT PORTC
#define D_CPIN PC2

#define SCEDIR DDRC
#define SCEPORT PORTC
#define SCEPIN PC0
   
#define RSTDIR DDRC
#define RSTPORT PORTC
#define RSTPIN PC1

// -- Backlight
#define BACKLIGHTDIR DDRC
#define BACKLIGHTPORT PORTC
#define BACKLIGHTPIN PC5

/* special options */
#define HIGH_PRECISION_CALCULATIONS // makes speed calculation 32-bit instead of truncating to 16
#define LONG_CALCULATIONS // makes calculations possible on longer data types
#define FRAC_BITS 10 // Fixed-point fraction for PULSE_DIST and SPEED_FACTOR. WARNING! modifying may lead to overflow errors and better precision (most likely just the former). Less then 8 is not recommended if LONG_SPEED is used
#define ONE_SECOND 3695 // in timer ticks, calibrated for 1MHz/256 // TODO: autocalculate

/* PREFERENCES */
// #define DEBUG

#define CRANK
#define CADENCE
#define STORAGE
#define BACKLIGHT
#define DISTANCE
#define CURRENT_SPEED
#define SPEED_VS_DISTANCE_PLOT
#define STOPWATCH
#define MAXSPEED
#define AVGSPEED
#define COMBINED_RESET
#define SPEED_VS_TIME_PLOT
#define CONFIG
// #define MOD_BACKLIGHT // backlight control via module
#define BACKLIGHT_BUTTON // backlight control via hw button
//#define BACKLIGHT_VOLTAGE // shows voltage along with backlight
#define LCD_CLEAN


#define METRIC_PULSE_DIST 2133L // millimeters
