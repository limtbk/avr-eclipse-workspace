/*
 * pcd8544.c
 *
 *  Created on: Sep 26, 2014
 *      Author: lim
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "pcd8544.h"
#include "timer1.h"

const uint8_t PROGMEM font5x7 [][CHAR_WIDTH - 1] = {
	    { 0x00, 0x00, 0x00, 0x00, 0x00 },   //
	    { 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
	    { 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
	    { 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
	    { 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
		{ 0x32, 0x34, 0x08, 0x16, 0x26 },   // %
	    { 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
	    { 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
	    { 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
	    { 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
	    { 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
	    { 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
	    { 0x00, 0x00, 0x50, 0x30, 0x00 },   // ,
	    { 0x10, 0x10, 0x10, 0x10, 0x10 },   // -
	    { 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
	    { 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
	    { 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
	    { 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
	    { 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
	    { 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
	    { 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
	    { 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
	    { 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
	    { 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
	    { 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
	    { 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
	    { 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
	    { 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
	    { 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
	    { 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
	    { 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
	    { 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
	    { 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
	    { 0x7E, 0x11, 0x11, 0x11, 0x7E },   // A
	    { 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
	    { 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
	    { 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
	    { 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
	    { 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
	    { 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
	    { 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
	    { 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
	    { 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
	    { 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
	    { 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
	    { 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
	    { 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
	    { 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
	    { 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
	    { 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
	    { 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
	    { 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
	    { 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
	    { 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
	    { 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
	    { 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
	    { 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
	    { 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
	    { 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
	    { 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [
	    { 0x02, 0x04, 0x08, 0x10, 0x20 },   // backslash
	    { 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]
	    { 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^
	    { 0x40, 0x40, 0x40, 0x40, 0x40 },   // _
	    { 0x00, 0x01, 0x02, 0x04, 0x00 },   // '
	    { 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
	    { 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
	    { 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
	    { 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
	    { 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
	    { 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
	    { 0x0C, 0x52, 0x52, 0x52, 0x3E },   // g
	    { 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
	    { 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
	    { 0x20, 0x40, 0x44, 0x3D, 0x00 },   // j
	    { 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
	    { 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
	    { 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
	    { 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
	    { 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
	    { 0x7C, 0x14, 0x14, 0x14, 0x08 },   // p
	    { 0x08, 0x14, 0x14, 0x18, 0x7C },   // q
	    { 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
	    { 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
	    { 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
	    { 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
	    { 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
	    { 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
	    { 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
	    { 0x0C, 0x50, 0x50, 0x50, 0x3C },   // y
	    { 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z
	    { 0x00, 0x7F, 0x3E, 0x1C, 0x08 },   // > Filled   // {
		{ 0x08, 0x1C, 0x3E, 0x7F, 0x00 }, 	// < Filled   // |
		{ 0x08, 0x7C, 0x7E, 0x7C, 0x08 },   // Arrow up   // }
		{ 0x10, 0x3E, 0x7E, 0x3E, 0x10 },   // Arrow down // ~
		{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },   // Cursor // ~
};

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define ABS(x) ((x)>0?(x):-(x))

typedef struct TextBufStruct {
	uint8_t pos;
	uint8_t chg_start;
	uint8_t chg_length;
	char buf[LCD_CHARS];
} TTextBuf;

typedef struct VideoBufStruct {
	uint16_t pos;
	uint16_t chg_start;
	uint16_t chg_length;
	uint8_t buf[LCD_BYTES];
} TVideoBuf;

TVideoBuf video_buf;
TTextBuf text_buf;

uint8_t overlay;
uint8_t rotate;
uint8_t optimize_chg;

void lcd_send_data(uint8_t data, uint8_t isData, uint8_t isReverse)
{
	CLRP(LCD_CE);
	if (isData) {
		SETP(LCD_DC); //1 for data
	} else {
		CLRP(LCD_DC); //0 for command
	}

	for (uint8_t i=0; i<CHAR_HEIGHT; i++) {
		if ((data>>(isReverse?i:CHAR_HEIGHT-1-i)) & 0x01) {
			SETP(LCD_DIN);
		} else {
			CLRP(LCD_DIN);
		}

		CLRP(LCD_CLK);
		SETP(LCD_CLK);
	}
	SETP(LCD_CE);
}

void lcd_base_addr(uint16_t addr) {
	lcd_send_data(0x80 |(addr % LCD_WIDTH),0,0);
	lcd_send_data(0x40 |(addr / LCD_WIDTH),0,0);
}

void lcd_send_buffer(void)
{
	uint16_t start = optimize_chg?video_buf.chg_start:0;
	uint16_t end = optimize_chg?(video_buf.chg_start+video_buf.chg_length):(start + LCD_BYTES);

	lcd_base_addr(start);
    for (uint16_t i=start; i<end; i++) {
    	if (rotate) {
    		lcd_send_data(video_buf.buf[LCD_BYTES-1-i],1,1);
    	} else {
    		lcd_send_data(video_buf.buf[i],1,0);
    	}
    }
    video_buf.chg_length = 0;
}

void lcd_send_text_buffer(void)
{
	video_buf.pos = 0;
    for (uint8_t i=0; i<LCD_CHARS; i++) {
    	char out_char = text_buf.buf[i];
    	out_char = out_char>31?(out_char<128?out_char:32):32;
    	for (uint8_t j=0; j<CHAR_WIDTH-1; j++) {
    		if (overlay) {
    			video_buf.buf[video_buf.pos] ^= pgm_read_byte(&font5x7[out_char-32][j]) << 1;
    		} else {
    			video_buf.buf[video_buf.pos] = pgm_read_byte(&font5x7[out_char-32][j]) << 1;
    		}
    		video_buf.pos++;
    	}
		if (!overlay) {
			video_buf.buf[video_buf.pos] = 0;
		}
		video_buf.pos++;
    }
    video_buf.chg_start = 0;
    video_buf.chg_length = LCD_BYTES;
    lcd_send_buffer();
}

void lcd_clear(uint8_t pattern)
{
    for (uint16_t i=0; i<LCD_BYTES; i++) {
    	video_buf.buf[i] = pattern;
    }
    video_buf.chg_start = 0;
    video_buf.chg_length = LCD_BYTES;
    lcd_send_buffer();
}

void lcd_init(void)
{
	SETD(LCD_RST);
	SETD(LCD_CE);
	SETD(LCD_DC);
	SETD(LCD_DIN);
	SETD(LCD_CLK);
	SETD(LCD_LED);

	SETP(LCD_RST);
	timer1_delay_us2(30000); //delay 15ms
	CLRP(LCD_RST);
	timer1_delay_us2(32000); //delay 64ms
	timer1_delay_us2(32000);
	timer1_delay_us2(32000);
	timer1_delay_us2(32000);
	SETP(LCD_RST);
	SETP(LCD_CE);

	lcd_send_data(0x21,0,0);  // LCD Extended Commands
	lcd_send_data(0xC8,0,0);  // Set LCD Vop(Contrast)
	lcd_send_data(0x06,0,0);  // Set Temp coefficent
	lcd_send_data(0x13,0,0);  // LCD bias mode 1:48
	lcd_send_data(0x20,0,0);  // Standard Commands, Horizontal addressing
	lcd_send_data(0x0C,0,0);  // LCD in normal mode

	int contrast = 0x40;
	lcd_send_data(0x21,0,0);				// LCD Extended Commands
	lcd_send_data(0x80 | contrast,0,0);		// Set LCD Vop(Contrast)
	lcd_send_data(0x20,0,0);				// LCD std cmds, hori addr mode

    lcd_clear(0x00);
    overlay = 0;
    rotate = 0;
    SETP(LCD_LED);
}

void lcd_putchar(char ch) {
	while (text_buf.pos>=LCD_CHARS) {
		text_buf.pos -= LCD_CHARS;
	}
	text_buf.buf[text_buf.pos] = ch;
	text_buf.pos++;
}

void lcd_textpos(uint8_t x, uint8_t y) {
	if ((x<LCD_WIDTH) && (y<LCD_HEIGHT)) {
		text_buf.pos = x + y*LCD_WIDTH;
	}
}

void lcd_putstr(char *str)
{
    while (*str) {
    	lcd_putchar(*str++);
    }
    lcd_send_text_buffer();
}

void lcd_overlay(uint8_t poverlay) {
	overlay = poverlay;
}

void lcd_rotate(uint8_t protate) {
	rotate = protate;
}

void lcd_optimize(uint8_t poptimize) {
	optimize_chg = poptimize;
}

void lcd_point(uint8_t x, uint8_t y) {
	if ((x<LCD_WIDTH) && (y<LCD_HEIGHT)) {
		uint16_t point_address = x + (y >> 3)*LCD_WIDTH;
		uint8_t point_bit = y & 7;
		uint8_t bit = 1 << point_bit;
		if (overlay) {
			video_buf.buf[point_address] ^= bit;
		} else {
			video_buf.buf[point_address] |= bit;
		}
		if (video_buf.chg_length) {
			video_buf.chg_length = MAX(video_buf.chg_length, ABS(video_buf.chg_start - point_address + 1));
			video_buf.chg_start = MIN(video_buf.chg_start, point_address);
		} else {
			video_buf.chg_length = 1;
			video_buf.chg_start = point_address;
		}
	}
}

void lcd_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
	int8_t dx = ABS(x1-x0), sx = x0<x1 ? 1 : -1;
	int8_t dy = ABS(y1-y0), sy = y0<y1 ? 1 : -1;
	int8_t err = (dx>dy ? dx : -dy) >> 1, e2;
	for(;;) {
		lcd_point(x0, y0);
		if (x0==x1 && y0==y1) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}
}

void lcd_cliplot(uint8_t cx, uint8_t cy, uint8_t x, uint8_t y) {
	lcd_point(cx+x, cy+y);
	lcd_point(cx-x, cy+y);
	lcd_point(cx+x, cy-y);
	lcd_point(cx-x, cy-y);
	lcd_point(cx+y, cy+x);
	lcd_point(cx-y, cy+x);
	lcd_point(cx+y, cy-x);
	lcd_point(cx-y, cy-x);
}

void lcd_circle(uint8_t x0, uint8_t y0, uint8_t r) {
	int8_t x=0, y=r, p=1-r;
	lcd_cliplot(x0,y0,x,y);
	while(x<y) {
		x++;
		if(p<0) {
			p+=(x<<1)+1;
		} else {
			y--;
			p+=((x-y)<<1)+1;
		}
		lcd_cliplot(x0,y0,x,y);
	}
}
