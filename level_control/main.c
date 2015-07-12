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
	uint16_t onLevel;
	uint16_t offLevel;
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

void lcd_printsdec(int16_t pc) {
	int16_t c = pc;
	char n[] = "              ";
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

int16_t timeToCm(int16_t time) {
	return time/116;
}

int16_t timeToLiter(int16_t time) {
	return (((int32_t)time)*10)/231;
}

int main(void)
{
	init();

	uint16_t offlvl = 0;
	uint16_t onlvl = 0;

	uint8_t pump = 0;

//	lcd_overlay(1);
	lcd_optimize(0);
	lcd_rotate(1);
	lcd_clear(0x00);

	lcd_putstr("..............");
	lcd_putstr("..............");
	lcd_putstr("..............");
	lcd_putstr("..............");
	lcd_putstr("..............");
	lcd_putstr("..............");

//	TTime time;
	uint32_t avg_dist = 0;
	while (1) {
		lcd_textpos(0,0);
//		ds13xx_gettime(&time, 1);
		uint16_t dist = hcsr04_getDistance();
		avg_dist = (dist + 7*avg_dist)>>3;


		uint16_t dist_cm = avg_dist/116;
		uint16_t vol_l = (((uint32_t)avg_dist)*10)/231;

		int16_t lvl = offlvl - vol_l;
		int16_t lvl_on = offlvl - onlvl;
//		lcd_printsdec(lvl);
//		lcd_printsdec(lvl_on);
//		lcd_printsdec(dist_cm);

		lcd_printsdec(lvl);
		lcd_printsdec(lvl_on);
		lcd_printsdec(dist_cm);

		lcd_putstr("              ");
		usart_printhex(dist);

		lcd_putchar(' ');
		if (!PINV(OFF_LVL_BTN)) {
			offlvl = vol_l;
			if (avg_dist != settings.offLevel) {
				settings.offLevel = avg_dist;
				saveSettings(&settings);
			}
			lcd_putchar('+');
		} else {
			lcd_putchar('-');
		}

		if (!PINV(ON_LVL_BTN)) {
			onlvl = vol_l;
			if (avg_dist != settings.onLevel) {
				settings.onLevel = avg_dist;
				saveSettings(&settings);
			}
			lcd_putchar('+');
		} else {
			lcd_putchar('-');
		}

		if (offlvl>onlvl) {
			if (pump) {
				if (offlvl<vol_l) {
					pump = 0;
				}
			} else {
				if (onlvl>vol_l) {
					pump = 1;
				}
			}
			if (pump) {
				lcd_putstr("            PUMP ON       ");
				SETP(PUMP_CTRL);
			} else {
				lcd_putstr("            PUMP OFF      ");
				CLRP(PUMP_CTRL);
			}
		} else {
			lcd_putstr("            CONFIG ERROR  ");
		}

	}

	return 0;
}

#ifdef __AVR_ATmega32U4__
ISR(USB_COM_vect) {
}

ISR(USB_GEN_vect) {
}
#endif
