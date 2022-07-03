#include <pico/stdlib.h>

#include "button.h"
#include "../pin/pin.h"


void
button_init(uint n)
{
    pin_init(n, GPIO_IN);
    gpio_pull_up(n);
}


bool
button_pressed(uint n)
{
    return gpio_get(n) == false;
}
