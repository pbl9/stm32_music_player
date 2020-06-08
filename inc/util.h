/*
 * util.h
 *
 *  Created on: May 22, 2020
 *      Author: pawel
 */

#ifndef UTIL_H_
#define UTIL_H_

#include "main.h"

void send_error(char* buffer,const char* file,uint16_t line,void (*sender)(char*,uint16_t));
void fill_zeros(uint8_t* buffer,uint16_t begin,uint16_t end);
#endif /* UTIL_H_ */
