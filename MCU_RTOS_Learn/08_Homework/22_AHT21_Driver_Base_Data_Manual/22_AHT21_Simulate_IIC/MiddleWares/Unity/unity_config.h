/**
 * 
 * @file   unity_config.h
 * @brief  
 * 
 * @author AtlasFg
 * @date   2026-10-17
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


#ifndef __UNITY_CONFIG_H__
#define __UNITY_CONFIG_H__
#ifdef __cplusplus
extern "C"
{
#endif
#include "usart.h"
#define UNITY_OUTPUT_CHAR(a) HAL_UART_Transmit(&huart1,(uint8_t[]){(uint8_t)(a)},1,10)
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //__UNITY_CONFIG_H__