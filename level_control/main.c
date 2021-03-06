#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "ports.h"
#include "pcd8544.h"
#include "hcsr04.h"
#include "timer1.h"
#include "usart.h"
#include "i2c.h"
#include "ds13xx.h"
#include "at24cxx.h"
#include "macro.h"

/*
 * Assume, that in current configuration there are following connections:
 * rx & tx to UART-USB
 * LCD_RST PORTA_D8
 * LCD_CE  PORTA_D7
 * LCD_DC  PORTA_D6
 * LCD_DIN PORTA_D5
 * LCD_CLK PORTA_D4
 * LCD_LED PORTA_D3
 * HCSR04_TRIG PORTA_D2
 * HCSR04_ECHO PORTA_D10
 * SCL PORTA_A5
 * SDA PORTA_A4
 * ON_LVL_BTN PORTA_D12
 * OFF_LVL_BTN PORTA_D11
 * PUMP_CTRL PORTA_D9
 */

#define OFF_LVL_BTN PORTA_D11
#define ON_LVL_BTN PORTA_D12
#define PUMP_CTRL PORTA_D9

typedef struct SettingsStruct {
	int16_t lowestLevel;
	int16_t onLevel;
	int16_t maxLevel;
} TSettings;

TSettings settings;

void loadSettings(TSettings *settings) {
	eeprom_read_block(settings, (const void *)0x00, sizeof(TSettings));
}

void saveSettings(TSettings *settings) {
	eeprom_write_block(settings, (void *)0x00, sizeof(TSettings));
}

void init(void) {
	SETD(TXLED);
	SETD(RXLED);

	CLRD(OFF_LVL_BTN);
	SETP(OFF_LVL_BTN);
	CLRD(ON_LVL_BTN);
	SETP(ON_LVL_BTN);
	SETD(PUMP_CTRL);
	CLRP(PUMP_CTRL);

#ifdef __AVR_ATmega32U4__
	USBCON = (0<<USBE)|(1<<FRZCLK)|(0<<OTGPADE)|(0<<VBUSTE);
#endif
	timer1_init();
	hcsr04_init();
	lcd_init();
	usart_init();
	i2c_init();
//	twiInit();
	sei();
	loadSettings(&settings);
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
	char n[] = "       ";
	uint8_t i = 5;
	n[i--] = (c % 10) + '0';
	c = c / 10;
	while (c>0) {
		n[i--] = (c % 10) + '0';
		c = c / 10;
	}
	lcd_putstr(n);
}

void lcd_printsdec(int16_t pc) {
	int16_t c = pc;
	char n[] = "       ";
	if (c<0) {
		c = -c;
	}
	uint8_t i = 5;
	n[i--] = (c % 10) + '0';
	c = c / 10;
	while (c>0) {
		n[i--] = (c % 10) + '0';
		c = c / 10;
	}
	if (pc<0) {
		n[i--] = '-';
	}
	lcd_putstr(n);
}

int16_t us2ToCm(int16_t time) {
	return time/116;
}

int16_t us2ToLiter(int16_t time) {
	return (((int32_t)time)*10)/231;
}

int main(void)
{
	init();

	uint8_t pump = 0;

//	lcd_overlay(1);
	lcd_optimize(0);
	lcd_rotate(1);
	lcd_clear(0xFF);

	lcd_putstr("..............");
	lcd_putstr("..............");
	lcd_putstr("..............");
	lcd_putstr("..............");
	lcd_putstr("..............");
	lcd_putstr("..............");

	lcd_clear(0x00);
	lcd_autosend_buffer(0);

	TTime time;
	TDate date;
//	TAt24cHeaderBlock headerBlock;
	uint16_t lastWrittenAddr = 0;
	uint32_t avg_dist = 0;
	while (1) {
		ds13xx_gettime(&time, 1);
		ds13xx_getdate(&date, 1);
		uint8_t temp = bintodec(ds13xx_readbyte(0x11));

		uint16_t dist = hcsr04_getDistance();
		avg_dist = (dist + 7*avg_dist)>>3;

		int16_t lvl = settings.lowestLevel - avg_dist;
		int16_t lvl_on = settings.onLevel;

		lcd_textpos(0,0);
		lcd_printsdec(us2ToLiter(lvl));
		lcd_textpos(7,0);
		lcd_printsdec(us2ToLiter(lvl_on));
		lcd_textpos(0,1);
		lcd_printsdec(avg_dist);
		lcd_textpos(7,1);
		lcd_printsdec(us2ToCm(avg_dist));

		lcd_textpos(0,4);
		lcd_printhex(time.hour);
		lcd_putchar(':');
		lcd_printhex(time.min);
		lcd_putchar(':');
		lcd_printhex(time.sec);
		lcd_putstr(" ");
		lcd_printhex(temp);
		lcd_putstr("C  ");

//		lcd_putstr("              ");
//		usart_printhex(dist);

		if (!PINV(OFF_LVL_BTN)) {
//			offlvl = vol_l;
			if (avg_dist != settings.lowestLevel) {
				settings.lowestLevel = avg_dist;
				settings.maxLevel = 0;
				saveSettings(&settings);
			}
			lcd_textpos(5,2);
			lcd_putchar('+');
		} else {
			lcd_textpos(5,2);
			lcd_putchar('-');
		}

		if (!PINV(ON_LVL_BTN)) {
			if (lvl>0) {
				if (lvl != settings.onLevel) {
					settings.onLevel = lvl;
					settings.maxLevel = lvl;
					saveSettings(&settings);
				}
			}
			lcd_textpos(12,2);
			lcd_putchar('+');
		} else {
			lcd_textpos(12,2);
			lcd_putchar('-');
		}

		if (avg_dist < 32767) {
			if (settings.maxLevel<lvl) {
				settings.maxLevel = lvl;
				saveSettings(&settings);
			}
			uint16_t addrToWrite = ((date.weekday-1)*288 + dectobin(time.hour)*12 + dectobin(time.min)/5)*2;
			if (addrToWrite!=lastWrittenAddr) {
				at24cxx_write_block(addrToWrite, &avg_dist, 2);
				lastWrittenAddr = addrToWrite;
			}

			if (settings.onLevel>0) {
				if (pump) {
					if (settings.lowestLevel<avg_dist) {
						pump = 0;
					}
				} else {
					if (settings.onLevel<lvl) {
						pump = 1;
					}
				}
				if (pump) {
					lcd_textpos(0,3);
					lcd_putstr("PUMP ON     ");
					SETP(PUMP_CTRL);
				} else {
					lcd_textpos(0,3);
					lcd_putstr("PUMP OFF    ");
					CLRP(PUMP_CTRL);
				}
			} else {
				lcd_textpos(0,3);
				lcd_putstr("CONFIG ERROR");
			}
		} else {
			lcd_textpos(0,3);
			lcd_putstr("SONAR ERROR ");

		}

		uint16_t xres = settings.maxLevel / LCD_WIDTH;
		uint8_t xon = settings.onLevel/xres;
		uint8_t xcur = lvl/xres;

		lcd_overlay(0);
		for (uint8_t y = 0; y<8; y++) {
			lcd_line(0, LCD_HEIGHT-y-1, LCD_WIDTH-1, LCD_HEIGHT-y-1);
		}
		lcd_overlay(1);
		for (uint8_t y = 1; y<7; y++) {
			lcd_line(1, LCD_HEIGHT-y-1, LCD_WIDTH-2, LCD_HEIGHT-y-1);
		}
		lcd_overlay(0);

		lcd_line(xon, LCD_HEIGHT-8, xon, LCD_HEIGHT-1);

		if (lvl>0) {
			uint8_t k = (((lvl % xres) * 5) / xres);
			for (uint8_t x = 0; x<xcur; x++) {
				if (x<xcur-1) {
					lcd_line(x, LCD_HEIGHT-7, x, LCD_HEIGHT-2);
				} else {
					lcd_line(x, LCD_HEIGHT-k-2, x, LCD_HEIGHT-2);
				}
			}
		}

		lcd_send_buffer();

	}

	return 0;
}

#ifdef __AVR_ATmega32U4__
ISR(USB_COM_vect) {
}

ISR(USB_GEN_vect) {
}
#endif
