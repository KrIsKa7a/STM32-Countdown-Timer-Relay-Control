#ifndef USART_H_
#define USART_H_

#include "stdint.h"

void usart3_rx_tx_init(void);
uint8_t usart3_read(void);
uint8_t usart3_read_countdown_start(int number);
void usart3_write_ch(int ch);
void usart3_write(char* str, int len);
void usart3_writeLine(char* str, int len);

#endif
