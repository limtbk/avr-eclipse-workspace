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

void initUSART(void);
void putchr(char ch);
char chrready(void);
char getchr(void);
void printstr(char *string);
char chartohex(char c);
char hextochar(char c);
void printhex(char c);

#endif /* USART_H_ */
