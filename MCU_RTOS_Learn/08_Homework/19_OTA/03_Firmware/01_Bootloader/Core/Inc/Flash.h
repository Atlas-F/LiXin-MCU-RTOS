/**
 * 
 * @file   Flash.h
 * @brief  
 * 
 * @author AtlasFg
 * @date   2026-08-27
 * @version 1.0
 * 
 * @par 说明
 * * 
 * @note 注意事项
 * * 
 * @warning 参考
 * * 
 * * 
 * ********************************************************************************
 */

#ifndef __FLASH_H__
#define __FLASH_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "main.h"



/* Private functions ---------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000)   //扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000)   //扇区1起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000)   //扇区2起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000)   //扇区3起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000)   //扇区4起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000)   //扇区5起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000)   //扇区6起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000)   //扇区7起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000)   //扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000)   //扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000)   //扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000)   //扇区11起始地址,128 Kbytes
/* Exported functions ------------------------------------------------------- */
uint8_t Flash_erase(u32 addr,u32 size);
static uint16_t STMFLASH_GetFlashSector(u32 addr);
void FlashUnlock(void);

void FLashLock(void);

FLASH_Status EreaseAppSector( uint32_t FLASH_Sector );

void Flash_Write( uint32_t address, uint32_t data );


#endif /* __FLASH_H__ */

