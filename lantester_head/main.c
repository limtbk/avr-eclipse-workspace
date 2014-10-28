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
	timer1_init();
	lcd_init();
	usart_init();
	sei();
}

int main(void)
{
	init();

	uint8_t con_arr[] = {2, 5, 0, 6, 7, 1, 3, 4};

//	lcd_overlay(1);
	lcd_optimize(0);
	lcd_rotate(1);
	lcd_clear(0x00);

	lcd_putstr("12345678  HEAD");
	lcd_putstr("              ");
	lcd_putstr("         CROSS");
	lcd_putstr("          OVER");
	lcd_putstr("              ");
	lcd_putstr("12345678  TAIL");

	lcd_line(CHAR_WIDTH*8, 0, CHAR_WIDTH*8, LCD_HEIGHT-1);

	while (1) {

		for (uint8_t i = 0; i < 8; i++) {
			uint8_t c = con_arr[i];
			if (c<8) {
				lcd_line((i)*CHAR_WIDTH+CHAR_WIDTH/2-1, 1*CHAR_HEIGHT+1, (c)*CHAR_WIDTH+CHAR_WIDTH/2-1, 5*CHAR_HEIGHT-1);
			}
			lcd_send_buffer();
		}
//		lcd_textpos(0,0);
//		lcd_printhex(0);
//		usart_printhex(0);


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
