/**
 * ********************************************************************************
 * @file   uart_parse_task.c
 * @brief  UART 接收数据并剖析数据 任务
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
/* Private includes ----------------------------------------------------------*/
#include "uart_parse_task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "elog.h"
#include "cmsis_os.h"
#include "queue.h"

#include "usart.h"

#include "Mid_CircularBuffer.h"
#include "BSP_uart_driver.h"

/* Private define ------------------------------------------------------------*/

// 数据帧 帧头 帧尾 flag
#define FRAME_NOT_DETECTED  (0x01)
#define FRAME_HEAD          (0x02)
#define FRAME_END           (0x03)

#define FRAME_HEAD_FLAG     (0xFE)
#define FRAME_END_FLAG      (0xFF)



/* Private variables ---------------------------------------------------------*/
/* APP订阅者数组 */
App_subscriber_t App_subscribers[PROTO_MAX_SUBSCRIBERS] = {0} ;

QueueHandle_t g_Uart_irq_rec_A = NULL ;

static CircularBuffer_t * g_circular_buf_from_drv = NULL ;

static g_App_subscribe_number = 0 ;        // APP 注册命令cmd数量


/* Private function ----------- -----------------------------------------------*/

// 没有retval
/**
 * @brief  UART 接收数据，并解析数据任务
 * 
 * @param [in]  argument 无作用
 * 
 * @details
 * @note
 * @warning
 */
void UartRecFuncA(void *argument)
{
  /* USER CODE BEGIN UartRecFuncA */
    uint32_t rec_data = 0 ;
    static uint32_t receive_data_cnt = 0 ;

    elog_i(LOG_TAG, " UartRecFuncA Init Success !\r\n");

    // 创建队列
    g_Uart_irq_rec_A = NULL ;
    g_Uart_irq_rec_A = xQueueCreate(1, 4) ;
    if( NULL == g_Uart_irq_rec_A)
    {
        log_i(" g_Uart_irq_rec_A Create Fail !\r\n");
    }else
    {
        log_i(" g_Uart_irq_rec_A Create Success !\r\n");
    }

    // 拿到 环形缓冲区地址
    g_circular_buf_from_drv = BSP_GetCircularBuffer();
    if( NULL == g_circular_buf_from_drv)
    {
        log_e(" ERROR: g_circular_buf_from_drv is NULL pointer \n");
    }

    /* Infinite loop */
    for(;;)
    {
        xQueueReceive(g_Uart_irq_rec_A, &rec_data, 0xFFFF) ;
        log_i(" rec_data = [%x] \r\n", rec_data);

        // 依次将环形缓冲区中的数据进行实现和解析
        if( NULL == g_circular_buf_from_drv )
        {
            log_e(" ERROR: g_circular_buf_from_drv is NULL pointer \n");
        }
        // 数据解析
        while ( 0x00 != Cirbuf_IsEmpty(g_circular_buf_from_drv) )
        {
            uint8_t temp_data = 0 ;
            static uint8_t data_counter = 0 ;
            static uint8_t temp_data_array[20] = {0};      // 解包缓冲

            if( 0x00 == Cirbuf_GetData(g_circular_buf_from_drv, &temp_data) )
            {
                // log_i(" Analysis data  : [%d] \n", temp_data);
            }
            // osDelay(4);
            // 寻找帧头
            static uint32_t status = FRAME_NOT_DETECTED ;
            switch (status)
            {
                case FRAME_NOT_DETECTED:                    // 寻找帧头，检测到帧头，则开始输出后面的数据
                    if (FRAME_HEAD_FLAG == temp_data)
                    {
                        log_i("Data packet start ");        
                        status = FRAME_HEAD ;
                    }
                    break;

                case FRAME_HEAD:
                    if( FRAME_END_FLAG == temp_data )       // 检测到帧尾，发送一个换行
                    {
                        log_i("Data packet end ");
                        status = FRAME_NOT_DETECTED;
                        // 计算校验和
                        uint32_t data_sum = temp_data_array[data_counter-1];
                        uint32_t data_sum_temp = 0 ;
                        for( int i = 0; i < data_counter-1; i++ )
                        {
                            data_sum_temp += temp_data_array[i];
                        }
                        log_i("data_sum_temp :[%d] ", data_sum_temp );

                        // 判断校验和  // 打印净荷数据
                        if( data_sum == data_sum_temp )     
                        {
                            // TODO：解析数据
                            // 新协议： HEAD(1) + CMD(1) + LENGTH(1) + DATA(n) + CHK(1) + TAIL(1)
                            /*1、解析协议中的cmd 数据，cmd 数据在协议中的帧头后第一个*/
                                log_i(" Data CMD : [%d] ", temp_data_array[0]);

                                App_message_t temp_msg= { .data_cnt = receive_data_cnt++ } ;    // 对于订阅者来说 这个计数值有什么作用？
                                for( int i = 0; i < temp_data_array[1] ; i++ )
                                {
                                    temp_msg.data[i] = temp_data_array[i+2] ;       // 这里不应该是 i+2 吗。从length 后面开始传递数据
                                }
                            /*2、解析协议中的数据，净数据在length 后*/
                                log_i(" Data Length : [%d] ", temp_data_array[1]);

                            /*3、转发到对应的APP订阅者 */
                            for( int i = 0 ; i < PROTO_MAX_SUBSCRIBERS ; i++ )
                            {
                                for( int j = 0 ; j < PROTO_MAX_CMD_SUBSCRIBER ; j++ )
                                {
                                    if( App_subscribers[i].cmd[j] == temp_msg.data[0] )
                                    {
                                        xQueueSendToBack( App_subscribers[i].rx_handle , &temp_msg, 0);
                                    }
                                }
                            }


                            // 打印净荷数据，除帧头和帧尾
                            for( int i = 3; i < data_counter - 1 ; i++ )
                            {
                                log_i(" Data : [%d] ", temp_data_array[i]);
                            }
                            // for( int i = 0; i < data_counter - 1 ; i++ )
                            // {
                            //     log_i(" Data : [%d] ", temp_data_array[i]);
                            // }
                        }
                        // 清零
                        for( int i = 0; i < data_counter ; i++ )
                        {
                            temp_data_array[i] = 0x00 ;
                        }
                        data_counter = 0 ;
                    }
                    else{
                        // log_i("FRAME_HEAD data :[%d] \n", temp_data);
                        temp_data_array[data_counter] = temp_data ;
                        data_counter++;
                        log_i("data_counter :[%d] \n", data_counter);
                    }
                    break;
                
                default:                                    // 持续输出数据，并时刻检查帧尾
                    break;
            }

        }
        
        osDelay(1);
    }
    /* USER CODE END UartRecFuncA */
}



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
uint8_t APP_ArraySubcribe( uint8_t cmd[PROTO_MAX_CMD_SUBSCRIBER], QueueHandle_t rx_handle )
{
    // App_subscribers[g_App_subscribe_number++].cmd = cmd ;
    for( int i = 0; i < PROTO_MAX_CMD_SUBSCRIBER; i++ )
    {
        if( g_App_subscribe_number < PROTO_MAX_SUBSCRIBERS )
        {
            return 0 ;
        }
        // App_subscribers[g_App_subscribe_number++].cmd[i] = cmd[i] ;
        App_subscribers[g_App_subscribe_number].cmd[i] = cmd[i] ;
    }
    App_subscribers[g_App_subscribe_number].rx_handle = rx_handle ;
    g_App_subscribe_number++;

}
