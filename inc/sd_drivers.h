/*
 * sd_drivers.h
 *
 *  Created on: Feb 19, 2019
 *      Author: pawel
 */

#ifndef SD_DRIVERS_H_
#define SD_DRIVERS_H_

#include "stm32l4xx.h"

#define BSP_SD_CardInfo HAL_SD_CardInfoTypeDef

/**
  * @brief  SD status structure definition
  */
#define   MSD_OK                        ((uint8_t)0x00)
#define   MSD_ERROR                     ((uint8_t)0x01)
#define   MSD_ERROR_SD_NOT_PRESENT      ((uint8_t)0x02)

/**
  * @brief  SD transfer state definition
  */
#define   SD_TRANSFER_OK                ((uint8_t)0x00)
#define   SD_TRANSFER_BUSY              ((uint8_t)0x01)
#define   SD_TRANSFER_ERROR             ((uint8_t)0x02)

#define SD_DETECT_PIN            GPIO_PIN_9
#define SD_DETECT_PORT           GPIOB

#define SD_DATATIMEOUT           ((uint32_t)100000000)

#define SD_PRESENT               ((uint8_t)0x01)
#define SD_NOT_PRESENT           ((uint8_t)0x00)

/* SD IRQ handler */
#define SDMMCx_IRQHandler        SDMMC1_IRQHandler
#define SDMMCx_IRQn              SDMMC1_IRQn


/* DMA definitions for SD DMA transfer */
#define SD_DMAx_CLK_ENABLE       __HAL_RCC_DMA2_CLK_ENABLE
#define SD_DMAx_CLK_DISABLE      __HAL_RCC_DMA2_CLK_DISABLE
#define SD_DMAx_STREAM           DMA2_Channel5
#define SD_DMAx_IRQn             DMA2_Channel5_IRQn
#define SD_DMAx_IRQHandler       DMA2_Channel5_IRQHandler


uint8_t BSP_SD_Init(void);
uint8_t BSP_SD_DeInit(void);
uint8_t BSP_SD_ITConfig(void);
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t BSP_SD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks);
uint8_t BSP_SD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks);
uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr);
void    BSP_SD_IRQHandler(void);
void    BSP_SD_DMA_Tx_IRQHandler(void);
void    BSP_SD_DMA_Rx_IRQHandler(void);
uint8_t BSP_SD_GetCardState(void);
void    BSP_SD_GetCardInfo(BSP_SD_CardInfo *CardInfo);
uint8_t BSP_SD_IsDetected(void);

/* These __weak functions can be surcharged by application code in case the current settings
   (eg. interrupt priority, callbacks implementation) need to be changed for specific application needs */
void    BSP_SD_MspInit(SD_HandleTypeDef *hsd, void *Params);
void    BSP_SD_MspDeInit(SD_HandleTypeDef *hsd, void *Params);
void    BSP_SD_AbortCallback(void);
void    BSP_SD_WriteCpltCallback(void);
void    BSP_SD_ReadCpltCallback(void);

#endif /* SD_DRIVERS_H_ */
