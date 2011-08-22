#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

/* EXAMPLE UNFINISHED

Interrupt-driven blinking and switching diodes

Features: Pin change interrupt, hardware interrupt, timer overflow interrupt, sleep mode, pin output, inline assembly

Connect a diode to PB2 (pin change interrupt)
Connect a diode to PD5 (timer overflow)
Connect diodes to PB1, PD4, PD6 (hardware interrupt)

Connect a button to PD2 (INT0)
Connect a button to PB0 (PCINT)
*/

volatile uint8_t state=0;
volatile uint8_t pcstate=0;
volatile uint16_t hiclock=0;

ISR(INT0_vect) {
  state++;
}

ISR(PCINT_vect) {
    pcstate++;
    if (pcstate % 2) {
      PORTB &= ~_BV(PB2); /* LED on */
    } else {
      PORTB |= _BV(PB2); /* LED off */
    }
}

ISR(TIMER1_OVF_vect) {
    hiclock++;
    
    if (hiclock % 2) {
        PORTD &= ~_BV(PD5); /* LED on */
    } else {
        PORTD |= _BV(PD5); /* LED off */
    }
}

uint32_t time() {
   uint32_t ret = TCNT1;
   asm volatile("movw %C0, %A1" : "+r" (ret) : "r" (hiclock));
   return ret;// + TCNT1;
}

int main(void) {
  uint8_t clksrc;
  DDRD |= 1<<PD4; /* set Pd4 to output */
  DDRD |= 1<<PD5; /* set Pd5 to output */
  DDRD |= 1<<PD6; /* set Pd6 to output */
  DDRB |= 1<<PB1; /* set Pb1 to output */
  DDRB |= 1<<PB2; /* set Pb1 to output */
  DDRB &= ~(1<<PB0); /* set Pb0 to input */
  
  DDRD &= ~(1<<PD2); /* set PD2 to input */
  PORTD |= 1<<PD2; // enable pullup resistor
  PORTB |= 1<<PB0; // enable pullup resistor
  
  //set prescaler for 16bit clock to 1024
  clksrc = TCCR1B;
  clksrc |= 1<<CS12 | 1<<CS10;
  clksrc &= ~(1<<CS11);
  TCCR1B = clksrc;

//pin change interrupt mask  
  PCMSK |= (1<<PCINT0);
  
  // interrupt on INT0 pin falling edge (sensor triggered)
  MCUCR = (1<<ISC01) | (1<<ISC00);

  // turn on interrupts!
  GIMSK |= (1<<INT0) | (1<<PCIE);
  TIMSK = _BV(TOIE1);
  sei();
  while(1) {
      if (state % 3 == 1)
      {
        PORTD &= ~(1<<PD6); /* LED on */
        PORTD |= 1<<PD4; /* LED off */
        PORTB |= 1<<PB1; /* LED off */
      } else if (state % 3) {
        PORTB |= 1<<PB1; /* LED off */
        PORTD |= 1<<PD6; /* LED off */
        PORTD &= ~(1<<PD4); /* LED on */
      } else {
        PORTD |= 1<<PD6; /* LED off */
        PORTB &= ~(1<<PB1); /* LED on */
        PORTD |= 1<<PD4; /* LED off */
      }
      state = time();
  }
  while (1) {
      sleep_mode();
  }
}

