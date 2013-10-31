#include "power.h"
#include <avr/sleep.h>

void power_sleep() {
  SMCR = _BV(SE); // Enable sleep mode + Idle
  sleep_cpu();
  SMCR = 0; // Disable sleep
}
