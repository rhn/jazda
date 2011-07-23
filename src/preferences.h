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

/* basic constants and switches, pin defines */

/* REQUIRES: none
*/

#define PULSEDIR DDRD
#define PULSEPORT PORTD
#define PULSEPIN PD2

#define CRANKDIR DDRD
#define CRANKPORT PORTD
#define CRANKPIN PD4

#define BUTTONDIR DDRD
#define BUTTONPORT PORTD
#define BUTTONPIN PD3

#define LEFTPIN PD7
#define LEFTPORT PIND
#define LEFTDIR DDRD

#define RIGHTPIN PD0
#define RIGHTPORT PIND
#define RIGHTDIR DDRD

#define SELECTPIN PD6
#define SELECTPORT PIND
#define SELECTDIR DDRD
  
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

/* special options */
#define HIGH_PRECISION_CALCULATIONS // makes speed calculation 32-bit instead of truncating to 16
#define LONG_CALCULATIONS // makes calculations possible on longer data types

/* PREFERENCES */
#define DEBUG

#define CRANK
#define DISTANCE
#define CURRENT_SPEED
#define SPEED_VS_DISTANCE_PLOT
#define STOPWATCH
#define MAXSPEED
#define AVGSPEED
#define COMBINED_RESET
#define SPEED_VS_TIME_PLOT
#define METRIC_PULSE_DIST 2133L // millimeters
