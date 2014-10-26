/*
 * hcsr04.c
 *
 *  Created on: Sep 27, 2014
 *      Author: lim
 */

#include <avr/io.h>
#include "timer1.h"
#include "hcsr04.h"

void hcsr04_init(void) {
	CLRP(HCSR04_ECHO);
	CLRD(HCSR04_ECHO);
	CLRP(HCSR04_TRIG);
	SETD(HCSR04_TRIG);
}

void trigger() {
	INVP(TXLED);
	SETP(HCSR04_TRIG);
	timer1_delay_us2(20); //10us
	CLRP(HCSR04_TRIG);
	INVP(TXLED);
}

uint16_t measure() {
	INVP(RXLED);
	while (!PINV(HCSR04_ECHO)) {
	}
	timer1_measureTime();
	while (PINV(HCSR04_ECHO)) {
	}
	uint16_t result = timer1_measureTime();
	INVP(RXLED);
	return result;
}

uint16_t hcsr04_getDistance() {
	trigger();
	return measure();
}
