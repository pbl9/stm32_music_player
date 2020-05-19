/*
 * conf.c
 *
 *  Created on: Feb 19, 2019
 *      Author: pawel
 */

#include "conf.h"

void Error_Handler(void)
{

  while(1){
  BSP_LED_Off(LED2);
  HAL_Delay(500);
  BSP_LED_On(LED2);
  }
}

void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* MSI is enabled after System reset, activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;//RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;//RCC_MSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;//RCC_MSIRANGE_6;
  //RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

}
void GPIO_Conf()
{
	GPIO_InitTypeDef gpio;
	//UART Pins
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();

	      /**USART2 GPIO Configuration
	      PA2     ------> USART2_TX
	      PA3     ------> USART2_RX
	      */
	 gpio.Pin = GPIO_PIN_2|GPIO_PIN_3;
	 gpio.Mode = GPIO_MODE_AF_PP;
	 gpio.Pull = GPIO_PULLUP;
	 gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	 gpio.Alternate = GPIO_AF7_USART2;
	 HAL_GPIO_Init(GPIOA, &gpio);
	 //Config pins for dac channels
	 gpio.Pin=GPIO_PIN_4|GPIO_PIN_5;
	 gpio.Pull=GPIO_NOPULL;
	 gpio.Speed=GPIO_SPEED_FREQ_HIGH;
	 gpio.Mode=GPIO_MODE_ANALOG;
	 HAL_GPIO_Init(GPIOA,&gpio);
	 //push button
	 gpio.Mode = GPIO_MODE_IT_RISING;
	 gpio.Pull = GPIO_NOPULL;
	 gpio.Pin = GPIO_PIN_13;
	 HAL_GPIO_Init(GPIOC, &gpio);


}
void UART_Conf()
{
	  huart2.Instance = USART2;
	  huart2.Init.BaudRate = 115200;
	  huart2.Init.WordLength = UART_WORDLENGTH_8B;
	  huart2.Init.StopBits = UART_STOPBITS_1;
	  huart2.Init.Parity = UART_PARITY_NONE;
	  huart2.Init.Mode = UART_MODE_TX_RX;
	  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	  HAL_UART_Init(&huart2);
}
void send_message(char* m,uint16_t size)
{
	HAL_UART_Transmit(&huart2,(uint8_t*)m,size,1000);
}
void DAC_Conf()
{
	  DAC_ChannelConfTypeDef sConfig;
	//DMA_HandleTypeDef dac_dma;
	  __HAL_RCC_DAC1_CLK_ENABLE();
	//__HAL_RCC_DMA1_CLK_ENABLE();

	  hdac1.Instance = DAC1;
	  if (HAL_DAC_Init(&hdac1) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /**DAC channel OUT1 config
	  */
	  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
	  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
	  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
	  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
	  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
	  {
	    Error_Handler();
	  }


}
void HAL_DAC_MspInit(DAC_HandleTypeDef* dacHandle)
{

  if(dacHandle->Instance==DAC1)
  {
    /* DAC1 clock enable */
    //__HAL_RCC_DAC1_CLK_ENABLE();


    /* DAC1 DMA Init */
    /* DAC_CH1 Init */
    hdma_dac_ch1.Instance = DMA1_Channel3;
    hdma_dac_ch1.Init.Request = DMA_REQUEST_6;
    hdma_dac_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac_ch1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_dac_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_dac_ch1.Init.Mode = DMA_CIRCULAR;
    hdma_dac_ch1.Init.Priority = DMA_PRIORITY_MEDIUM;
    if (HAL_DMA_Init(&hdma_dac_ch1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(dacHandle,DMA_Handle1,hdma_dac_ch1);

  /* USER CODE BEGIN DAC1_MspInit 1 */

  /* USER CODE END DAC1_MspInit 1 */
  }
}
void DMA_Init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}
void Sample_Clock_Init()
{
	TIM_MasterConfigTypeDef sMasterConfig;
	__HAL_RCC_TIM6_CLK_ENABLE();
	tim.Instance=TIM6;
	tim.Init.CounterMode=TIM_COUNTERMODE_UP;
	tim.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	tim.Init.Prescaler=0;
	tim.Init.Period=1814;
	tim.Init.AutoReloadPreload=TIM_AUTORELOAD_PRELOAD_DISABLE;
	tim.Init.RepetitionCounter=0;
	HAL_TIM_Base_Init(&tim);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	//HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_3);
	//HAL_NVIC_SetPriority(TIM3_IRQn,3,0);
	//HAL_NVIC_EnableIRQ(TIM3_IRQn);
	HAL_TIMEx_MasterConfigSynchronization(&tim, &sMasterConfig);
	//HAL_TIM_Base_Start_IT(&tim);

}
