/* ----- PROJECT-DEPENDENT DECLARATIONS ---- */

#define EMUINT0PIN LEFTPIN
#define EMUINT0PORT LEFTPORT
#define EMUINT0DIR LEFTDIR
void EMUINT0_handler(uint8_t state) {
    on_left_button(state);
}

#define EMUINT1PIN RIGHTPIN
#define EMUINT1PORT RIGHTPORT
#define EMUINT1DIR RIGHTDIR
void EMUINT1_handler(uint8_t state) {
    on_right_button(state);
}

#define EMUINT2PIN SELECTPIN
#define EMUINT2PORT SELECTPORT
#define EMUINT2DIR SELECTDIR
void EMUINT2_handler(uint8_t state) {
    on_select_button(state);
}

/* THE ACTUAL LIBRARY */

volatile char emuintpinstate = 0;

void update_emuint_state(void) {
    #ifdef EMUINT0PIN
    if (EMUINT0PORT & 1<<EMUINT0PIN) {
        emuintpinstate |= 0b00000001;
    } else {
        emuintpinstate &= ~0b00000001;
    }
    #endif
    
    #ifdef EMUINT1PIN
    if (EMUINT1PORT & 1<<EMUINT1PIN) {
        emuintpinstate |= 0b00000010;
    } else {
        emuintpinstate &= ~0b00000010;
    }
    #endif
    
    #ifdef EMUINT2PIN
    if (EMUINT2PORT & 1<<EMUINT2PIN) {
        emuintpinstate |= 0b00000100;
    } else {
        emuintpinstate &= ~0b00000100;
    }
    #endif
    
    #ifdef EMUINT3PIN
    if (EMUINT3PORT & 1<<EMUINT3PIN) {
        emuintpinstate |= 0b00001000;
    } else {
        emuintpinstate &= ~0b00001000;
    }
    #endif
    
    #ifdef EMUINT4PIN
    if (EMUINT4PORT & 1<<EMUINT4PIN) {
        emuintpinstate |= 0b00010000;
    } else {
        emuintpinstate &= ~0b00010000;
    }
    #endif
    
    #ifdef EMUINT5PIN
    if (EMUINT5PORT & 1<<EMUINT5PIN) {
        emuintpinstate |= 0b00100000;
    } else {
        emuintpinstate &= ~0b00100000;
    }
    #endif
    
    #ifdef EMUINT6PIN
    if (EMUINT6PORT & 1<<EMUINT6PIN) {
        emuintpinstate |= 0b01000000;
    } else {
        emuintpinstate &= ~0b01000000;
    }
    #endif
    
    #ifdef EMUINT7PIN
    if (EMUINT7PORT & 1<<EMUINT7PIN) {
        emuintpinstate |= 0b10000000;
    } else {
        emuintpinstate &= ~0b10000000;
    }
    #endif
}

void emuint_dispatch(void) {
    char oldstate = emuintpinstate;
    update_emuint_state();    
        
    char statechange = oldstate ^ emuintpinstate;
    
    #ifdef EMUINT0PIN
    if (statechange & 0b00000001) {
        pulses++;
        EMUINT0_handler(emuintpinstate & 0b00000001);
    }
    #endif
    
    #ifdef EMUINT1PIN
    if (statechange & 0b00000010) {
        EMUINT1_handler(emuintpinstate & 0b00000010);
    }
    #endif

    #ifdef EMUINT2PIN
    if (statechange & 0b00000100) {
        EMUINT2_handler(emuintpinstate & 0b00000100);
    }
    #endif

    #ifdef EMUINT3PIN
    if (statechange & 0b00001000) {
        EMUINT3_handler(emuintpinstate & 0b00001000);
    }
    #endif

    #ifdef EMUINT4PIN
    if (statechange & 0b00010000) {
        EMUINT4_handler(emuintpinstate & 0b00010000);
    }
    #endif

    #ifdef EMUINT5PIN
    if (statechange & 0b00100000) {
        EMUINT5_handler(emuintpinstate & 0b00100000);
    }
    #endif
    
    #ifdef EMUINT6PIN
    if (statechange & 0b01000000) {
        EMUINT6_handler(emuintpinstate & 0b01000000);
    }
    #endif
    
    #ifdef EMUINT7PIN
    if (statechange & 0b10000000) {
        EMUINT7_handler(emuintpinstate & 0b10000000);
    }
    #endif
}
