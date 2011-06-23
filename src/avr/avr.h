#include <avr/interrupt.h>

/* AVR-specific routines */

/* REQUIRES:
AVR #defines regarding interrupts

TODO: chip-specific functions to chip-specific files
*/

volatile uint16_t extended_time = 0;

void setup_pulse(void) {
  /* -----  hwint button */  
  PULSEDIR &= ~(1<<PULSEPIN); /* set PD2 to input */
  PULSEPORT |= 1<<PULSEPIN; // enable pullup resistor
    
  // interrupt on INT0 pin falling edge (sensor triggered)
  MCUCR |= (1<<ISC01);
  MCUCR &= ~(1<<ISC00);
  /* ------ end */

  // turn on interrupts!
  GIMSK |= (1<<INT0);
}

inline void on_pulse(void);

ISR(INT0_vect) {
    on_pulse();
}


void setup_buttons(void) {
  /* -----  hwint button */  
  BUTTONDIR &= ~(1<<BUTTONPIN); /* set PD3 to input */
  BUTTONPORT |= 1<<BUTTONPIN; // enable pullup resistor
  
  // emuint buttons
  LEFTDIR &= ~(1<<LEFTPIN);
  LEFTPORT |= 1<<LEFTPIN;

  RIGHTDIR &= ~(1<<RIGHTPIN);
  RIGHTPORT |= 1<<RIGHTPIN;
  
  SELECTDIR &= ~(1<<SELECTPIN);
  SELECTPORT |= 1<<SELECTPIN;
  
  // interrupt on INT1 pin both edges (emuint state change)
  MCUCR |= (1<<ISC10);
  MCUCR &= ~(1<<ISC11);
  /* ------ end */

  // turn on interrupts!
  GIMSK |= (1<<INT1);
}

void emuint_dispatch(void);

volatile uint8_t pulses = 0;

ISR(INT1_vect) {
  emuint_dispatch();
}


void setup_timer(void) {
// sets up millisecond precision 16-bit timer
/* TODO: consider using 8-bit timer overflowing every (half) second + 4-bit USI*/
// set up prescaler to /1024 XXX CPU must be 8 MHz...
  uint8_t clksrc;
  clksrc = TCCR1B;
  clksrc |= 1<<CS12 | 1<<CS10;
  clksrc &= ~(1<<CS11);
  TCCR1B = clksrc;

  TIMSK |= 1 << TOIE1; // enable overflow interrupt
  
  // set up each second timer
  OCR1B = TCNT1 + ONE_SECOND;
  TIMSK |= 1 << OCIE1B;
}

inline uint16_t get_time() {
  return TCNT1;
}

uint32_t get_extended_time() {
  return (((uint32_t)extended_time) << 16) | TCNT1; // TODO: may be possible to optimize in asm
}

void set_trigger_time(const uint16_t time) { // XXX: optimize: inline
  OCR1A = time;
  TIMSK |= 1 << OCIE1A;
}

inline void speed_on_trigger(void);

ISR(TIMER1_COMPA_vect) {
    speed_on_trigger();
}

ISR(TIMER1_OVF_vect) {
    extended_time++;
}

inline void on_each_second();

ISR(TIMER1_COMPB_vect) {
    on_each_second();
    OCR1B += ONE_SECOND;
}
