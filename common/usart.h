/*
 * usart.h
 *
 *  Created on: May 30, 2014
 *      Author: lim
 */
#ifndef USART_H_
#define USART_H_

#define BAUDRATE 57600
#define BAUDDIVIDER (F_CPU/(16*BAUDRATE)-1)
#define USART_TX_BUF 32
#define USART_RX_BUF 32

void usart_init(void);
void usart_putchr(char ch);
char usart_chrready(void);
char usart_getchr(void);
void usart_printstr(char *string);
char usart_chartohex(char c);
char usart_hextochar(char c);
void usart_printhex(char c);

#endif /* USART_H_ */
