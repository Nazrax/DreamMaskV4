#ifndef _ADC_H_
#define _ADC_H_

void adc_init(void);
void adc_dostuff(void);

void adc_block(void);
void adc_unblock(void);

uint16_t adc_voltage(void);

#endif
