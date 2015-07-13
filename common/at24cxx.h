/*
 * at24cxx.h
 *
 *  Created on: Jul 12, 2015
 *      Author: lim
 */

#ifndef AT24CXX_H_
#define AT24CXX_H_

#include <avr/io.h>

uint8_t at24cxx_readbyte(uint8_t addr);
uint8_t at24cxx_writebyte(uint8_t addr, uint8_t byte);

#endif /* AT24CXX_H_ */
