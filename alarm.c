#include "globals.h"
#include "alarm.h"
#include "leds.h"

uint16_t alarm_time = 0;

void alarm_init() {
  alarm_time = INITIAL_ALARM_TIME;
}

void alarm_snooze() {
  uint16_t new_time = clock.tseconds + SNOOZE_TIME;
  if (new_time > alarm_time)
    alarm_time = new_time;
  led_power = 1;
}

void alarm_set_relative(uint8_t hours, uint8_t minutes) {
  alarm_time = clock.tseconds + hours * 3600 + minutes * 60;
}

void alarm_handle(void) {
  if (clock.tseconds > alarm_time) {
    if (clock.tseconds - alarm_time < ALARM_ACTIVE_TIME) {
      alarm_active = true;
      if (clock.subseconds > 3) {
        led_off(0);
        led_off(1);
      } else {
        if (clock.seconds % 2 == 0) {
          led_on(0);
          led_off(1);
        } else {
          led_on(1);
          led_off(0);
        }
      }
    } else {
      alarm_active = false;
      alarm_time = clock.tseconds + ALARM_WAIT_TIME;
      led_power++;
    }
  } else {
    alarm_active = false;
  }
}
