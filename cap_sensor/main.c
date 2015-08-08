#include <avr/io.h>
#include <avr/interrupt.h>
#include "ports.h"
//#include "timer1.h"
#include "usart.h"
#include "macro.h"

void init(void) {

	CLRD(PORTA_D2);
	CLRP(PORTA_D2);

	SETD(PORTA_D13);
	CLRP(PORTA_D13);


#ifdef __AVR_ATmega32U4__
	USBCON = (0<<USBE)|(1<<FRZCLK)|(0<<OTGPADE)|(0<<VBUSTE);
#endif
//	timer1_init();
	usart_init();
	sei();
}

int main(void)
{
	init();

	usart_printstr("\n\rCapacity sensor\n\r");

//	timer1_delay_ms(1000);

	for (uint16_t i = 0; i < 2048; i++) {
		usart_printhex(LO(i));
	}

	usart_printstr("\n\rIt's measure time!\n\r");

	while (1) {
		uint16_t time = 0;
		for (uint8_t i = 0; i < 255; i++) {
//			CLRP(PORTA_D2);
//			SETD(PORTA_D2);
//			CLRD(PORTA_D2);

			uint16_t t = 0;
			CLRP(PORTA_D13);
			while (PINV(PORTA_D2)) {
				t++;
			}

//			SETP(PORTA_D2);
			SETP(PORTA_D13);
			while (!PINV(PORTA_D2)) {
				t++;
			}

			time += t;
		}

//		INVP(PORTA_D13);

//		timer1_measureTime();

//		uint16_t time = timer1_measureTime();

//		usart_printhex(time);
		usart_printhex(HI(time));
		usart_printhex(LO(time));
//		usart_putchr(' ');
		usart_printstr("\n\r");
	}

	return 0;
}

#ifdef __AVR_ATmega32U4__
ISR(USB_COM_vect) {
}

ISR(USB_GEN_vect) {
}
#endif
