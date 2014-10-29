/*
 * main.c
 *
 *  Created on: Oct 28, 2014
 *      Author: lim
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "ports.h"
#include "timer1.h"

#define LAN0 PORT_D2
#define LAN1 PORT_D3
#define LAN2 PORT_D4
#define LAN3 PORT_C5
#define LAN4 PORT_C4
#define LAN5 PORT_C3
#define LAN6 PORT_C2
#define LAN7 PORT_C1

#define BITMASKD ((1<<PD2) | (1<<PD3) | (1<<PD4))
#define NBITMASKD (~BITMASKD)
#define BITMASKC ((1<<PC1) | (1<<PC2) | (1<<PC3) | (1<<PC4) | (1<<PC5))
#define NBITMASKC (~BITMASKC)

void enable_outs() {
	DDRD |= BITMASKD;
	DDRC |= BITMASKC;
}

void disable_outs() {
	DDRD &= NBITMASKD;
	DDRC &= NBITMASKC;
}

void set_outs(uint8_t val) {
	PORTD = (PORTD & NBITMASKD) | ((val >> (7-PD4)) & BITMASKD);
	PORTC = (PORTC & NBITMASKC) | ((val << (PC5-4)) & BITMASKC);
}

void init() {
	timer1_init();
	sei();
	timer1_delay_ms(100);
}

void loop() {
	uint8_t rp = 0b11111111;
	uint8_t rv = 0b00000000;
	enable_outs();
	for (uint8_t sh = 0; sh < 8; sh++) {
		rv = rp & 0b01010101;
		set_outs(rv);
		timer1_delay_ms(5);
		set_outs(0);
		timer1_delay_ms(5);
		rv = rp & 0b10101010;
		set_outs(rv);
		timer1_delay_ms(5);
		set_outs(0);
		timer1_delay_ms(5);
		rp = rp << 1;
	}
	disable_outs();
	timer1_delay_ms(80);
}

int main (void) {
	init();

	while (1) {
		loop();
	}

	return 0;
}
