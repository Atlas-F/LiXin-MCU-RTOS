/**
 * 
 * @file   Usart.h
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
#ifndef __USART_H__
#define __USART_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"



/* functions -----------------------------------------------------------------*/


void USART_SendChar( USART_TypeDef* USARTx, uint8_t data );

uint8_t USART_ReceiveChar(USART_TypeDef* USARTx);

void USART1_Init(void);












#endif /* __USART_H__ */

