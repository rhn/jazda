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

typedef struct signed_point {
    int8_t x;
    int8_t y;
} point_t;

typedef struct module_actions {
    void (*redraw)(void);
    void (*button_left)(uint8_t state);
    void (*button_right)(uint8_t state);
    struct module_actions * (*button_select)(uint8_t state);
} module_actions_t;

typedef struct module_record {
    void (*redraw)(void);
} module_record_t;

#define nibblepair_t uint8_t
#define NIBBLEPAIR(num1, num2) (nibblepair_t)((num1 << 4) | (num2 & 0x0F))
