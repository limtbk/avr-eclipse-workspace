#include <avr/io.h>
#include <avr/interrupt.h>
#include "ports.h"
#include "usart.h"
#include "macro.h"

void init(void) {

//	CLRD(PORTA_D2);
//	CLRP(PORTA_D2);

	PORTD = 0b00000000;
	DDRD  = 0b00000011;
	PORTB = 0b00000000;
	DDRB =  0b00000000;
	PORTC = 0b00000000;
	DDRC =  0b00000000;

	SETD(PORTA_D13);
	CLRP(PORTA_D13);

#ifdef __AVR_ATmega32U4__
	USBCON = (0<<USBE)|(1<<FRZCLK)|(0<<OTGPADE)|(0<<VBUSTE);
#endif

	usart_init();
	sei();
}

int main(void)
{
	init();

	usart_printstr("\n\rCapacity sensor\n\r");

	while (1) {
		uint16_t tarr[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

		cli();
		for (uint8_t i = 0; i < 255; i++) {
			CLRP(PORTA_D13);
			uint8_t ok = 1;
			while (ok) {
				uint16_t pins = ((PIND & 0b11111100) >> 2) | ((PINB & 0b00011111) << 6) | ((PINC & 0b00000001) << 11);
				ok = (pins!=0);
				for (uint8_t ti = 0; ti < 12; ti++) {
					tarr[ti] += pins & 1;
					pins = pins >> 1;
				}
			}
			SETP(PORTA_D13);
			while (ok) {
				uint16_t pins = ((PIND & 0b11111100) >> 2) | ((PINB & 0b00011111) << 6) | ((PINC & 0b00000001) << 11);
				pins = pins ^ 0b0000111111111111;
				ok = (pins!=0);
				for (uint8_t ti = 0; ti < 12; ti++) {
					tarr[ti] += pins & 1;
					pins = pins >> 1;
				}
			}
		}
		sei();
		for (uint8_t ti = 0; ti < 12; ti++) {
			usart_printhex(HI(tarr[ti]));
			usart_printhex(LO(tarr[ti]));
			usart_putchr(' ');
		}
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
