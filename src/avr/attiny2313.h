#define CLKDIR DDRB
#define CLKPORT PORTB
#define CLKPIN PB1

#define RSTDIR DDRB
#define RSTPORT PORTB
#define RSTPIN PB2

#define SCEDIR DDRB
#define SCEPORT PORTB
#define SCEPIN PB3

#define SDADIR DDRB
#define SDAPORT PORTB
#define SDAPIN PB4

#define D_CDIR DDRB
#define D_CPORT PORTB
#define D_CPIN PB5

void setup_cpu(void) {
// makes CPU run at 1 MHz
// clock prescaler, device clock must be 8 MHz
  clock_prescale_set(clock_div_8);
}

