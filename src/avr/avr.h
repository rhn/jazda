#include <avr/interrupt.h>

/* AVR-specific routines */

/* REQUIRES:
#defines regarding interrupts
*/

#ifdef PULSEINT
    #define PULSEDIR DDRD
    #define PULSEPORT PORTD
    #define PULSEPIN PD2

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
#endif

#ifdef MSTIMER
    void setup_timer(void) {
    // sets up millisecond precision 16-bit timer
    /* TODO: consider using 8-bit timer overflowing every (half) second + 4-bit USI*/
    // set up prescaler to /1024 XXX CPU must be 8 MHz...
      uint8_t clksrc;
      clksrc = TCCR1B;
      clksrc |= 1<<CS12 | 1<<CS10;
      clksrc &= ~(1<<CS11);
      TCCR1B = clksrc;
    }

    inline uint16_t get_time() { // XXX: use it!!!
      return TCNT1;
    }

    void set_trigger_time(const uint16_t time) { // XXX: optimize: inline
      OCR1A = time;
      TIMSK |= 1 << OCIE1A;
    }
    
    inline void on_trigger(void);
    
    ISR(TIMER1_COMPA_vect) {
        on_trigger();
    }
#endif
