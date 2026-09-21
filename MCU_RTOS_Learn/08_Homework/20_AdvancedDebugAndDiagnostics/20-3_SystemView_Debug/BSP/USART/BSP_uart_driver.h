/**
 * ********************************************************************************
 * @file   BSP_uart_driver.h
 * @brief  
 * 
 * @author AtlasFg
 * @date   2026-08-12
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

#ifndef __BSP_UART_DRIVER_H__
#define __BSP_UART_DRIVER_H__

/* Private includes ----------------------------------------------------------*/

#include <stdint.h>

#include "usart.h"

#include "Mid_CircularBuffer.h"


/* Private variables ---------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/
void UartDriverFunc(void *argument);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

/**
 * @brief  获取 BSP 文件中环形缓冲区对象的指针
 * 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
CircularBuffer_t * BSP_GetCircularBuffer(void);


void BSP_DMA_HalfFull_Irq_Callback( uint32_t number_of_data );

void BSP_DMA_CompleteFull_Irq_Callback( uint32_t number_of_data );

void BSP_UART_Idle_Irq_Callback( uint32_t number_of_data );


#endif  // __BSP_UART_DRIVER_H__

