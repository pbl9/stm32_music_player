/*
 * callbacks.c
 *
 *  Created on: Mar 24, 2019
 *      Author: pawel
 */


#include "main.h"

extern uint8_t half_xfer_complete;
extern uint8_t xfer_complete;

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
	half_xfer_complete=1;
	xfer_complete=0;
}
void HAL_DAC_ConvCpltCallbackCh1 (DAC_HandleTypeDef *hdac)
{
	half_xfer_complete=0;
	xfer_complete=1;
}
