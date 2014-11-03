/*
 * main.c
 *
 *  Created on: Nov 2, 2014
 *      Author: lim
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "ports.h"

#define RED_LED PORT_B3
#define IR_LED PORT_B4
#define RIGHT_BUTTON PORT_B0
#define LEFT_BUTTON PORT_B1

const uint16_t PROGMEM deltaTimes[] = {180, 2208, 52, 110, 46, 268, 52, 5040, 180, 2208, 52, 110, 46, 268, 52, 5000};
#define DELTA_TIMES_LENGTH 16
#define DELTA_TIMES(index) pgm_read_word(&deltaTimes[index])

const uint16_t PROGMEM deltaTimes2[] = {32, 488, 32, 5000};
#define DELTA_TIMES2_LENGTH 4
#define DELTA_TIMES2(index) pgm_read_word(&deltaTimes2[index])

uint16_t time = 0;
uint8_t send = 0;
uint8_t current_delta_index = 0;
uint16_t next_time = 0;
uint8_t ir1start = 0;
uint8_t ir2start = 0;

/*
Nikon:
 1 - 2250	-180
 0 - 27600	-2208
 1 - 650	-52
 0 - 1375	-110
 1 - 575	-46
 0 - 3350	-268
 1 - 650	-52
 0 - 63000	-5040
 1 - 2250	-180
 0 - 27600	-2208
 1 - 650	-52
 0 - 1375	-110
 1 - 575	-46
 0 - 3350	-268
 1 - 650	-52

40kHz - 25us - period
12.5us - 1 time tick - clk/8 / 15

Canon:
 1 - 480	-32
 0 - 7324	-488
 1 - 480	-32

33kHz - 30.30us - period
15us - 1 time tick - clk


 */

void init(void) {
	SETD(RED_LED);
	SETD(IR_LED);

	CLRD(RIGHT_BUTTON);
	SETP(RIGHT_BUTTON);
	CLRD(LEFT_BUTTON);
	SETP(LEFT_BUTTON);

	GIMSK = (1<<PCIE);
	PCMSK = (1<<PCINT0) | (1<<PCINT1);

	TCCR0A = (1<<WGM01) | (0<<WGM00);
	TCCR0B = (0<<WGM02) | (0<<CS02) | (1<<CS01) | (0<<CS00); //clk/8
	//OCR0A = 30-1; //9.6MHz/40kHz = 240 = 8 * 30
	OCR0A = 15-1; //9.6MHz/40kHz = 240 = 8 * 30
	TIMSK0 = (1<<OCIE0A);
	sei();
}

void loop(void) {
}

int main(void)
{
	init();

	while (1) {
		loop();
	}


	return 0;
}

ISR(PCINT0_vect) {
	if ((ir1start==0) && (ir2start==0)) {
		if (!PINV(RIGHT_BUTTON)) {
			cli();
			OCR0A = 18-1; //9.6MHz/33kHz = 8 * 36
			ir2start = 1;
			SETP(RED_LED);
			next_time = time;
			sei();
		}
		if (!PINV(LEFT_BUTTON)) {
			cli();
			OCR0A = 15-1; //9.6MHz/40kHz = 240 = 8 * 30
			ir1start = 1;
			SETP(RED_LED);
			next_time = time;
			sei();
		}
	}
}

ISR(TIM0_COMPA_vect) {
	if (ir1start != 0) {
		if (time == next_time) {
			next_time = next_time + DELTA_TIMES(current_delta_index);
			send = !(current_delta_index & 1);
			current_delta_index++;
			if (current_delta_index>=DELTA_TIMES_LENGTH) {
				current_delta_index = 0;
				ir1start = 0;
				ir2start = 0;
				CLRP(RED_LED);
			}
		}
	} else if (ir2start != 0) {
		if (time == next_time) {
			next_time = next_time + DELTA_TIMES2(current_delta_index);
			send = !(current_delta_index & 1);
			current_delta_index++;
			if (current_delta_index>=DELTA_TIMES2_LENGTH) {
				current_delta_index = 0;
				ir1start = 0;
				ir2start = 0;
				CLRP(RED_LED);
			}
		}
	}

	if (send!=0) {
		INVP(IR_LED);
	} else {
		CLRP(IR_LED);
	}
	time++;
}

