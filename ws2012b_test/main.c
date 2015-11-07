/*
 * main.c
 *
 *  Created on: Nov 7, 2015
 *      Author: lim
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "ports.h"
#include "macro.h"
#include "usart.h"
#include "i2c.h"
#include "ds13xx.h"

#define LED PORTA_D13
#define LEDCTRL PORTA_D3

#define send_bytesm(port__, ddr__, pin__, portnum__, source__) \
			uint8_t __bitcnt__ = 8;\
			asm volatile (\
				"	ldi		%[bitcnt],8"			"\n\t"\
				"btloop%=:"							"\n\t"\
				"	sbi		%[port],%[nn]"			"\n\t"\
				"	nop"							"\n\t"\
				"	rol		%[byte]"				"\n\t"\
				"	brcs	bit1%="					"\n\t"\
				"	cbi		%[port],%[nn]"			"\n\t"\
				"	nop"							"\n\t"\
				"	nop"							"\n\t"\
				"	rjmp	bit0%="					"\n\t"\
				"bit1%=:"							"\n\t"\
				"	nop"							"\n\t"\
				"	nop"							"\n\t"\
				"	nop"							"\n\t"\
				"	nop"							"\n\t"\
				"	nop"							"\n\t"\
				"bit0%=:"							"\n\t"\
				"	cbi		%[port],%[nn]"			"\n\t"\
				"	nop"							"\n\t"\
				"	nop"							"\n\t"\
				"	nop"							"\n\t"\
				"	nop"							"\n\t"\
				"	subi	%[bitcnt],1"			"\n\t"\
				"	brne	btloop%="				"\n\t"\
				::\
				[port] "I" (_SFR_IO_ADDR(port__)),\
				[nn] "I" (portnum__),\
				[bitcnt] "d" (__bitcnt__),\
				[byte] "r" (source__)\
			)

#define send_bytes(port, source) send_bytesm(port, source)

void init() {
	usart_init();
	i2c_init();
	SETD(LED);
	SETD(LEDCTRL);

	usart_printstr("\n\r2812b test\n\r");
}

void setcolor(uint8_t r, uint8_t g, uint8_t b) {
	uint8_t current_pixels[3] = {g, r, b};
	cli();
	for (uint8_t i = 0; i<3; i++) {
		uint8_t ibyte = current_pixels[i+0];
		send_bytes(LEDCTRL, ibyte);
	}
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
						"crrggbb - set rgb color\n\r"
						);
				break;
			}
			case 'c':
			{
				usart_printstr("color");
				uint8_t rcol = usart_hextochar(usart_getchr())*16;
				rcol += usart_hextochar(usart_getchr());
				usart_printhex(rcol);
				uint8_t gcol = usart_hextochar(usart_getchr())*16;
				gcol += usart_hextochar(usart_getchr());
				usart_printhex(gcol);
				uint8_t bcol = usart_hextochar(usart_getchr())*16;
				bcol += usart_hextochar(usart_getchr());
				usart_printhex(bcol);
				setcolor(rcol, gcol, bcol);
			}
			default:
				break;
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
