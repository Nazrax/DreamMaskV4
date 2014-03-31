#ifndef _MORSE_PATTERNS_H_
#define _MORSE_PATTERNS_H_

#include <avr/pgmspace.h>

const char morse_a[] PROGMEM = ".-"; 
const char morse_b[] PROGMEM = "-..."; 
const char morse_c[] PROGMEM = "-.-."; 
const char morse_d[] PROGMEM = "-.."; 
const char morse_e[] PROGMEM = "."; 
const char morse_f[] PROGMEM = "..-."; 
const char morse_g[] PROGMEM = "--."; 
const char morse_h[] PROGMEM = "...."; 
const char morse_i[] PROGMEM = ".."; 
const char morse_j[] PROGMEM = ".---"; 
const char morse_k[] PROGMEM = "-.-"; 
const char morse_l[] PROGMEM = ".-.."; 
const char morse_m[] PROGMEM = "--"; 
const char morse_n[] PROGMEM = "-."; 
const char morse_o[] PROGMEM = "---"; 
const char morse_p[] PROGMEM = ".--."; 
const char morse_q[] PROGMEM = "--.-"; 
const char morse_r[] PROGMEM = ".-."; 
const char morse_s[] PROGMEM = "..."; 
const char morse_t[] PROGMEM = "-"; 
const char morse_u[] PROGMEM = "..-"; 
const char morse_v[] PROGMEM = "...-"; 
const char morse_w[] PROGMEM = ".--"; 
const char morse_x[] PROGMEM = "-..-"; 
const char morse_y[] PROGMEM = "-.--"; 
const char morse_z[] PROGMEM = "--.."; 
const char morse_1[] PROGMEM = ".----"; 
const char morse_2[] PROGMEM = "..---"; 
const char morse_3[] PROGMEM = "...--"; 
const char morse_4[] PROGMEM = "....-"; 
const char morse_5[] PROGMEM = "....."; 
const char morse_6[] PROGMEM = "-...."; 
const char morse_7[] PROGMEM = "--..."; 
const char morse_8[] PROGMEM = "---.."; 
const char morse_9[] PROGMEM = "----."; 
const char morse_0[] PROGMEM = "-----";

const char * const morse_num_table[] PROGMEM = {
  morse_0,
  morse_1,
  morse_2,
  morse_3,
  morse_4,
  morse_5,
  morse_6,
  morse_7,
  morse_8,
  morse_9
};

const char * const morse_alpha_table[] PROGMEM = {   
  morse_a,
  morse_b,
  morse_c,
  morse_d,
  morse_e,
  morse_f,
  morse_g,
  morse_h,
  morse_i,
  morse_j,
  morse_k,
  morse_l,
  morse_m,
  morse_n,
  morse_o,
  morse_p,
  morse_q,
  morse_r,
  morse_s,
  morse_t,
  morse_u,
  morse_v,
  morse_w,
  morse_x,
  morse_y,
  morse_z
};

#endif
