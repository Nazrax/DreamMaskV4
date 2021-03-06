#include "globals.h"

#include "adc.h"
#include "alarm.h"
#include "buttons.h"
#include "calib_32kHz.h"
#include "clock.h"
#include "detector.h"
#include "flash.h"
#include "leds.h"
#include "morse.h"
#include "power.h"
#include "serial.h"
#include "spi.h"

#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <string.h>
#include <stdio.h>

extern uint16_t adc_min, adc_max;
extern reading_data_t *leftData, *rightData;
extern uint16_t left_reading, right_reading;


// Function prototypes
static void init_io(void);
static void buttons_handle(void);

static void init_io() {
  DDRB |= _BV(DDB1) | _BV(DDB2); // LEDs 1 & 2
  DDRD |= _BV(DDD3); // LED 3

  DDRC |= _BV(DDC2) | _BV(DDC3); // IR LEDs

  PORTB |= _BV(PORTB0); // Button 1 pullup
  PORTC |= _BV(PORTC5); // Button 2 pullup


  // Configure buttons
  buttons[0].port = PB;
  buttons[1].port = PC;
  buttons[0].pin = PINB0;
  buttons[1].pin = PINC5;
  buttonPresses = 0;

  PCMSK0 = _BV(PCINT0); // Button 1 on pin 14/PB0 / PCINT0
  PCMSK1 = _BV(PCINT13); // Button 2 on pin 28 / PC5 / PCINT13
  
  PCICR = _BV(PCIE0) | _BV(PCIE1);
}


int main(void) {
  int old_mcusr = MCUSR;
  MCUSR = 0;
  wdt_disable();

  CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
  CLKPR = _BV(CLKPS0); // Divide by 2, 4 MHz
  DoCalibrate();

  wdt_enable(WDTO_8S);

  //CLKPR = (1<<CLKPCE); // set Clock Prescaler Change Enable
  //CLKPR = 0;           // No prescaling

  init_io();

  led_init();
  detector_init();

  /*
  while(1) { 
    PORTB |= _BV(PORTB1) | _BV(PORTB2);
    wdt_reset(); 
    _delay_ms(2000);
    PORTB &= ~(_BV(PORTB1) | _BV(PORTB2));
    wdt_reset(); 
    _delay_ms(2000);
  }
  */

  // Turn on LEDs
  led_reset();
  led_want(0);
  led_want(1);
  led_want(2);
  led_handle();


  _delay_ms(1000);
  led_reset();
  led_handle();
  _delay_ms(500);
  led_want(0);
  led_want(1);
  led_want(2);
  led_handle();
  
  //PORTB |= _BV(PORTB1) | _BV(PORTB2);
  //PORTD |= _BV(PORTD3);

  usart_init();
  sei();

  adc_init();
  clock_init();
  spi_init();
  flash_init();
  alarm_init();

  sprintf(serial_out, "Voltage is %d\r\n", adc_voltage());
  usart_send();
  while (flag_serial_sending);

  strcpy_P(serial_out, PSTR("Scanning flash!\r\n"));
  usart_send();
  while (flag_serial_sending);

  flash_scan();
  strcpy_P(serial_out, PSTR("Flash address is "));
  sprintf(serial_out+17, "%d\r\n", flash_addr);
  usart_send();
  while (flag_serial_sending);

  flag_want_adc = false;

  if ((old_mcusr & _BV(WDRF)) || (old_mcusr & _BV(BORF))) {
    strcpy_P(serial_out, PSTR("Enabling ADC: chip was reset\r\n"));
    usart_send();
    flag_want_adc = true;
  } else if (flash_addr > 0) {
    strcpy_P(serial_out, PSTR("Enabling ADC: flash not empty\r\n"));
    usart_send();
    flag_want_adc = true;
  } else {
    strcpy_P(serial_out, PSTR("Disabling ADC\r\n"));
    usart_send();
    flag_want_adc = false;
  }
  while(flag_serial_sending);

  strcpy_P(serial_out, PSTR("Ready!\r\n> "));
  usart_send();
  while(flag_serial_sending);

  led_reset();
  led_handle();


  bool_t dreaming = false;
  char changeBuf[8];
  changeBuf[0] = '\0';

  while(true) {
    //sprintf(serial_out, "Main loop: %ld\r\n", clock_ticks);
    //usart_send();
    if (!dreaming && (leftData->dreaming || rightData->dreaming)) {
      strcpy_P(changeBuf, PSTR("started\0"));
    } else if (dreaming && !leftData->dreaming && !rightData->dreaming) {
      strcpy_P(changeBuf, PSTR("stopped\0"));
    } else {
      changeBuf[0] = '\0';
    }
    dreaming = leftData->dreaming || rightData->dreaming;

    if (flag_adc_verbose && flag_did_adc && flag_clock_ticked) {
      flag_did_adc = false;
      flag_clock_ticked = false;

      //uint16_t left = (flash_buf[flash_buf_ctr - 4] << 8) + flash_buf[flash_buf_ctr - 3];
      //uint16_t right = (flash_buf[flash_buf_ctr - 2] << 8) + flash_buf[flash_buf_ctr - 1];
      
      sprintf(serial_out, "%04ld LR:%04d RR:%04d LC:%04ld RC:%04ld LD:%d RD:%d %s\n\r",
              clock_ticks,
              left_reading,
              right_reading,
              leftData->movementCount,
              rightData->movementCount,
              leftData->dreaming,
              rightData->dreaming,
              changeBuf
              );
      usart_send();
      while(flag_serial_sending);
    }

    //PORTD ^= _BV(PORTD3);
    clock_update();

    led_reset();

    buttons_update();
    buttons_handle();

    alarm_handle();

    morse_handle();


    //if (!alarm_active) {
    //  led_off(0);
    //  led_off(1);
    //}

    adc_dostuff();

    usart_dostuff();

    led_handle();

    buttons_age();


    wdt_reset();
    power_idle();
  }

  return 0;
}

/*
static void morse_alarm_handle() {
  if (clock.subseconds == 1) {
    if (clock.seconds == 0) {
      if (clock.hours >= 5 && clock.minutes % 15 == 0) {
	strcpy_P(morse_out, PSTR("DREAM DREAM"));
	//morse_send();
      }
    }
  }
}
*/

static void buttons_handle() {
  if (!flag_want_adc && clock.tseconds > 1 && (pressed(&buttons[0]) || pressed(&buttons[1]))) {
    flag_want_adc = true;
    cli();
    clock.tseconds = 0;
    clock.subseconds = 0;
    clock.seconds = 0;
    clock.minutes = 0;
    clock.hours = 0;
    sei();
  }

  if (buttons[1].current || buttons[1].current) {
    led_want(0);
    led_want(1);
    led_want(2);
  }

  if (pressed(&buttons[0]) || pressed(&buttons[1])) {
    adc_min = 1023;
    adc_max = 0;

    alarm_snooze(0);
    //led_power = 1;
    /*
    if (alarm_active == true) { // || (alarm_time - clock.tseconds) < 300) {
      alarm_time = clock.tseconds + 60*60*10;
    } else {
      alarm_snooze();
    }
    */
  } 

  if (pressed(&buttons[0])) {
    buttonPresses++;
    //strcpy_P(serial_out, PSTR("Button 0 pressed\r\n"));
    //usart_send();
    while (flag_serial_sending);
  }

  if (pressed(&buttons[1])) {
    buttonPresses++;
    //strcpy_P(serial_out, PSTR("Button 1 pressed\r\n"));
    //usart_send();
    while (flag_serial_sending);
  }
}

ISR(PCINT0_vect) {
  buttons_update();
}

ISR(PCINT1_vect) {
  buttons_update();
}

ISR(PCINT2_vect) {
  buttons_update();
}
