#ifndef __PIN_H__
#define __PIN_H__


#include <pico/stdlib.h>


void pin_init(uint n, bool direction);
void pin_on(uint n);
void pin_off(uint n);
void pin_blink(uint n, int times, uint32_t interval_ms);
void pin_blink_inverted(uint n, int times, uint32_t interval_ms);


#endif // __PIN_H__
