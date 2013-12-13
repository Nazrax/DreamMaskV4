#ifndef _LEDS_H_
#define _LEDS_H_

extern uint8_t led_power;

void led_init(void);
void led_on(uint8_t led);
void led_off(uint8_t led);

#endif
