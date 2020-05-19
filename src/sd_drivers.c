/*
 * sd_drivers.c
 *
 *  Created on: Feb 19, 2019
 *      Author: pawel
 */

#include "sd_drivers.h"

SD_HandleTypeDef uSdHandle;

static void SD_Detect_MspInit(void);
static void SD_Detect_MspDeInit(void);
//static HAL_StatusTypeDef SD_DMAConfigRx(SD_HandleTypeDef *hsd);
//static HAL_StatusTypeDef SD_DMAConfigTx(SD_HandleTypeDef *hsd);

uint8_t BSP_SD_Init(void)
{
  uint8_t sd_state = MSD_OK;

  /* uSD device interface configuration */
  uSdHandle.Instance = SDMMC1;
  uSdHandle.Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
  uSdHandle.Init.ClockBypass         = SDMMC_CLOCK_BYPASS_DISABLE;
  uSdHandle.Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  uSdHandle.Init.BusWide             = SDMMC_BUS_WIDE_1B;
  uSdHandle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
  uSdHandle.Init.ClockDiv            = SDMMC_TRANSFER_CLK_DIV;

  /* Msp SD Detect pin initialization */
  SD_Detect_MspInit();

  /* Check if the SD card is plugged in the slot */
 /* if (BSP_SD_IsDetected() != SD_PRESENT)
  {
    return MSD_ERROR_SD_NOT_PRESENT;
  }*/

  /* Msp SD initialization */
  BSP_SD_MspInit(&uSdHandle, NULL);

  /* HAL SD initialization */
  if (HAL_SD_Init(&uSdHandle) != HAL_OK)
  {
    sd_state = MSD_ERROR;
  }

  /* Configure SD Bus width */
  /* SD Bus witdth - 1B already */

  return  sd_state;
}
__weak void BSP_SD_MspInit(SD_HandleTypeDef *hsd, void *Params)
{
	GPIO_InitTypeDef gpio;
	RCC_PeriphCLKInitTypeDef  RCC_PeriphClkInit;

	  /* Prevent unused argument(s) compilation warning */
	  UNUSED(Params);

	  HAL_RCCEx_GetPeriphCLKConfig(&RCC_PeriphClkInit);

	  /* Configure the SDMMC1 clock source. The clock is derived from the PLLSAI1 */
	  /* Hypothesis is that PLLSAI1 VCO input is 8Mhz */
	  RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SDMMC1;
	  RCC_PeriphClkInit.PLLSAI1.PLLSAI1N = 12;
	  RCC_PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV4;
	  RCC_PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK;
	  RCC_PeriphClkInit.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_PLLSAI1;
	  if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit) != HAL_OK)
	  {
	    while (1) {}
	  }

	  /* Enable SDMMC1 clock */
	  __HAL_RCC_SDMMC1_CLK_ENABLE();
		//SDIO Pins
	  gpio.Pin=GPIO_PIN_12;//Clock - PC12
	  gpio.Mode=GPIO_MODE_AF_PP;
	  gpio.Pull=GPIO_PULLUP;
	  gpio.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
	  gpio.Alternate=GPIO_AF12_SDMMC1;
	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  HAL_GPIO_Init(GPIOC,&gpio);
	  gpio.Pin=GPIO_PIN_2;//CMD Pin - PD2
	  gpio.Alternate=GPIO_AF12_SDMMC1;
	  __HAL_RCC_GPIOD_CLK_ENABLE();
      HAL_GPIO_Init(GPIOD,&gpio);
	  gpio.Pin=GPIO_PIN_8;//DAT0 Pin - PC8 - MISO in SPI
	  gpio.Alternate=GPIO_AF12_SDMMC1;
	  HAL_GPIO_Init(GPIOC,&gpio);
	  //interrupts
	  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_3);
	  HAL_NVIC_SetPriority(SDMMCx_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(SDMMCx_IRQn);
	  //end init
}
__weak void BSP_SD_MspDeInit(SD_HandleTypeDef *hsd, void *Params)
{
  GPIO_InitTypeDef gpioinitstruct = {0};

  /* Prevent unused argument(s) compilation warning */
  UNUSED(Params);

  /* Enable SDMMC1 clock */
  __HAL_RCC_SDMMC1_CLK_DISABLE();

  /* Enable DMA2 clocks */
  SD_DMAx_CLK_DISABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* Common GPIO configuration */
  gpioinitstruct.Mode      = GPIO_MODE_ANALOG;
  gpioinitstruct.Pull      = GPIO_NOPULL;
  gpioinitstruct.Speed     = GPIO_SPEED_FREQ_LOW;
  gpioinitstruct.Alternate = 0;

  /* GPIOC configuration */
  gpioinitstruct.Pin = GPIO_PIN_2|GPIO_PIN_8;

  HAL_GPIO_Init(GPIOC, &gpioinitstruct);

  /* GPIOD configuration */
  gpioinitstruct.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOD, &gpioinitstruct);

  /* NVIC configuration for SDMMC1 interrupts */
  HAL_NVIC_DisableIRQ(SDMMCx_IRQn);

}
uint8_t BSP_SD_DeInit(void)
{
  uint8_t sd_state = MSD_OK;

  uSdHandle.Instance = SDMMC1;
  /* HAL SD deinitialization */
  if (HAL_SD_DeInit(&uSdHandle) != HAL_OK)
  {
    sd_state = MSD_ERROR;
  }

  /* Msp SD deinitialization */
  BSP_SD_MspDeInit(&uSdHandle, NULL);

  SD_Detect_MspDeInit();

  return  sd_state;
}
/**
  * @brief  Reads block(s) from a specified address in an SD card, in polling mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read
  * @param  NumOfBlocks: Number of SD blocks to read
  * @param  Timeout: Timeout for read operation
  * @retval SD status
  */
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
  HAL_StatusTypeDef  sd_state = HAL_OK;
///!!!!!!!!!!!
  sd_state =  HAL_SD_ReadBlocks(&uSdHandle, (uint8_t *)pData, ReadAddr, NumOfBlocks, Timeout);
//!!!!!!!!!!!!
  if (sd_state == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}
/**
  * @brief  Writes block(s) to a specified address in an SD card, in polling mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written
  * @param  NumOfBlocks: Number of SD blocks to write
  * @param  Timeout: Timeout for write operation
  * @retval SD status
  */
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
  HAL_StatusTypeDef  sd_state = HAL_OK;

  sd_state = HAL_SD_WriteBlocks(&uSdHandle, (uint8_t *)pData, WriteAddr, NumOfBlocks, Timeout);

  if (sd_state == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}
/**
  * @brief  Erases the specified memory area of the given SD card.
  * @param  StartAddr: Start byte address
  * @param  EndAddr: End byte address
  * @retval SD status
  */
uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr)
{
  HAL_StatusTypeDef  sd_state = HAL_OK;

  sd_state = HAL_SD_Erase(&uSdHandle, StartAddr, EndAddr);

  if (sd_state == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}
void BSP_SD_IRQHandler(void)
{
  HAL_SD_IRQHandler(&uSdHandle);
}
/**
  * @brief  Gets the current SD card data status.
  * @param  None
  * @retval Data transfer state.
  */
uint8_t BSP_SD_GetCardState(void)
{
  HAL_SD_CardStateTypedef card_state;
  card_state = HAL_SD_GetCardState(&uSdHandle);

  if (card_state == HAL_SD_CARD_TRANSFER)
  {
    return (SD_TRANSFER_OK);
  }
  else if ((card_state == HAL_SD_CARD_SENDING) || (card_state == HAL_SD_CARD_RECEIVING) ||(card_state == HAL_SD_CARD_PROGRAMMING))
  {
    return (SD_TRANSFER_BUSY);
  }
  else
  {
    return (SD_TRANSFER_ERROR);
  }
}
/**
  * @brief  Get SD information about specific SD card.
  * @param  CardInfo: Pointer to HAL_SD_CardInfoTypedef structure
  * @retval None
  */
void BSP_SD_GetCardInfo(BSP_SD_CardInfo *CardInfo)
{
  /* Get SD card Information */
  HAL_SD_GetCardInfo(&uSdHandle, CardInfo);
}
static void SD_Detect_MspInit(void)
{
	GPIO_InitTypeDef gpio;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	gpio.Pin=GPIO_PIN_5;
	gpio.Mode=GPIO_MODE_INPUT;
	gpio.Pull=GPIO_PULLUP;
	gpio.Speed=GPIO_SPEED_FREQ_MEDIUM;

	HAL_GPIO_Init(GPIOA,&gpio);
}
uint8_t BSP_SD_IsDetected(void)
{
  __IO uint8_t status = SD_PRESENT;

  if(HAL_GPIO_ReadPin(GPIOA,SD_DETECT_PIN)!=GPIO_PIN_RESET)
  {
	  status=SD_NOT_PRESENT;
  }
  return status;
}
static void SD_Detect_MspDeInit(void)
{
  /* Disable all interrupts */
  /*HAL_NVIC_DisableIRQ(MFX_INT_EXTI_IRQn);*/
	GPIO_InitTypeDef gpio;
	__HAL_RCC_GPIOA_CLK_DISABLE();
	gpio.Pin=GPIO_PIN_5;
	gpio.Mode=GPIO_MODE_ANALOG;
	gpio.Speed=GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOA,&gpio);


}
