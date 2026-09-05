/**
 * 
 * @file   SPI.h
 * @brief  
 * 
 * @author AtlasFg
 * @date   2026-09-03
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

 #ifndef __SPI_H__
 #define __SPI_H__


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define F_CS_Pin GPIO_Pin_4
#define F_CS_GPIO_Port GPIOA
/* Exported functions ------------------------------------------------------- */

void SPI1_Init(void);
u8 SPI1_WriteByte(u8 *WriteData, u16 dataSize, u32 timeout);
u8 SPI1_ReadByte(u8 *ReadData, u16 dataSize, u32 timeout);



#endif /* __SPI_H__ */