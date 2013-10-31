#include "globals.h"

#include "adc.h"
#include "buttons.h"
#include "power.h"
#include "serial.h"
#include "clock.h"
#include "spi.h"
#include "flash.h"

#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

// Function prototypes
static void init_io(void);
static void buttons_handle(void);

static void init_io() {
  DDRB |= _BV(DDB2); // Set SS as output

  DDRB |= _BV(DDB0); // LED
  DDRD |= _BV(DDD5) | _BV(DDD6) | _BV(DDD7); // IR LEDs
  
  PORTD |= _BV(PORTD2) | _BV(PORTD3); // Enable internal pullups on buttons

  // Configure buttons
  buttons[0].pin = PIND3;
  buttons[1].pin = PIND2;
  buttonPresses = 0;

  PCMSK2 = _BV(PCINT18) | _BV(PCINT19);
  PCICR = _BV(PCIE2);
}


int main(int argc, char** argv) {
  MCUSR = 0;
  //wdt_disable();
  wdt_enable(WDTO_4S);

  CLKPR = (1<<CLKPCE); // set Clock Prescaler Change Enable
  CLKPR = 0;           // No prescaling

  //PORTB |= _BV(PORTB0); // Turn on LEDs

  init_io();


  usart_init();
  sei();

  adc_init();
  clock_init();
  spi_init();
  flash_init();

  strcpy_P(serial_out, PSTR("Scanning flash!\r\n"));
  usart_send();
  while (flag_serial_sending);

  flash_scan();
  strcpy_P(serial_out, PSTR("Flash address is "));
  sprintf(serial_out+17, "%d\r\n", flash_addr);
  usart_send();
  while (flag_serial_sending);

  strcpy_P(serial_out, PSTR("Ready!\r\n> "));
  usart_send();

  flag_want_adc = true;

  while(true) {
    clock_update();

    buttons_update();
    buttons_handle();

    adc_dostuff();
    usart_dostuff();

    buttons_age();

    wdt_reset();
    power_sleep();
  }

  return 0;
}

static void buttons_handle() {
  if (pressed(&buttons[0]) || pressed(&buttons[1])) {
    buttonPresses++;
    strcpy_P(serial_out, PSTR("Button pressed\r\n"));
    usart_send();
    while (flag_serial_sending);
  }
}

ISR(PCINT2_vect) {
  buttons_update();
}
