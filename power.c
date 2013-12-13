#include "power.h"
#include <avr/sleep.h>

void power_idle() {
  /*
  SMCR = _BV(SE); // Enable sleep mode + Idle
  sleep_cpu();
  SMCR = 0; // Disable sleep
  */

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  sleep_cpu();
  sleep_disable();
}

void power_save() {
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
  sleep_enable();
  sleep_cpu();
  sleep_disable();
}
