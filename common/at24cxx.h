/*
 * at24cxx.h
 *
 *  Created on: Jul 12, 2015
 *      Author: lim
 */

#ifndef AT24CXX_H_
#define AT24CXX_H_

#include <avr/io.h>

uint8_t at24cxx_readbyte(uint16_t addr);
uint8_t at24cxx_writebyte(uint16_t addr, uint8_t byte);
uint8_t at24cxx_read_block (uint16_t addr, void *dst, uint8_t size);
uint8_t at24cxx_write_block (uint16_t addr, const void *src, uint8_t size);

#endif /* AT24CXX_H_ */
