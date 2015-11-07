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

	uint8_t pwmarr[] = {0, 0, 0, 0};


	usart_printstr("\n\rFish on wheels\n\r");

	while (1) {
		SETP(M0P);
		CLRP(M0N);
		SETP(M1P);
		CLRP(M1N);
		for (uint32_t i = 0; i < 3276800; i++) {
		}
		SETP(M0N);
		CLRP(M0P);
		SETP(M1N);
		CLRP(M1P);
		for (uint32_t i = 0; i < 3276800; i++) {
		}



	}

	return 0;
}

#ifdef __AVR_ATmega32U4__
ISR(USB_COM_vect) {
}

ISR(USB_GEN_vect) {
}
#endif
