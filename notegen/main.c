/*
 * main.c
 *
 *  Created on: Oct 8, 2013
 *      Author: lim
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "ports.h"

const uint16_t PROGMEM frequencies[] = {466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880};

#define FREQUENCIES(index) pgm_read_word(&frequencies[index])

#define SAMPLE_RATE_POW 13 //2^15=32768 samples per second
#define SAMPLE_RATE ((uint16_t)1<<SAMPLE_RATE_POW)
#define SAMPLE_RATE_DIV2 ((uint16_t)(1<<(SAMPLE_RATE_POW-1)))
#define SAMPLE_RATE_MASK ((uint16_t)(SAMPLE_RATE - 1))
#define OSC2_PRESCALLER 8
#define OSC2_OCRVALUE ((F_CPU>>SAMPLE_RATE_POW)/OSC2_PRESCALLER)
#define SIMULTANEOUS_NOTES 6

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168__)
#define LED_PORT PORT_B5
#elif defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__)
#define LED_PORT PORT_B0
#endif

uint16_t playingNotesFreq[SIMULTANEOUS_NOTES];
uint16_t playingNotesPhase[SIMULTANEOUS_NOTES];

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168__)
ISR(TIMER2_COMPA_vect)
{
	cli();

	uint8_t buf = 0;
	uint8_t msk = 1;
    for (uint8_t l = 0; l < 6; l++) {

    	uint16_t keyFreq = playingNotesFreq[l];
		uint16_t phase = playingNotesPhase[l];
		phase += keyFreq;
		playingNotesPhase[l] = phase;

		uint16_t ab = phase & SAMPLE_RATE_MASK;

		buf |= (ab<SAMPLE_RATE_DIV2)?0:msk;
        msk = msk << 1;
	}

    PORTB = buf;

    sei();
}

void initPWM()
{
	TCCR2A = (1<<WGM21)|(0<<WGM20); //CTC mode
	TCCR2B = (0b010<<CS20); //prescaller CLK/8
 	OCR2A = OSC2_OCRVALUE-1; // 16 000 000 / 32 768 / 8 = 61
 	TIMSK2 = (1<<OCIE2A); //Interrupt on second timer compare
}
#endif


void init()
{
	initPWM();
	SETD(LED_PORT);
	DDRB = 0b00111111;

    for (uint8_t l = 0; l < SIMULTANEOUS_NOTES; l++) {
    	playingNotesFreq[l] = FREQUENCIES(l);
		playingNotesPhase[l] = 0;
	}
    sei();
}

int main()
{
	init();
	SETP(LED_PORT);

	while (1) {
	}

	return 0;
}
