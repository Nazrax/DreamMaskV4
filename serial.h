#ifndef _SERIAL_H_
#define _SERIAL_H_

#define usart_send() serial_out_ctr=0; flag_serial_sending = true; usart_send_next_byte();

inline void usart_init(void);
void usart_reset_output(void);
void usart_send_next_byte(void);
void usart_dostuff(void);

#endif // _SERIAL_H_
