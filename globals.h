#ifndef _GLOBALS_H_
#define _GLOBALS_H_

//#define DEBUG 1

#define F_CPU 3686400 // 3.6864 MhZ clock

#include <avr/io.h>
#include "types.h"

#define USART_BAUDRATE 230400
//#define USART_BAUDRATE 57600
#define BAUD_VALUE (((F_CPU / (USART_BAUDRATE * 16))) - 1)

#define BUTTON_COUNT 2
#define TICKS_PER_SECOND 8

#define REGISTER_BIT(...) REGISTER_BIT_(__VA_ARGS__)
#define REGISTER_BIT_(rg,bt) ((volatile _io_reg*)&rg)->bit##bt 

// GPIOR0
#define SERIAL_SENDING GPIOR0, 0
#define XMODEM_SENDING GPIOR0, 1
#define XMODEM_FINALIZING GPIOR0, 2
#define NEXT_PACKET GPIOR0, 3
#define FLASH_POWER GPIOR0, 4
#define READ_FLASH GPIOR0, 5
#define IN_MENU GPIOR0, 6

// GPIOR1
#define COMMAND_READY GPIOR1, 0
#define DO_ADC GPIOR1, 1
#define WANT_ADC GPIOR1, 2
#define WANT_ADC_HEADER GPIOR1, 3
#define USER_ACTIVE GPIOR1, 4
#define SPI_VERBOSE GPIOR1, 5
#define ADC_VERBOSE GPIOR1, 6
#define FLASH_VERBOSE GPIOR1, 7

// GPIOR2
#define ALARM_ACTIVE GPIOR2, 0
#define FLAG_DID_ADC GPIOR2, 1

// Flags
#define flag_serial_sending REGISTER_BIT(SERIAL_SENDING)
#define flag_xmodem_sending REGISTER_BIT(XMODEM_SENDING)
#define flag_xmodem_finalizing REGISTER_BIT(XMODEM_FINALIZING)
#define flag_xmodem_next_packet REGISTER_BIT(NEXT_PACKET)
#define flag_flash_power REGISTER_BIT(FLASH_POWER)
#define flag_read_flash REGISTER_BIT(READ_FLASH)
#define flag_in_menu REGISTER_BIT(IN_MENU)

#define flag_command_ready REGISTER_BIT(COMMAND_READY)
#define flag_do_adc REGISTER_BIT(DO_ADC)
#define flag_want_adc REGISTER_BIT(WANT_ADC)
#define flag_want_header REGISTER_BIT(WANT_ADC_HEADER)
#define flag_user_active REGISTER_BIT(USER_ACTIVE)
#define flag_spi_verbose REGISTER_BIT(SPI_VERBOSE)
#define flag_adc_verbose REGISTER_BIT(ADC_VERBOSE)
#define flag_flash_verbose REGISTER_BIT(FLASH_VERBOSE)

#define alarm_active REGISTER_BIT(ALARM_ACTIVE)
#define flag_did_adc REGISTER_BIT(FLAG_DID_ADC)

// Clock stuff
extern clock_t clock;
extern volatile uint32_t clock_ticks;

// Flash stuff
extern uint16_t flash_addr;
extern uint8_t flash_buf[256];
extern volatile uint16_t flash_buf_ctr;

// Serial stuff
extern char serial_out[132];
extern volatile char serial_in[64];
extern volatile uint8_t serial_in_ctr;
extern volatile uint8_t serial_out_ctr;

// Buttons
button_t buttons[BUTTON_COUNT];
bool_t doublePress, longDoublePress, reallyLongDoublePress;
uint8_t buttonPresses;


#endif
