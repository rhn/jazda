/* basic constants and switches, pin defines */

/* REQUIRES: none
*/

#define PULSEDIR DDRD
#define PULSEPORT PORTD
#define PULSEPIN PD2

#define BUTTONDIR DDRD
#define BUTTONPORT PORTD
#define BUTTONPIN PD3

#define LEFTPIN PD5
#define LEFTPORT PIND
#define LEFTDIR DDRD

#define RIGHTPIN PD6
#define RIGHTPORT PIND
#define RIGHTDIR DDRD

#define SELECTPIN PD7
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
#define HIGH_PRECISION_SPEED // makes speed calculation 32-bit instead of truncating to 16

/* PREFERENCES */
#define DEBUG

#define DISTANCE
#define CURRENT_SPEED
#define SPEED_VS_DISTANCE_PLOT
#define STOPWATCH
#define MAXSPEED
#define AVGSPEED
#define LONG_SPEED
#define METRIC_PULSE_DIST 2133L // millimeters
