#include "globals.h"
#include "leds.h"

#ifdef DEBUG
uint8_t led_power = 254;
#else
//uint8_t led_power = 7;
//uint8_t led_power = 1;
//uint8_t led_power = 24;
//uint8_t led_power = 36;

// uint8_t led_power = 80; // Enough to wake me
uint8_t led_power = 30;
#endif

bool_t want_led[3];
bool_t leds_blocked;

void led_on(uint8_t led);
void led_off(uint8_t led);

void led_block() {
  leds_blocked = true;
}

void led_unblock() {
  leds_blocked = false;
}

void led_reset(void) {
  int i;
  for(i=0; i<3; i++) {
    want_led[i] = false;
  }
  leds_blocked = false;
}

void led_want(uint8_t i) {
  want_led[i] = true;
}

void led_handle(void) {
  int i;
  for(i=0; i<3; i++) {
    if (i == 2 || !leds_blocked) {
      if (want_led[i]) {
	led_on(i);
      } else {
	led_off(i);
      }
    }
  }
}

void led_init(void) {
  // fast pwm mode (8 bit)
  // clock, no scaling
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(WGM12) | _BV(CS10);
}

void led_on(uint8_t led) {
  if (led == 0) {
    //PORTB |= _BV(PORTB1);
    OCR1A = led_power;
    TCCR1A |= _BV(COM1A1);
  } else if (led == 1) {
    //PORTB |= _BV(PORTB2);
    OCR1B = led_power;
    TCCR1A |= _BV(COM1B1);
  } else if (led == 2) {
    PORTD |= _BV(PORTD3);
  }
}

void led_off(uint8_t led) {
  if (led == 0) {
    //PORTB &= ~(_BV(PORTB1));
    TCCR1A &= ~(_BV(COM1A1));
  } else if (led == 1) {
    //PORTB &= ~(_BV(PORTB2));
    TCCR1A &= ~(_BV(COM1B1));
  } else if (led == 2) {
    PORTD &= ~(_BV(PORTD3));
  }
}
