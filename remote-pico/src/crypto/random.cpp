#include <pico/stdlib.h>
#include <hardware/regs/rosc.h>
// #include <hardware/regs/addressmap.h>

#include "crypto.h"


static uint8_t
random_byte()
{
    int random = 0;
    volatile uint32_t * rnd_reg= (uint32_t *) (ROSC_BASE + ROSC_RANDOMBIT_OFFSET);
    
    for(int i=0; i<8; ++i){
        random = random << 1;
        random = random + (0x00000001 & (*rnd_reg));
    }

    return random;
}


void
random_bytes(uint8_t * bytes, size_t size)
{
    for (size_t i=0; i<size; ++i) {
        bytes[i] = random_byte();
    }
}
