#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "ports.h"
#include "pcd8544.h"
#include "hcsr04.h"
#include "timer1.h"
#include "usart.h"
#include "i2c.h"
#include "ds13xx.h"

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

//	ds13xx_settime(ttime(0x01,0x36,0x00),1);
//	ds13xx_setdate(tdate(0x07,0x05,0x07,0x15),1);

//	uint8_t x = 0;
	TTime time;
	TDate date;
//	TTemperature temperature;
//	uint32_t avg_dist = 0;
	while (1) {
		lcd_textpos(0,0);
		ds13xx_gettime(&time, 1);
		ds13xx_getdate(&date, 1);
//		ds13xx_gettemperature(&temperature, 1);
		uint8_t temp = bintodec(ds13xx_readbyte(0x11));
//		uint16_t dist = hcsr04_getDistance();
//		avg_dist = (dist + 7*avg_dist)>>3;
//		uint16_t dist_cm = avg_dist/116;
//		uint16_t vol_l = (((uint32_t)dist)*10)/231;
//		lcd_printdec(vol_l);
//		lcd_printdec(dist_cm);
		lcd_printhex(time.hour);
		lcd_putchar(':');
		lcd_printhex(time.min);
		lcd_putchar(':');
		lcd_printhex(time.sec);
		lcd_putstr(" ");
		lcd_printhex(temp);
		lcd_putstr("C  ");

		lcd_printhex(date.day);
		lcd_putchar('-');
		lcd_printhex(date.month);
		lcd_putstr("-20");
		lcd_printhex(date.year);
		lcd_putchar(' ');
		switch (date.weekday) {
			case 1: {
				lcd_putstr("MON");
				break;
			}
			case 2: {
				lcd_putstr("TUE");
				break;
			}
			case 3: {
				lcd_putstr("WED");
				break;
			}
			case 4: {
				lcd_putstr("THU");
				break;
			}
			case 5: {
				lcd_putstr("FRI");
				break;
			}
			case 6: {
				lcd_putstr("SAT");
				break;
			}
			case 7: {
				lcd_putstr("SUN");
				break;
			}
			default:
				break;
		}

		for (uint8_t i = 0; i < 28; i++) {
			uint8_t b = ds13xx_readbyte(i);
			lcd_printhex(b);
		}


//		usart_printhex(dist);
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
