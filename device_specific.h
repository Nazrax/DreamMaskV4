/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *      Device specific definitions
 *
 * \par Application note:
 *      AVR055: Using a 32 kHz crystal to calibrate the internal RC oscillator
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Name: RELEASE_1_1 $
 * $Revision: 1.2 $
 * $RCSfile: device_specific.h,v $
 * $Date: 2006/02/17 12:49:26 $  \n
 ******************************************************************************/


#ifndef __DEVICE_SPECIFIC_H__
#define __DEVICE_SPECIFIC_H__

// ATmega88P
#define ASYNC_TIMER                        AS2
#define NO_PRESCALING                      CS20
#define ASYNC_TIMER_CONTROL_REGISTER       TCCR2B
#define ASYNC_TIMER_CONTROL_UPDATE_BUSY    TCR2AUB
#define OUTPUT_COMPARE_UPDATE_BUSY         OCR2AUB
#define TIMER_UPDATE_BUSY                  TCN2UB
#define TIMER                              TCNT2
#define OSCCAL_RESOLUTION                  7
#define LOOP_CYCLES                        7
#define TWO_RANGES

#endif

