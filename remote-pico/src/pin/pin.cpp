#include <pico/stdlib.h>

#include "pin.h"


void
pin_init(uint n, bool direction)
{
    gpio_init(n);
    gpio_set_dir(n, direction);
}


void
pin_on(uint n)
{
    gpio_put(n, true);
}


void
pin_off(uint n)
{
    gpio_put(n, false);
}


void
pin_blink(uint n, int times, uint32_t interval_ms)
{
    for (int i=0; i<times; ++i) {
        pin_on(n);
        sleep_ms(interval_ms);
        pin_off(n);
        sleep_ms(interval_ms);
    }
}


void
pin_blink_inverted(uint n, int times, uint32_t interval_ms)
{
    for (int i=0; i<times; ++i) {
        pin_off(n);
        sleep_ms(interval_ms);
        pin_on(n);
        sleep_ms(interval_ms);
    }
}
