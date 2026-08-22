/**
 * ********************************************************************************
 * @file   uart_parse_task.h
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

#ifndef __UART_PARSE_TASK__
#define __UART_PARSE_TASK__

/* Private includes ----------------------------------------------------------*/
#include <stdint.h>

#include "FreeRTOS.h"

#include "queue.h"



/* Private define ------------------------------------------------------------*/

#define    PROTO_MAX_LENGTH          (100)
#define    PROTO_MAX_SUBSCRIBERS     (100)
#define    PROTO_MAX_CMD_SUBSCRIBER     (100)       // 协议订阅者可订阅的最大命令数



/* Private typedef -----------------------------------------------------------*/

/**
 * @brief  订阅者描述
 * 
 * @details
 *          cmd：       订阅者可以订阅哪些命令
 *          rx_handle:  订阅者的通信邮箱地址，即通信邮箱句柄
 * @note
 * @warning
 */
typedef struct
{
    uint8_t cmd[PROTO_MAX_CMD_SUBSCRIBER] ;
    QueueHandle_t rx_handle ;
} App_subscriber_t;



/**
 * @brief  发送给订阅者的数据包
 * 
 * @details
 *          data: 数据信息
 *          data_cnt: 发送消息的协议ID
 * @note
 * @warning
 */
typedef struct
{
    uint32_t data_cnt;
    uint8_t data[PROTO_MAX_LENGTH];
} App_message_t ;


/* Private variables ---------------------------------------------------------*/

// uint8_t buffer1[1] = {0};
// uint8_t buffer2[1] = {0};


/* APP订阅者数组 */
// extern App_subscriber_t App_subscribers[PROTO_MAX_SUBSCRIBERS] ;









/* Private function prototypes -----------------------------------------------*/

void UartRecFuncA(void *argument);


/**
 * @brief  APP 订阅者数组注册
 * 
 * @param [in]  cmd        要订阅的命令
 * @param [in]  rx_handle  接收者的邮箱地址
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
uint8_t APP_ArraySubcribe( uint8_t cmd[10], QueueHandle_t rx_handle );













#endif // __UART_PARSE_TASK__
