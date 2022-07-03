#include <pico/stdlib.h>
#include <malloc.h>

#include "button.h"


struct ToggleButton {
    uint n;
    bool on;
    bool was_pressed;
};


HTBUTTON
togglebutton_init(uint n, bool on)
{
    gpio_init(n);
    gpio_set_dir(n, GPIO_IN);
    gpio_pull_up(n);

    struct ToggleButton * b = (struct ToggleButton *) malloc(sizeof(struct ToggleButton));
    if (b == NULL) {
        return NULL;
    }

    b->n = n;
    b->on = on;
    b->was_pressed = false;
    
    return (HTBUTTON) b;
}


void
togglebutton_free(HTBUTTON htb)
{
    free(htb);
}


bool
togglebutton_on(HTBUTTON htb)
{
    struct ToggleButton * b = (struct ToggleButton *) htb;

    if (!togglebutton_pressed(htb)) {
        // button is currently not pressed -> state remains unchanged
        b->was_pressed = false;
        return b->on;
    }

    if (b->was_pressed == false) {
        // state change
        b->on = !b->on;
        b->was_pressed = true;
    }

    return b->on;
}


bool
togglebutton_off(HTBUTTON htb)
{
    return !togglebutton_on(htb);
}


bool
togglebutton_pressed(HTBUTTON htb)
{
    struct ToggleButton * b = (struct ToggleButton *) htb;

    return gpio_get(b->n) == false;
}
