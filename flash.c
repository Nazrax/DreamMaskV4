#include "globals.h"
#include "spi.h"
#include "flash.h"
#include "serial.h"

#include <stdio.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

uint8_t flash_buf[256];
uint16_t flash_addr;
volatile uint16_t flash_buf_ctr;

void flash_enable_write(void);
void flash_wait_for_wel(void);
uint8_t flash_status(void);
void flash_wait_for_idle(void);
bool_t flash_busy(void);

void flash_init(void) {
  DDRB |= _BV(DDB1); // /CS

  strcpy_P(serial_out, PSTR("\r\Flash initialized\r\n"));
  usart_send();
  while (flag_serial_sending);

  //flash_powerdown();
}

void flash_doheader() {
  if (flag_want_header) {
    flash_buf[0] = 0xFF;
    flash_buf[1] = 0xEE;
    flash_buf[2] = 0;
    flash_buf[3] = clock.hours;
    flash_buf[4] = clock.minutes;
    flash_buf[5] = clock.seconds;
    flash_buf[6] = buttonPresses;
    flash_buf_ctr = 7;
    flag_want_header = false;
  }
}

void flash_condwrite() {
  if (flash_buf_ctr > 253) { // Leave room for 3 byte writes
    //uint8_t i;
    //bool_t nonzero = false;

    if (flag_flash_verbose) {
      sprintf(serial_out, "\r\nSaving to flash at %d %ld\r\n", flash_addr, clock_ticks);
      usart_send();
    }
    flash_write(flash_addr++);
    flash_buf_ctr = 0;
    flag_want_header = true;
  }
}

uint8_t flash_status(void) {
  uint8_t status;
  flash_select();
  spi_send(INSTR_READ_STATUS1);
  status = spi_send(DONTCARE);
  flash_deselect();
  wdt_reset();
  return status;
}

void flash_scan() {
  uint8_t data;

  if (flag_flash_verbose) {
    strcpy_P(serial_out, PSTR("\r\nPowering up flash\r\n"));
    usart_send();
  }

  flash_powerup();

  if (flag_flash_verbose) {
    strcpy_P(serial_out, PSTR("\r\nWaiting for idle\r\n"));
    usart_send();
  }

  flash_wait_for_idle();
  for(flash_addr=0; flash_addr < 4096; flash_addr++) {
    flash_select();
    spi_send(INSTR_READ_DATA);
    spi_send(flash_addr >> 8);
    spi_send(flash_addr & 0xFF);
    spi_send(1); // Check the second byte of each page
    data = spi_send(DONTCARE);
    flash_deselect();
    wdt_reset();
    if (data == 0xFF)
      break;
  }

  flash_powerdown();
}

void flash_enable_write(void) {
  flash_wait_for_idle();

  do {
    flash_select();
    spi_send(INSTR_WRITE_ENABLE);
    flash_deselect();
  } while (!(flash_status() & STATUS_WEL));
}

void flash_write(uint16_t addr) {
  int i;
  uint8_t retval;

  do {
    flash_powerup();

    flash_enable_write();
    flash_wait_for_idle();
    //_delay_ms(50);

    flash_select();
    spi_send(INSTR_PAGE_PROGRAM);
    spi_send(addr >> 8);
    spi_send(addr & 0xFF);
    spi_send(0);

    for(i=0; i<256; i++) {
      spi_send(flash_buf[i]);
    }
    flash_deselect();

    // Now verify that the write actually "took"
    flash_wait_for_idle();
    flash_select();
    spi_send(INSTR_READ_DATA);
    spi_send(addr >> 8);
    spi_send(addr & 0xFF);
    spi_send(1);
    retval = spi_send(DONTCARE);
    flash_deselect();
  } while (flash_buf[1] != 0xff && retval == 0xff);

  flash_powerdown();
}

void flash_read(uint16_t addr) {
  int i;
  flash_powerup();

  flash_select();
  spi_send(INSTR_READ_DATA);
  spi_send(addr >> 8);
  spi_send(addr & 0xFF);
  spi_send(0);

  for(i=0; i<256; i++) {
    flash_buf[i] = spi_send(DONTCARE);
    PORTD ^= _BV(PORTD6);
  }
  flash_deselect();

  flash_powerdown();
}

void flash_erase(void) {
  flash_powerup();
  _delay_ms(50);

  flash_enable_write();
  flash_wait_for_idle();

  flash_select();
  spi_send(INSTR_CHIP_ERASE);
  flash_deselect();

  flash_powerdown();

  flag_want_header = true;
  flash_buf_ctr = 0;
}

void flash_wait_for_idle(void) {
  flash_select();
  spi_send(INSTR_READ_STATUS1);
  while (spi_send(DONTCARE) & STATUS_BUSY);
  flash_deselect();
}

void flash_wait_for_wel(void) {
  flash_select();
  spi_send(INSTR_READ_STATUS1);
  while (!(spi_send(DONTCARE) & STATUS_WEL));
  flash_deselect();
}

bool_t flash_busy(void) {
  uint8_t status;

  flash_select();
  spi_send(INSTR_READ_STATUS1);
  status = spi_send(DONTCARE);
  flash_deselect();

  return status & STATUS_BUSY;
}
