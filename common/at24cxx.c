/*
 * at24cxx.c
 *
 *  Created on: Jul 12, 2015
 *      Author: lim
 */

#include "at24cxx.h"
#include "i2c.h"

#define AT24CXX_I2C_ADDRESS 0x50

uint8_t at24cxx_readbyte(uint8_t addr) {
	uint8_t res = 0;
	uint8_t result = 0;
	res = i2c_start(AT24CXX_I2C_ADDRESS << 1);
	if (res == 0) {
		res |= i2c_write(addr);
		res |= i2c_start((AT24CXX_I2C_ADDRESS << 1) | 0x01);
		result = i2c_readNak();
		i2c_stop();
	}
	return result;
}

uint8_t at24cxx_writebyte(uint8_t addr, uint8_t byte) {
	uint8_t res = 0;
	res = i2c_start(AT24CXX_I2C_ADDRESS << 1);
	if (res == 0) {
		res |= i2c_write(addr);
		res |= i2c_write(byte);
		i2c_stop();
	}
	return res;
}