#define CLKDIR DDRC
#define CLKPORT PORTC
#define CLKPIN PC5

#define RSTDIR DDRC
#define RSTPORT PORTC
#define RSTPIN PC3

#define SCEDIR DDRC
#define SCEPORT PORTC
#define SCEPIN PC2

#define SDADIR DDRC
#define SDAPORT PORTC
#define SDAPIN PC4

#define D_CDIR DDRC
#define D_CPORT PORTC
#define D_CPIN PC1   

void setup_cpu(void) {
   // makes CPU clock 1 MHz
   // can't be done on this device
}
