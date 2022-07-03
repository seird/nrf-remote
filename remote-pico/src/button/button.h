#ifndef __togglebutton_H__
#define __togglebutton_H__


#include <pico/stdlib.h>


typedef void * HTBUTTON;


HTBUTTON togglebutton_init(uint n, bool on);
void togglebutton_free(HTBUTTON htb);
bool togglebutton_on(HTBUTTON htb);
bool togglebutton_off(HTBUTTON htb);
bool togglebutton_pressed(HTBUTTON htb);

void button_init(uint n);
bool button_pressed(uint n);


#endif // __togglebutton_H__
