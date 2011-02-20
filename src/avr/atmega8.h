#define CLKDIR DDRC
#define CLKPORT PORTC
#define CLKPIN PC5

#define SDADIR DDRC
#define SDAPORT PORTC
#define SDAPIN PC4

#define D_CDIR DDRC
#define D_CPORT PORTC
#define D_CPIN PC3

#define SCEDIR DDRC
#define SCEPORT PORTC
#define SCEPIN PC2
   
#define RSTDIR DDRC
#define RSTPORT PORTC
#define RSTPIN PC0

#define HIGH_PRECISION_SPEED
#define MSTIMER
#define PULSEINT

#include "avr.h"

void setup_cpu(void) {
   // makes CPU clock 1 MHz
   // can't be done on this device
}
