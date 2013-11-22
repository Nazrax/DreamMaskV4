#include "globals.h"
#include "clock.h"

#include <avr/interrupt.h>

volatile uint32_t clock_ticks;
clock_t clock;

// The clock uses timer 2 in sync mode
void clock_init() {
  TCCR1B = _BV(WGM12); // CTC Mode
  TCCR1B |= _BV(CS11) | _BV(CS10); // fCPU / 64
  OCR1A = 7200; // 8 ticks / second
  TIMSK1 |= _BV(OCIE1A); // Enable CTC interrupt for OCR1A

  /*
  TCCR2A |= _BV(WGM21); // CTC mode
  TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20); // Clock / 1024
  OCR2A = 125; // Fire every 125 clock ticks (125 times / second)
  TIMSK2 |= _BV(OCIE2A); // Enable CTC interrupt for OCR2A
  */
}

void clock_update() {
  while (clock.subseconds > TICKS_PER_SECOND) {    
    clock.subseconds -= TICKS_PER_SECOND;
    clock.seconds++;
    
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
ISR(TIMER1_COMPA_vect) {
  clock_ticks++;

  //  if (clock_ticks % 4 == 0) {
    PORTB ^= _BV(PORTB2);
    //}

  clock.subseconds++;
  if (flag_want_adc && !flag_xmodem_sending) { // Read every clock tick (8 times / second)
    flag_do_adc = true;
  }
}
