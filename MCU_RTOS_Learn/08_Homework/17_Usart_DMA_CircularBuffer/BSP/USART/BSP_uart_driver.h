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

#include "usart.h"

#include "Mid_CircularBuffer.h"


/* Private variables ---------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/
void UartDriverFunc(void *argument);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

CircularBuffer_t * BSP_GetCircularBuffer(void);




#endif  // __BSP_UART_DRIVER_H__

