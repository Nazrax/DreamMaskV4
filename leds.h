#ifndef _LEDS_H_
#define _LEDS_H_

extern uint8_t led_power;

void led_init(void);
void led_reset(void);
void led_block(void);
void led_unblock(void);
void led_want(uint8_t led);
void led_handle(void);
//void led_on(uint8_t led);
//void led_off(uint8_t led);

#endif
