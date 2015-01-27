#include "adc.h"
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
  adc_block();
}

void morse_handle(void) {
  led_unblock();

  if(morse_sending) {
    char currentChar = morse_out[morse_char_ctr];

    if (currentChar == '\0') {
      morse_sending = false;
      adc_unblock();
      return;
    }
    
    uint8_t omcc = morse_char_ctr;
    uint8_t omtc = morse_tick_ctr;
    uint8_t omec = morse_element_ctr;

    char element = 'x';
    bool_t wantLeds = false;
    int8_t idx = -1;
    if (currentChar == ' ') {
      // LEDs stay off
      if (morse_tick_ctr > 2) {
	morse_tick_ctr = 0;
	morse_element_ctr = 0;
	morse_char_ctr++;
      } else {
	morse_tick_ctr++;
      }
    } else {
      const char* const * table;

      if (currentChar >= '0' && currentChar <= '9') {
	idx = currentChar - '0';
	table = morse_num_table;
      } else {
	table = morse_alpha_table;
	
	if (currentChar >= 'A' && currentChar <= 'Z') {
	  idx = currentChar - 'A';
	} else if (currentChar >= 'a' && currentChar <= 'z') {
	  idx = currentChar - 'a';
	}
      }

      if (idx >= 0) {
	char *elements = (char*)pgm_read_word(&(table[idx]));
	element = pgm_read_byte(&elements[morse_element_ctr]);

	if (element == '\0') {
	  if (morse_tick_ctr > 0) {
	    morse_element_ctr = 0;
	    morse_tick_ctr = 0;
	    morse_char_ctr++;
	  } else {
	    morse_tick_ctr++;
	  }
	  element = '*';
	} else {
	  uint8_t ticks;

	  if (element == '.') {
	    ticks = 1;
	  } else {
	    ticks = 3;
	  }

	  if (morse_tick_ctr < ticks) {
	    wantLeds = true;
	    morse_tick_ctr++;
	  } else {
	    morse_tick_ctr = 0;
	    morse_element_ctr++;
	  }
	} 
      }
    }

    sprintf(serial_out, "%d %d %d %c %c %d\r\n", omcc, omec, omtc, currentChar, element, wantLeds);

    if (wantLeds) {
      led_want(0);
      led_want(1);
    }
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
