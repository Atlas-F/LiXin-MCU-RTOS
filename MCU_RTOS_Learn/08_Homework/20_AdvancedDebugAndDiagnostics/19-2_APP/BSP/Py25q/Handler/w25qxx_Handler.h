/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file W25Q_Handler.h
 * 
 * @par dependencies 
 * - W25Q_Handler.h
 * 
 * @author Jack | R&D Dept. | EternalChip 立芯嵌入式
 * 
 * @brief Functions related to reading and writing in the chip's flash area.
 * 
 * Processing flow:
 * 
 * call directly.
 * 
 * @version V1.0 2024-09-13
 *
 * @note 1 tab == 4 spaces!
 * 
 *****************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __W25Q_HANDLER_H
#define __W25Q_HANDLER_H
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"

#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/*
最小读写单元Pag = 256 byte
一个扇区 = 16个Pag = 4096 byte
一个块 = 16个扇区 = 64KB
*/
typedef struct 
{
    uint8_t  databuf[4096];           //按找4096个数据进行读写设置
    uint16_t write_databuf_index;
    uint32_t write_index;
    uint8_t  write_sector_index;      //4096
    uint32_t read_index;
    uint8_t  read_sector_index;
}st_W25Q_Handler;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define EXTERN_Flash

/* Exported functions ------------------------------------------------------- */
void W25Q64_Init(void);
uint8_t W25Q64_EraseChip(void);

uint8_t W25Q64_WriteData(uint8_t *data, uint32_t length);
uint8_t W25Q64_WriteData_End(void);
uint8_t W25Q64_ReadData(uint8_t *data, uint16_t *length);
#endif /* __FLASH_H */
