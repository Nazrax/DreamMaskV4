#include "globals.h"

#include "buttons.h"
#include "flash.h"
#include "serial.h"
#include "leds.h"
#include "detector.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern reading_data_t *leftData, *rightData;
extern bool_t morse_sending;
extern bool_t want_led;

volatile uint16_t flash_buf_ctr;

static inline void adc_start(void);
static void saveToBuffer(uint16_t);

static volatile bool_t adc_ready;
static volatile bool_t adc_finished;

static bool_t adc_blocked = false;

uint16_t adc_min, adc_max, left_reading, right_reading;

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
  /*
  // Turn on timer for delay
  TCNT0 = 0;
  TCCR0A = _BV(WGM01); // CTC Mode
  TCCR0B = _BV(CS00) | _BV(CS01); // clock / 64
  OCR0A = 1; // Around 1500 interrupts / second
  TIMSK0 |= _BV(OCIE0A); // Enable CTC interrupt for OCR1A

  // Wait for the delay
  adc_ready = false;
  
  while(!adc_ready) {
    SMCR = _BV(SE); // Enable sleep mode + Idle
    sleep_cpu();
  } 
  // Now we've waited about 500us
  // Do the ADC
  */

  adc_finished = false;
  ADCSRA |= _BV(ADEN); // Enable ADC
  while (!adc_finished) {
    SMCR = _BV(SE) | _BV(SM0); // Enable sleep mode + ADC mode
    sleep_cpu();
  }
  ADCSRA &= ~(_BV(ADEN)); // Disable ADC

  SMCR = 0; // Disable sleep
  //TCCR0A = 0; // Stop the timer
  //TCCR0B = 0; // Stop the timer
}

static inline void adc_start(void) {
  PORTC |= _BV(PORTC2); // Turn on IR LED 
  led_block(); // Turn off visible LEDs
  led_handle();

  ADMUX = _BV(REFS0); // Select first endpoint (ADC0)
  wait_and_adc();

  PORTC &= ~(_BV(PORTC2)); // Turn off IR LED
  PORTC |= _BV(PORTC3); // Turn on IR LED

  ADMUX = _BV(REFS0) | _BV(MUX0); // Select next endpoint (ADC1)
  wait_and_adc();

  PORTC &= ~(_BV(PORTC3)); // Turn off IR LED
  led_unblock(); // Turn on visible LEDs
  led_handle();

  detector_update(left_reading, right_reading);
  saveToBuffer(left_reading);
  saveToBuffer(right_reading);
}

uint16_t adc_voltage(void) {
  int i;
  cli();
  uint8_t old_admux = ADMUX;
  uint8_t old_adcsra = ADCSRA;
  uint16_t last_reading = 2048;

  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); // Internal 1.1v bandgap reference
  for(i=0; i<100; i++) {
    ADCSRA = _BV(ADEN) | _BV(ADSC);
    while (!(ADCSRA & _BV(ADIF)));
    wdt_reset();
    _delay_ms(1);
    if (abs(ADC - last_reading) < 3) {
      break;
    }
    last_reading = ADC;
  }
  ADMUX = old_admux;
  ADCSRA = old_adcsra;
  uint16_t reading = ADC;
  sei();

  return reading;
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
    bool_t verified = false;
    int loopCount = 0;
    while (!verified) {
      flash_write(flash_addr);
      _delay_ms(1);
      verified = flash_verify(flash_addr);
      if (!verified) {
	sprintf(serial_out, "Verification %d failed\r\n", loopCount);
	usart_send();
	_delay_ms(1);
	while (flag_serial_sending);
	loopCount++;
      }
    }
    flash_addr++;
    flash_buf_ctr = 0;
  }
}

ISR(ADC_vect) {
  uint16_t reading = 1024 - ADC;
  //static uint16_t lastReading;

  if (ADMUX & _BV(MUX0)) { // Just did ADC1
    //sprintf(serial_out, "%s %d %d %d %d %d\r\n", serial_out, reading, buttonPresses, adc_min, adc_max, led_power);
    //sprintf(serial_out, "%04ld %d %d %d %d %d %d\r\n", clock_ticks, lastReading, reading, buttonPresses, adc_min, adc_max, led_power);
    //usart_send();
    right_reading = reading;
    flag_did_adc = true;
  } else { // Just did ADC0
    //sprintf(serial_out, "foo %04ld %d", clock_ticks, reading);
    //sprintf(serial_out, "Hello world\r\n");
    if (adc_min > reading) {
      adc_min = reading;
    }
    if (adc_max < reading) {
      adc_max = reading;
    }
    left_reading = reading;
  }
  //lastReading = reading;
  
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

  /* uint16_t threshold; */
  /* uint32_t movementSum; */
  /* uint32_t movementCount; */
  /* uint16_t lastMovement; */
  /* uint16_t ticksSinceMovement; */
  /* uint8_t cooldown; */


  //flash_buf[flash_buf_ctr++] = clock_ticks >> 8;
  //flash_buf[flash_buf_ctr++] = clock_ticks & 0xFF;
  flash_buf[flash_buf_ctr++] = (reading >> 8) | (want_led << 2) | (leftData->dreaming << 3) | (rightData->dreaming << 4) | (buttons[0].current << 5) | (buttons[1].current << 6) | (morse_sending << 7);
  flash_buf[flash_buf_ctr++] = reading & 0xFF;

  /*
  flash_buf[flash_buf_ctr++] = data->threshold >> 8;
  flash_buf[flash_buf_ctr++] = data->threshold & 0xFF;

  flash_buf[flash_buf_ctr++] = (data->movementSum >> 8) & 0xFF;
  flash_buf[flash_buf_ctr++] = data->movementSum & 0xFF;

  flash_buf[flash_buf_ctr++] = ((data->movementCount >> 8) & 0x0F) | (data->cooldown << 4);
  flash_buf[flash_buf_ctr++] = data->movementCount & 0xFF;

  flash_buf[flash_buf_ctr++] = data->lastMovement >> 8;
  flash_buf[flash_buf_ctr++] = data->lastMovement & 0xFF;

  flash_buf[flash_buf_ctr++] = data->ticksSinceMovement >> 8;
  flash_buf[flash_buf_ctr++] = data->ticksSinceMovement & 0xFF;
  */
}

ISR(TIMER0_COMPA_vect) {
  adc_ready = true;
}
