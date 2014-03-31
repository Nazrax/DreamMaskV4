#include "globals.h"
#include "leds.h"
#include "morse.h"
#include "morse_patterns.h"
#include "serial.h"

#include <stdio.h>
#include <avr/pgmspace.h>
#include <string.h>

static uint8_t morse_char_ctr, morse_tick_ctr, morse_element_ctr;
static bool_t morse_sending = false;
char morse_out[64];

void morse_send(void) {
  morse_char_ctr = 0;
  morse_tick_ctr = 0;
  morse_element_ctr = 0;
  morse_sending = true;
}

void morse_handle(void) {
  if(morse_sending) {
    char currentChar = morse_out[morse_char_ctr];

    if (currentChar == '\0') {
      morse_sending = false;
      return;
    }

    char element = 'x';

    if (currentChar == ' ') {
      element = 's';
    } else if (currentChar >= '0' && currentChar <= '9') {
    } else {
      int8_t idx = -1;

      if (currentChar >= 'A' && currentChar <= 'Z') {
	idx = currentChar - 'A';
      } else if (currentChar >= 'a' && currentChar <= 'z') {
	idx = currentChar - 'a';
      }

      if (idx >= 0) {
	char *elements = (char*)pgm_read_word(&(morse_alpha_table[idx]));
	element = pgm_read_byte(&elements[morse_element_ctr]);
	if (element == '\0') {
	  element = 'u';
	  morse_element_ctr = 0;
	  morse_char_ctr++;
	} else {
	  morse_element_ctr++;
	}
      }
    }

    sprintf(serial_out, "%c %d '%c'\r\n", currentChar, (currentChar - 'A'), element);
    /*
    char* p = serial_out;
    p[0] = currentChar;
    p[1] = ':';
    p[2] = ' ';
    p += 3;
    p = strcpy(p, elements);
    strcpy(p, "\r\n");
    */

    usart_send();
    while (flag_serial_sending);

    //morse_char_ctr++;
  }
}
