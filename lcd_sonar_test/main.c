#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "ports.h"
#include "pcd8544.h"
#include "hcsr04.h"
#include "timer1.h"
#include "usart.h"

#include "i2c.h"

#define SCL_CLOCK 100000L
//#define DS1307_I2C_ADDRESS 0x68


void init(void) {
	SETD(TXLED);
	SETD(RXLED);

#ifdef __AVR_ATmega32U4__
	USBCON = (0<<USBE)|(1<<FRZCLK)|(0<<OTGPADE)|(0<<VBUSTE);
#endif
	timer1_init();
	hcsr04_init();
	lcd_init();
	usart_init();
	i2c_init();
	sei();
}

void lcd_printhex(uint8_t c)
{
	char lo = c & 0x0F;
	char hi = (c & 0xF0) >> 4;
	lcd_putchar(hi + ((hi<10)?'0':'A'-10));
	lcd_putchar(lo + ((lo<10)?'0':'A'-10));
}

void lcd_printdec(uint16_t pc) {
	uint16_t c = pc;
	char n[] = "              ";
	uint8_t i = 4;
	n[i--] = (c % 10) + '0';
	c = c / 10;
	while (c>0) {
		n[i--] = (c % 10) + '0';
		c = c / 10;
	}
	lcd_putstr(n);
}

int main(void)
{
	init();

//	lcd_overlay(1);
	lcd_optimize(0);
	lcd_rotate(1);
	lcd_clear(0x00);

	lcd_putstr("..............");
	/*
	for (uint16_t j = 0; j < 16384; j++) {
		//scan i2c dev
		lcd_textpos(0,0);
		for (uint8_t i = 1; i < 127; i++) {
			uint8_t res1 = i2c_start(i << 1);
			uint8_t res2 = 0;//i2c_write(0);
			if (res1 == 0) {
				res2 = i2c_write(0);
			}

			i2c_stop();
			lcd_printhex(i);
			lcd_putstr("  ");
			lcd_printhex(res1);
			lcd_printhex(res2);
			lcd_putstr("      ");
		}
	}
*/
	/*
	uint8_t res = 0;
	res = i2c_start(0x68 << 1);
	lcd_putstr("#");
	lcd_printhex(res);
	if (res == 0) {
		res = i2c_write(0);

		res = i2c_write(0x00);
		res = i2c_write(0x53);
		res = i2c_write(0x02);
		res = i2c_write(3);
		res = i2c_write(0x22);
		res = i2c_write(0x10);
		res = i2c_write(0x14);
		res = i2c_write(0b10010011);
	}
	i2c_stop();
	*/

	for (uint16_t j = 0; j < 16384; j++) {
		lcd_textpos(0,0);
		lcd_printhex(j);
		lcd_putstr("#");

		uint8_t res = 0;
		res = i2c_start(0x68 << 1);
		lcd_putstr("#");
		lcd_printhex(res);
		if (res == 0) {
			res = i2c_write(0);
			lcd_printhex(res);
			res = i2c_start((0x68 << 1) | 0x01); //repeated start
			lcd_printhex(res);
			lcd_putstr("#   ");

			for (uint8_t i = 0; i < 35; i++) {
				res = i2c_readAck();
				lcd_printhex(res);
			}
			res = i2c_readNak();
			lcd_printhex(res);
		}
		i2c_stop();

	}



//	uint8_t x = 0;
	uint32_t avg_dist = 0;
	while (1) {
		lcd_textpos(0,0);
		uint16_t dist = hcsr04_getDistance();
		avg_dist = (dist + 7*avg_dist)>>3;
//		uint16_t div_factor = 116;
		uint16_t dist_cm = avg_dist/116;
		uint16_t vol_l = (((uint32_t)dist)*10)/231;
//		div_factor = 231;
//		vol_l = vol_l/div_factor;
		lcd_printdec(vol_l);
		lcd_printdec(dist_cm);
		lcd_printdec(avg_dist);
		lcd_printdec(dist);
		usart_printhex(dist);
//		uint8_t dst = (dist>>7)%LCD_HEIGHT;
//		lcd_overlay(0);
//		lcd_line(x, LCD_HEIGHT-1, x, 0);
//		lcd_overlay(1);
//		lcd_line(x, LCD_HEIGHT-1, x, 0);
//		lcd_overlay(0);
//		lcd_line(x, LCD_HEIGHT-1, x, dst);
//		lcd_send_buffer();
//
//		x++;
//		if (x>LCD_WIDTH) {
//			x = 0;
//		}
	}


	return 0;
}

#ifdef __AVR_ATmega32U4__
ISR(USB_COM_vect) {
}

ISR(USB_GEN_vect) {
}
#endif
