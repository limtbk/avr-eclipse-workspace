/*
 * main.c
 *
 *  Created on: Mar 7, 2015
 *      Author: lim
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000

#include "ports.h"
#include "usart.h"

#define RELAY_PIN0 PORTA_D2
#define RELAY_PIN1 PORTA_D3
#define RELAY_PIN2 PORTA_D4
#define RELAY_PIN3 PORTA_D5

uint8_t status = 0;
//uint32_t cc = 0;

void init(void) {
	usart_init();
	SETP(RELAY_PIN0);
	SETP(RELAY_PIN1);
	SETP(RELAY_PIN2);
	SETP(RELAY_PIN3);
	SETD(RELAY_PIN0);
	SETD(RELAY_PIN1);
	SETD(RELAY_PIN2);
	SETD(RELAY_PIN3);
	sei();
}

void loop() {
	if (usart_chrready()) {
		char ch = usart_getchr();
		switch (ch) {
			case 'h':
			{
				usart_printstr("\n\r"
						"h - help\n\r"
						"o# - open relay\n\r"
						"c# - close relay\n\r"
						"s = status\n\r");
				break;
			}
			case 'c':
			{
				char rel_ch = usart_getchr();
				switch (rel_ch) {
					case '0': {
						SETP(RELAY_PIN0);
						status |= 1<<0;
						break;
					}
					case '1': {
						SETP(RELAY_PIN1);
						status |= 1<<1;
						break;
					}
					case '2': {
						SETP(RELAY_PIN2);
						status |= 1<<2;
						break;
					}
					case '3': {
						SETP(RELAY_PIN3);
						status |= 1<<3;
						break;
					}
					default: {
						break;
					}
				}
				break;
			}
			case 'o':
			{
				char rel_ch = usart_getchr();
				switch (rel_ch) {
					case '0': {
						CLRP(RELAY_PIN0);
						status &= ~(1<<0);
						break;
					}
					case '1': {
						CLRP(RELAY_PIN1);
						status &= ~(1<<1);
						break;
					}
					case '2': {
						CLRP(RELAY_PIN2);
						status &= ~(1<<2);
						break;
					}
					case '3': {
						CLRP(RELAY_PIN3);
						status &= ~(1<<3);
						break;
					}
					default: {
						break;
					}
				}
				break;
			}
			case 's':
			{
				usart_printhex(status);
				break;
			}
			default:
			{
				break;
			}
		}
		usart_printstr("\n\rok\n\r");
	}
}

int main(void)
{
	init();
	while (1) {
		loop();
	}
	return 0;
}
