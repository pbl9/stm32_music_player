/*
 * callbacks.h
 *
 *  Created on: Mar 24, 2019
 *      Author: pawel
 */

#ifndef CALLBACKS_H_
#define CALLBACKS_H_

#include "conf.h"
#include "main.h"

extern uint16_t data_ch1[5],data_ch2[5];
extern DAC_HandleTypeDef hdac1;
extern volatile uint16_t dac_pointer;

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif /* CALLBACKS_H_ */
