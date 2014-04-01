#include "globals.h"

#include "flash.h"
#include "serial.h"
#include "leds.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <util/delay.h>


#include <stdio.h>

volatile uint16_t flash_buf_ctr;

static inline void adc_start(void);
static void saveToBuffer(uint16_t);

static volatile bool_t adc_ready;
static volatile bool_t adc_finished;

static bool_t adc_blocked = false;

uint16_t adc_min, adc_max;

void adc_init(void) {
  ADMUX = _BV(REFS0) | _BV(MUX0);
  ADCSRA =  _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // Enable ADC and ADC interrupt; set prescaler to 128
}

void adc_block(void) {
  adc_blocked = true;
}

void adc_unblock(void) {
  adc_blocked = false;
}

// Let the LEDs and ADC mux settle out
static inline void wait_and_adc(void) {
  // Turn on timer for delay
  TCNT0 = 0;
  TCCR0A = _BV(WGM01); // CTC Mode
  TCCR0B = _BV(CS00) | _BV(CS01); // clock / 64
  OCR0A = 39; // Around 1500 interrupts / second
  TIMSK0 |= _BV(OCIE0A); // Enable CTC interrupt for OCR1A

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
  TCCR0A = 0; // Stop the timer
  TCCR0B = 0; // Stop the timer
}

static inline void adc_start(void) {
  PORTC |= _BV(PORTC2) | _BV(PORTC3); // Turn on IR LEDs
  led_block(); // Turn off visible LEDs
  led_handle();

  ADMUX = _BV(REFS0); // Select first endpoint (ADC0)
  wait_and_adc();

  ADMUX = _BV(REFS0) | _BV(MUX0); // Select next endpoint (ADC1)
  wait_and_adc();

  PORTC &= ~(_BV(PORTC2) | _BV(PORTC3)); // Turn off IR LEDs
  led_unblock(); // Turn on visible LEDs
  led_handle();
}

void adc_dostuff(void) {
  if (flash_addr > 4090) {
    flag_want_adc = false;
  }

  if (adc_blocked) {
    saveToBuffer(65535);
  } else {
    if (flag_do_adc && !flag_xmodem_sending) {
      flag_do_adc = false;
      adc_start();
    }
  }

  if (flash_buf_ctr > 255) {
    if (flag_adc_verbose || flag_flash_verbose) {
      strcpy_P(serial_out, PSTR("Dumping\r\n"));
      usart_send();
      while (flag_serial_sending);
    }
    flag_want_header = true;
    flash_write(flash_addr++);
    flash_buf_ctr = 0;
  }
}

ISR(ADC_vect) {
  uint16_t reading = 1024 - ADC;
  if (flag_adc_verbose) {
    if (ADMUX & _BV(MUX0)) { // Just did ADC1
      sprintf(serial_out, "%s %d %d %d %d %d\r\n", serial_out, reading, buttonPresses, adc_min, adc_max, led_power);
      usart_send();
    } else { // Just did ADC0
      sprintf(serial_out, "%04ld %d", clock_ticks, reading);
      if (adc_min > reading) {
	adc_min = reading;
      }
      if (adc_max < reading) {
	adc_max = reading;
      }
    }
  }

  /*
  if (!(ADMUX & _BV(MUX0))) { // Just did ADC1
    led_power = 1.0 * (reading - adc_min) / (adc_max - adc_min) * 180 + 64;
  }
  */

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

  saveToBuffer(reading);

  //  if (flag_want_header && flash_buf_ctr == 0) {

  adc_finished = true;
}

void saveToBuffer(uint16_t reading) {
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
}

ISR(TIMER0_COMPA_vect) {
  adc_ready = true;
}
