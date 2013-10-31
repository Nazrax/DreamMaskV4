#ifndef _SPI_H_
#define _SPI_H_

#define DONTCARE 0

#include <avr/io.h>

void spi_init(void);
uint8_t spi_send(uint8_t);

#endif
