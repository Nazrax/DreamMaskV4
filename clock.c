#include "globals.h"
#include "clock.h"

#include <avr/interrupt.h>

volatile uint32_t clock_ticks;
myclock_t clock;

// The clock uses timer 2 in async mode
void clock_init() {
  ASSR = _BV(AS2); // Put timer 2 in async mode
  TCCR2A = _BV(WGM21); // CTC Mode
  TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20); // clock / 1024
  OCR2A = 3; // 8 ticks / second

  // Ensure we don't get a spurious interrupt after changing the clock source
  while (ASSR & _BV(TCN2UB) || ASSR & _BV(OCR2AUB) || ASSR & _BV(OCR2BUB) || ASSR & _BV(TCR2AUB) || ASSR & _BV(TCR2BUB));
  TIFR2 = 0;

  TIMSK2 |= _BV(OCIE2A); // Enable CTC interrupt for OCR1A
}

void clock_update() {
  while (clock.subseconds > TICKS_PER_SECOND) {    
    clock.subseconds -= TICKS_PER_SECOND;
    clock.seconds++;
    clock.tseconds++;
    
    if (clock.seconds > 59) {
      clock.seconds = 0;
      clock.minutes++;
      
      if (clock.minutes > 59) {
        clock.minutes = 0;
        clock.hours++;

        if (clock.hours > 23)
          clock.hours = 0;
      }
    }
  }
}

// The clock
ISR(TIMER2_COMPA_vect) {
  clock_ticks++;

  //  if (clock_ticks % 4 == 0) {
  //PORTB ^= _BV(PORTB2);
    //}

  clock.subseconds++;
  if (flag_want_adc && !flag_xmodem_sending) { // Read every clock tick (8 times / second)
    flag_do_adc = true;
  }
}
