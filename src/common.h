#define true 1
#define false 0
#define STRVALUE(arg) #arg
#define NUMSTR(num) STRVALUE(num)
#define HIGH(PORT, PIN) PORT |= _BV(PIN)
#define LOW(PORT, PIN) PORT &= ~_BV(PIN)


typedef struct unsigned_point {
    uint8_t x;
    uint8_t y;
} upoint_t;

#define nibbles_t uint8_t
#define NIBBLES(num1, num2) (nibbles_t)((num1 << 4) | (num2 & 0b00001111))
