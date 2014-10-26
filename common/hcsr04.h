/*
 * hcsr04.h
 *
 *  Created on: Sep 27, 2014
 *      Author: lim
 */

#ifndef HCSR04_H_
#define HCSR04_H_

#include "ports.h"

#ifndef HCSR04_TRIG
#define HCSR04_TRIG PORTA_D2
#endif
#ifndef HCSR04_ECHO
#define HCSR04_ECHO PORTA_D10
#endif

void hcsr04_init(void);
uint16_t hcsr04_getDistance();

#endif /* HCSR04_H_ */
