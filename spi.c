#include "globals.h"
#include "spi.h"
#include "flash.h"
#include "serial.h"

#include <avr/pgmspace.h>

#include <stdio.h>

uint8_t spi_send(uint8_t data) {
  uint8_t retval;

  if (flag_spi_verbose) {
    sprintf(serial_out, "SPI Sending: %d\r\n", data);
    usart_send();
    while(flag_serial_sending);
  }

  //PORTD ^= _BV(PORTD7);

  SPDR = data;
  while (!(SPSR & _BV(SPIF)));
  retval = SPDR;

  if (flag_spi_verbose) {
    sprintf(serial_out, "SPI Got: %d\r\n", retval);
    usart_send();
    while(flag_serial_sending);
  }

  return retval;
}

void spi_init(void) {
  DDRB |= _BV(PORTB2) | _BV(PORTB3) | _BV(PORTB5); // /SS, MOSI, and SCK are output
  //SPCR = _BV(SPE) | _BV(SPI2X) | _BV(MSTR); // Enable SPI, F_CPU / 2
  SPCR = _BV(SPE) | _BV(MSTR); // Enable SPI, F_CPU / 4
  //SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0); // Enable SPI, F_CPU / 16
  //SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1); // Enable SPI, F_CPU / 64
  flash_deselect();

  strcpy_P(serial_out, PSTR("\r\nSPI initialized\r\n"));
  usart_send();
  while (flag_serial_sending);
}
