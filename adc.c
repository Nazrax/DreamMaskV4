#include "globals.h"

#include "flash.h"
#include "serial.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <util/delay.h>


#include <stdio.h>

volatile uint16_t flash_buf_ctr;

static inline void adc_start(void);

static volatile bool_t adc_ready;
static volatile bool_t adc_finished;

void adc_init(void) {
  ADMUX = _BV(REFS0) | _BV(MUX0);
  ADCSRA =  _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // Enable ADC and ADC interrupt; set prescaler to 128
}

// Let the LEDs and ADC mux settle out
static inline void wait_and_adc(void) {
  // Turn on timer for delay
  TCNT2 = 0;
  TCCR2A = _BV(WGM21); // CTC Mode
  TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20); // fCPU/1024 = 15625 ticks / second
  OCR2A = 10; // Just over 1500 interrupts / second
  TIMSK2 |= _BV(OCIE2A); // Enable CTC interrupt for OCR1A

  // Wait for the delay
  adc_ready = false;

  while(!adc_ready) {
    SMCR = _BV(SE); // Enable sleep mode + Idle
    sleep_cpu();
  }

  // Now we've waited about 500us
  // Do the ADC
  adc_finished = false;
  ADCSRA |= _BV(ADEN); // Enable ADC
  while (!adc_finished) {
    SMCR = _BV(SE) | _BV(SM0); // Enable sleep mode + ADC mode
    sleep_cpu();
  }
  ADCSRA &= ~(_BV(ADEN)); // Disable ADC

  SMCR = 0; // Disable sleep
  TCCR2B = 0; // Stop the timer
}

static inline void adc_start(void) {
  //PORTD |= _BV(PORTD6); // Turn on IR LEDs
  PORTD |= _BV(PORTD7); // Turn on IR LEDs

  //ADMUX = _BV(REFS0); // Select first endpoint
  ADMUX = _BV(REFS0) | _BV(MUX1) | _BV(MUX2); // Select first endpoint (ADC6)
  wait_and_adc();

  //ADMUX = _BV(REFS0) | _BV(MUX0); // Select next endpoint
  ADMUX = _BV(REFS0) | _BV(MUX0) | _BV(MUX1) | _BV(MUX2); // Select next endpoint (ADC7)
  wait_and_adc();

  //PORTD &= ~(_BV(PORTD6)); // Turn off IR LEDs
  PORTD &= ~(_BV(PORTD7)); // Turn off IR LEDs
}

void adc_dostuff(void) {
  if (flash_addr > 4090) {
    flag_want_adc = false;
  }

  if (flag_do_adc && !flag_xmodem_sending) {
    flag_do_adc = false;
    adc_start();
  }

  if (flash_buf_ctr > 255) {
    if (flag_adc_verbose) {
      strcpy_P(serial_out, PSTR("Dumping\r\n"));
      usart_send();
      while (flag_serial_sending);
      flag_want_header = true;
    }
    flash_write(flash_addr++);
    flash_buf_ctr = 0;
  }
}

ISR(ADC_vect) {
  uint16_t reading = 1024 - ADC;
  if (flag_adc_verbose) {
    if (ADMUX & _BV(MUX0)) { // Just did ADC1
      sprintf(serial_out, "%s %d\r\n", serial_out, reading);
      usart_send();
    } else { // Just did ADC0
      sprintf(serial_out, "%04ld %d", clock_ticks, reading);
    }
  }

  /*
  if (flag_adc_verbose) {
    uint8_t len;
    itoa(clock.ticks, serial_out, 10);
    len = strlen(serial_out);
    serial_out[len] = ' ';
    itoa(reading, serial_out + len + 1, 10);
    len = strlen(serial_out);
    serial_out[len] = '\r';
    serial_out[len+1] = '\n';
    serial_out[len+2] = '\0';
  }
  */

  //  if (flag_want_header && flash_buf_ctr == 0) {
  if (flash_buf_ctr == 0) {
    flash_buf[0] = 0xFF;
    flash_buf[1] = 0xEE;
    flash_buf[2] = 0;
    flash_buf[3] = buttonPresses;
    flash_buf[4] = clock.hours;
    flash_buf[5] = clock.minutes;
    flash_buf[6] = clock.seconds;
    flash_buf[7] = clock.subseconds;
    flash_buf_ctr = 8;
    flag_want_header = false;
  }

  //flash_buf[flash_buf_ctr++] = clock_ticks >> 8;
  //flash_buf[flash_buf_ctr++] = clock_ticks & 0xFF;
  flash_buf[flash_buf_ctr++] = reading >> 8;
  flash_buf[flash_buf_ctr++] = reading & 0xFF;

  adc_finished = true;
}

ISR(TIMER2_COMPA_vect) {
  adc_ready = true;
}
