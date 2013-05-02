#ifndef __BCD_H__
#define __BCD_H__

#include <stdlib.h>

typedef struct bcd_nibble_struct {
    int high:4;
    int low:4;
} bcd_nibble_t;

typedef union bcd32_union {
    bcd_nibble_t nibbles[4];
    uint32_t bin;
} bcd32_t;

bcd32_t int_to_bcd32(uint32_t value);

#endif // __BCD_H__
