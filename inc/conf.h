/*
 * conf.h
 *
 *  Created on: Feb 19, 2019
 *      Author: pawel
 */

#ifndef CONF_H_
#define CONF_H_

#include "stm32l4xx.h"
#include "stm32l4xx_nucleo.h"

UART_HandleTypeDef huart2;
DAC_HandleTypeDef hdac1;
TIM_HandleTypeDef tim;
DMA_HandleTypeDef hdma_dac_ch1;

void GPIO_Conf();
void SystemClock_Config(void);
void UART_Conf();
void send_message(char* m,uint16_t size);
void DAC_Conf();
void Sample_Clock_Init();
void DMA_Init(void);
void Error_Handler(void);


#endif /* CONF_H_ */
