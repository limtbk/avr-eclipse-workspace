#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "ports.h"
#include "timer1.h"
#include "usart.h"
#include "i2c.h"
#include "ds13xx.h"
#include "at24cxx.h"
#include "macro.h"

void init(void) {
	SETD(TXLED);
	SETD(RXLED);

#ifdef __AVR_ATmega32U4__
	USBCON = (0<<USBE)|(1<<FRZCLK)|(0<<OTGPADE)|(0<<VBUSTE);
#endif
	timer1_init();
	usart_init();
	i2c_init();
	sei();
}

int main(void)
{
	init();
	// Dump at24c memory to uart

	usart_printstr("Dump: ");
	for (int i = 0; i < 4096; i++) {
		uint8_t d_i = at24cxx_readbyte(i);
		usart_printhex(d_i);
	}
	usart_printstr(" End");

	while (1) {
	}
	return 0;
}

#ifdef __AVR_ATmega32U4__
ISR(USB_COM_vect) {
}

ISR(USB_GEN_vect) {
}
#endif
