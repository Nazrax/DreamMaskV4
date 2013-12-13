#include "globals.h"
#include "leds.h"

#ifdef DEBUG
uint8_t led_power = 254;
#else
//uint8_t led_power = 7;
//uint8_t led_power = 1;
uint8_t led_power = 254;
#endif

void led_init(void) {
  TCCR0A = _BV(WGM01) | _BV(WGM00); // fast pwm mode
  TCCR0B = _BV(CS00); // clock, no scaling
}

void led_on(uint8_t led) {
  if (led == 0) {
    //PORTB |= _BV(PORTB1);
    OCR0A = led_power;
    TCCR0A |= _BV(COM0A1);
  } else if (led == 1) {
    //PORTB |= _BV(PORTB2);
    OCR0B = led_power;
    TCCR0A |= _BV(COM0B1);
  } else if (led == 2) {
    PORTD |= _BV(PORTD3);
  }
}

void led_off(uint8_t led) {
  if (led == 0) {
    //PORTB &= ~(_BV(PORTB1));
    TCCR0A &= ~(_BV(COM0A1));
  } else if (led == 1) {
    //PORTB &= ~(_BV(PORTB2));
    TCCR0A &= ~(_BV(COM0B1));
  } else if (led == 2) {
    PORTD &= ~(_BV(PORTD3));
  }
}

/*
void led_on(uint8_t led) {
  if (led == 0) {
    PORTB |= _BV(PORTB0);
    OCR0A = led_power;
    TCCR0A |= _BV(COM0A1);
  } else if (led == 1) {
    PORTB |= _BV(PORTB1);
    OCR0B = led_power;
    TCCR0A |= _BV(COM0B1);
  } else if (led == 2) {
  }
}

void led_off(uint8_t led) {
  if (led == 0) {
    PORTB &= ~(_BV(PORTB0));
    TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0));
  } else {
    PORTB &= ~(_BV(PORTB1));
    TCCR0A &= ~(_BV(COM0B1) | _BV(COM0B0));
  }
}
*/
