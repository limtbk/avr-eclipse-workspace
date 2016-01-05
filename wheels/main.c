#include <avr/io.h>
#include <avr/interrupt.h>
#include "ports.h"
#include "usart.h"
#include "macro.h"
#include "timer1.h"

#define M0P PORTA_D2
#define M0N PORTA_D3
#define M1P PORTA_D4
#define M1N PORTA_D5
#define M2P PORTA_D6
#define M2N PORTA_D7
#define M3P PORTA_D8
#define M3N PORTA_D9

void init(void) {
#ifdef __AVR_ATmega32U4__
	USBCON = (0<<USBE)|(1<<FRZCLK)|(0<<OTGPADE)|(0<<VBUSTE);
#endif
	usart_init();
	timer1_init();
	CLRP(M0P);
	CLRP(M0N);
	CLRP(M1P);
	CLRP(M1N);
	CLRP(M2P);
	CLRP(M2N);
	CLRP(M3P);
	CLRP(M3N);
	SETD(M0P);
	SETD(M0N);
	SETD(M1P);
	SETD(M1N);
	SETD(M2P);
	SETD(M2N);
	SETD(M3P);
	SETD(M3N);
	sei();
}

int main(void)
{
	init();

	int16_t m0 = 0;
	int16_t m1 = 0;

	usart_printstr("\n\rWheels\n\r");

	uint8_t pwmt = 0;
	uint8_t st = 16;

	while (1) {
		pwmt++;

		if (usart_chrready()) {
			char ch = usart_getchr();
			usart_putchr(ch);
			switch (ch) {
				case 'w':
				{
					m0 += st;
					m1 += st;
					break;
				}
				case 's':
				{
					m0 -= st;
					m1 -= st;
					break;
				}
				case 'a':
				{
					m0 -= st;
					m1 += st;
					break;
				}
				case 'd':
				{
					m0 += st;
					m1 -= st;
					break;
				}
				default:
				{
					m0 = 0;
					m1 = 0;
					break;
				}
			}
		}

		if (m0 > 255) {
			m0 = 255;
		}
		if (m0 < -255) {
			m0 = -255;
		}

		if (m1 > 255) {
			m1 = 255;
		}
		if (m1 < -255) {
			m1 = -255;
		}

		uint16_t m0s = ABS(m0);
		uint16_t m1s = ABS(m1);
		uint8_t m0d = (m0<0)?0:1;
		uint8_t m1d = (m1<0)?0:1;

		if (m0s>pwmt) {
			if (m0d == 0) {
				SETP(M0P);
				CLRP(M0N);
			} else {
				SETP(M0N);
				CLRP(M0P);
			}
		} else {
			CLRP(M0N);
			CLRP(M0P);
		}

		if (m1s>pwmt) {
			if (m1d == 0) {
				SETP(M1P);
				CLRP(M1N);
			} else {
				SETP(M1N);
				CLRP(M1P);
			}
		} else {
			CLRP(M1N);
			CLRP(M1P);
		}

//		SETP(M0P);
//		CLRP(M0N);
//		SETP(M1P);
//		CLRP(M1N);
//		for (uint32_t i = 0; i < 3276800; i++) {
//		}
//		SETP(M0N);
//		CLRP(M0P);
//		SETP(M1N);
//		CLRP(M1P);
//		for (uint32_t i = 0; i < 3276800; i++) {
//		}



	}

	return 0;
}

#ifdef __AVR_ATmega32U4__
ISR(USB_COM_vect) {
}

ISR(USB_GEN_vect) {
}
#endif
