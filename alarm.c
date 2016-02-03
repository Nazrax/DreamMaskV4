#include "globals.h"
#include "alarm.h"
#include "leds.h"
#include "detector.h"
#include "morse.h"
#include "serial.h"

#include <avr/pgmspace.h>

extern reading_data_t *leftData, *rightData;

uint16_t alarm_time = 0;

void alarm_init() {
  alarm_time = ALARM_INITIAL_TIME;
}

void alarm_snooze(uint16_t snoozeTime) {
  if (!snoozeTime) {
    snoozeTime = ALARM_SNOOZE_TIME;
  }
  uint16_t newTime = clock.tseconds + snoozeTime;
  if (newTime > alarm_time) {
    alarm_time = newTime;
  }
  //led_power = 1;
}

void alarm_set_relative(uint8_t hours, uint8_t minutes) {
  alarm_time = clock.tseconds + hours * 3600 + minutes * 60;
}

void alarm_handle(void) {
  if ((leftData->dreaming && !leftData->ticksSinceMovement) || (rightData->dreaming && !rightData->ticksSinceMovement)) {
    if (clock.tseconds > alarm_time) {
      strcpy_P(morse_out, PSTR(ALARM_STRING));
      morse_send();
      alarm_snooze(ALARM_WAIT_TIME);

      strcpy_P(serial_out, PSTR("Morsing alarm\r\n"));
      usart_send();
      while (flag_serial_sending);
    }
  }
  /*
  if (clock.tseconds > alarm_time) {
    if (clock.tseconds - alarm_time < ALARM_ACTIVE_TIME) {
      alarm_active = true;
      if (clock.subseconds > 3) {
        //led_off(0);
        //led_off(1);
      } else {
        if (clock.seconds % 2 == 0) {
          led_want(0);
          //led_off(1);
        } else {
          led_want(1);
          //led_off(0);
        }
      }
    } else {
      alarm_active = false;
      alarm_time = clock.tseconds + ALARM_WAIT_TIME;
      if (led_power < 10) {
	led_power++;
      } else if (led_power < 127) {
	led_power *= 2;
      } else {
	led_power = 254;
      }
    }
  } else {
    alarm_active = false;
  }
  */
}
