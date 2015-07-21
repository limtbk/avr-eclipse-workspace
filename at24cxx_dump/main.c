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

	uint8_t ch = usart_getchr();
	while (1) {
		if (ch == 'S') {
			break;
		}
	}

	usart_printstr("\n\rDump:\n\r");
	for (int i = 0; i < 4096; i++) {
		uint8_t d_i = at24cxx_readbyte(i);
		usart_printhex(d_i);
	}
	usart_printstr("\n\rErase:\n\r");
	uint32_t mask = 0xFFFFFFFF;
	uint32_t check = 0;
	for (int i = 0; i < 1024; i++) {
		at24cxx_write_block(i*4, &mask, 4);
		usart_putchr('.');
		timer1_delay_ms(5);
		at24cxx_read_block(i*4, &check, 4);
		if (check == mask) {
			usart_putchr('.');
		} else {
			usart_putchr('E');
		}
		timer1_delay_ms(5);
	}
	usart_printstr("\n\rCheck:\n\r");
	for (int i = 0; i < 4096; i++) {
		uint8_t d_i = at24cxx_readbyte(i);
		usart_printhex(d_i);
	}
	usart_printstr("\n\rDone\n\r");

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
