#include "globals.h"
#include "serial.h"
#include "flash.h"
#include "commands.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#define SOH 1
#define EOT 4
#define ACK 6
#define NAK 21
#define CAN 24

typedef enum {LOW, HIGH} lowhigh_t;

volatile char serial_in[64];
char serial_out[132];
volatile uint8_t serial_in_ctr;
volatile uint8_t serial_out_ctr;
static uint8_t packet_number = 1;
static lowhigh_t lowhigh;

static void prepare_packet(void);

inline void usart_init(void) {
  UBRR0 = BAUD_VALUE;
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01); // 8-bit, 1 stop bit, no parity
  UCSR0B = _BV(TXEN0) | _BV(TXCIE0) | _BV(RXEN0) | _BV(RXCIE0); // Turn on TX and RX and their interrupts
  //UCSR0A = _BV(U2X0); // Double serial clock
  flag_xmodem_sending = flag_xmodem_finalizing = flag_xmodem_next_packet = false;

}

void usart_send_next_byte(void) {
  bool_t length_ok = serial_out_ctr < 132;
  bool_t notnull = serial_out[serial_out_ctr] != '\0';

  if (length_ok && ((flag_xmodem_sending && !flag_xmodem_finalizing) || notnull)) {
    while (!(UCSR0A & _BV(UDRE0))); // Ensure empty transmit buffer
    UDR0 = serial_out[serial_out_ctr];
    serial_out_ctr++;
  } else {
    flag_serial_sending = false;
    if (flag_xmodem_finalizing) {
      flash_scan();
      flag_xmodem_sending = false;
      flag_xmodem_finalizing = false;
    }
  }
}

void usart_dostuff() {
  if (flag_command_ready)
    handle_command();

  if (flag_read_flash) {
    flash_read(flash_addr);
    flash_addr++;
    flag_read_flash = false;
  }

  if (flag_xmodem_next_packet) {
    prepare_packet();
    usart_send();
  }
}

static void prepare_packet(void) {
  uint8_t start = 0;
  if (lowhigh == HIGH) {
    start = 128;
  }

  if (flash_buf[2] == 0xFF || flash_full()) {
    PORTB ^= _BV(PORTB0); // Turn on LEDs


    serial_out[0] = EOT;
    serial_out[1] = '\0';
  } else {
    int i;
    uint8_t sum = 0;

    serial_out[0] = SOH;
    serial_out[1] = packet_number;
    serial_out[2] = ~packet_number;
    packet_number++;
    for(i=0; i<128; i++) {
      serial_out[i+3] = flash_buf[start+i];
      sum += flash_buf[start+i];
    }
    serial_out[131] = sum;

    if (lowhigh == LOW) {
      lowhigh = HIGH;
    } else {
      lowhigh = LOW;
      flag_read_flash = true;
    }
  }
  flag_xmodem_next_packet = false;
}


void usart_reset_output(void) {
  int i;
  for(i=0; i<132; i++)
    serial_out[i] = 0;
}

ISR(USART_TX_vect) {
  usart_send_next_byte();
}

ISR(USART_RX_vect) {
  char chr = UDR0;

//flag_user_active = true;
 //last_activity = clock_ticks;

  if (flag_xmodem_sending) {
    if (chr == NAK) {
      if (serial_out[0] == EOT) {
        flag_xmodem_finalizing = true;
      }
      serial_out[0] = EOT;
      serial_out[1] = '\0';
      usart_send();
    } else if (chr == ACK) {
      if (flash_buf[2] == 0xFF) {
        //if (packet_number > 1) {
        serial_out[0] = EOT;
        serial_out[1] = '\0';
        usart_send();
      } else {
        flag_xmodem_next_packet = true;
      }
    } else if (chr == CAN) {
      flag_xmodem_sending = false;
      flag_xmodem_finalizing = false;
      flash_scan();
    }
  } else {
    if (chr == '\r' || chr == '\n') {
      flag_command_ready = true;
    } else if (chr == NAK) {
      flag_xmodem_next_packet = true;
      flag_xmodem_sending = true;
      flag_read_flash = true;
      flag_xmodem_finalizing = false;

      flash_addr = 0;
      lowhigh = LOW;
      packet_number = 1;
    } else if (chr == CAN || chr == ACK) {
      // Ignore
    } else {
      while (!(UCSR0A & _BV(UDRE0))); // Ensure empty transmit buffer
      if (chr == 8) { // backspace
        if (serial_in_ctr > 0) {
          serial_in_ctr--;
          UDR0 = chr; // echo
          while (!(UCSR0A & _BV(UDRE0))); // Ensure empty transmit buffer
          UDR0 = ' ';
          while (!(UCSR0A & _BV(UDRE0))); // Ensure empty transmit buffer
          UDR0 = 8;
        }
      } else if (serial_in_ctr < 63) {
        UDR0 = chr; // echo
        serial_in[serial_in_ctr] = chr;
        serial_in_ctr++;
      } else {
        UDR0 = '*';
      }
    }
  }
}
