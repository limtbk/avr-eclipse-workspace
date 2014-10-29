#include <avr/io.h>
#include <avr/interrupt.h>
#include "ports.h"
#include "pcd8544.h"
#include "timer1.h"
#include "i2c.h"
#include "ds13xx.h"

void init(void) {
	DDRC = 0;
	PORTC = 0;
	DDRD = 0;
	PORTD = 0;
	timer1_init();
	lcd_init();
	//usart_init();


	/*
	PC6&ADC5 - PC0&ADC0, ADC7&PD3, ADC6&PD4

	PCINT20

	PCINT14 - PCINT8


	 */
	PCICR = (1<<PCIE2) | (1<<PCIE1);
	PCMSK2 = (1<<PCINT19) | (1<<PCINT20);
	PCMSK1 = (1<<PCINT13) | (1<<PCINT12) | (1<<PCINT11) | (1<<PCINT10) | (1<<PCINT9) | (1<<PCINT8);

//	PCICR = (1<<PCIE1) | (1<<PCIE2);

//	PCMSK1 = 0xFF;
//	PCMSK2 = 0xFF;

	sei();
}

uint8_t lastreg;
uint8_t count[] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t change = 0;

ISR(PCINT1_vect) {
	INVP(LCD_LED);
	uint8_t newreg = (PINC & 0b00111111) | ((PIND & ((1<<PD4) | (1<<PD3)))<<(7-PD4));
	uint8_t changes = (lastreg ^ newreg) & newreg; //detect 0 -> 1
	if (changes != 0) {
		lastreg = newreg;
		for (uint8_t i = 0; i < 8; i++) {
			count[i] += (changes & 1);
			change += (changes & 1);
			changes = changes >> 1;
		}
	}
}

ISR(PCINT2_vect) {
	INVP(LCD_LED);
	uint8_t newreg = (PINC & 0b00111111) | ((PIND & ((1<<PD4) | (1<<PD3)))<<(7-PD4));
	uint8_t changes = (lastreg ^ newreg) & newreg; //detect 0 -> 1
	if (changes != 0) {
		lastreg = newreg;
		for (uint8_t i = 0; i < 8; i++) {
			count[i] += (changes & 1);
			change += (changes & 1);
			changes = changes >> 1;
		}
	}
}

void show_connection_bk(void) {
	lcd_putstr("12345678  HEAD");
	lcd_putstr("              ");
	lcd_putstr("              ");
	lcd_putstr("              ");
	lcd_putstr("              ");
	lcd_putstr("12345678  TAIL");
//	lcd_line(CHAR_WIDTH*8, 0, CHAR_WIDTH*8, LCD_HEIGHT-1);
	lcd_send_buffer();
}

void show_connections(uint8_t *connections) {
	for (uint8_t i = 0; i < 8; i++) {
		uint8_t c = (connections[i]);
		if (c<8) {
			lcd_line((i)*CHAR_WIDTH+CHAR_WIDTH/2-1, 1*CHAR_HEIGHT+1, (c)*CHAR_WIDTH+CHAR_WIDTH/2-1, 5*CHAR_HEIGHT-1);
		}
	}
}

void calc_connections(uint8_t *connections) {
	for (uint8_t i = 0; i < 8; i++) {
		connections[i] = 8;
		if (count[i]>0) {
			connections[i] = count[i];
		}
	}
}

void reset_changes() {
	for (uint8_t i = 0; i < 8; i++) {
		count[i] = 0;
	}
}

int main(void)
{
	init();

	uint8_t con_arr[] = {2, 5, 0, 6, 7, 1, 3, 4};

	lcd_overlay(0);
	lcd_optimize(0);
	lcd_rotate(1);

	while (1) {
		//lcd_clear(0x00);
		show_connection_bk();
		show_connections(con_arr);
		lcd_send_buffer();
		change = 0;
		reset_changes();
		timer1_delay_ms(100);
		calc_connections(con_arr);
	}


	return 0;
}
