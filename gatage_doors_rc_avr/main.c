/*
 * main.c
 *
 *  Created on: Mar 7, 2015
 *      Author: lim
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 14745600

#include "ports.h"
#include "usart.h"

#define RELAY_PIN PORT_B0

uint8_t status = 0;
//uint32_t cc = 0;

void init(void) {
	usart_init();
	CLRP(RELAY_PIN);
	SETD(RELAY_PIN);
	sei();
}

void loop() {
//	cc++;
//	if (cc>60000) {
//		status = !status;
//		if (status) {
//			SETP(RELAY_PIN);
//		} else {
//			CLRP(RELAY_PIN);
//		}
//		cc = 0;
//	}
	if (usart_chrready()) {
		char ch = usart_getchr();
		switch (ch) {
			case 'h':
			{
				usart_printstr("\n\r"
						"h - help\n\r"
						"o - open doors\n\r"
						"c - close doors\n\r"
						"s = status\n\r");
				break;
			}
			case 'o':
			{
				SETP(RELAY_PIN);
				status = 1;
				break;
			}
			case 'c':
			{
				CLRP(RELAY_PIN);
				status = 0;
				break;
			}
			case 's':
			{
				if (status) {
					usart_printstr("1");
				} else {
					usart_printstr("0");
				}
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
