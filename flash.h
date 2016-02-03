#ifndef _FLASH_H_
#define _FLASH_H_

#include <avr/io.h>

#include "globals.h"
#include "spi.h"
#include "types.h"

#define INSTR_POWERDOWN 0xB9
#define INSTR_POWERUP 0xAB

#define INSTR_READ_DATA 0x03
#define INSTR_FAST_READ 0x0B

#define INSTR_WRITE_ENABLE 0x06
#define INSTR_PAGE_PROGRAM 0x02
#define INSTR_CHIP_ERASE 0x0C7

#define INSTR_READ_STATUS1 0x05
#define INSTR_READ_STATUS2 0x35

#define STATUS_BUSY 0x01
#define STATUS_WEL  0x02

#define flash_select() PORTC &= ~(_BV(PORTC4))
#define flash_deselect() PORTC |= _BV(PORTC4)

#define flash_powerdown() if (!flag_flash_power_hold) { flash_select();spi_send(INSTR_POWERDOWN);flash_deselect(); }
#define flash_powerup() if (!flag_flash_power_hold) { flash_select();spi_send(INSTR_POWERUP);flash_deselect(); }
#define flash_power_hold(x) flag_flash_power_hold = x;

#define flash_full() (flash_addr > 4093)

void flash_init(void);
void flash_doheader(void);
void flash_condwrite(void);
void flash_erase(void);
void flash_write(uint16_t);
void flash_read(uint16_t);
void flash_fast_read(uint16_t);
void flash_scan(void);
bool_t flash_verify(uint16_t);
uint32_t flash_adler32(void);

#endif // _FLASH_H_
