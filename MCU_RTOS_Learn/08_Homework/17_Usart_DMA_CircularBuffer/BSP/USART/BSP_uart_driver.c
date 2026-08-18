/**
 * ********************************************************************************
 * @file   BSP_uart_driver.c
 * @brief  BSP 层 UART 驱动程序
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
#include "BSP_uart_driver.h"

#include "stdio.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "usart.h"

#include "Mid_CircularBuffer.h"
#include "elog.h"


/* Private define  ----------------------------------------------------------*/
#define BUFFER_A 0
#define BUFFER_B 1

#define IRQ_SEND_TO_THREAD 0xA1A2A3A4
#define FRONT_SEND_TO_END 0xA4A3A2A1


/* Private variables ---------------------------------------------------------*/

extern QueueHandle_t g_Uart_irq_rec_A  ;

#if 0       // AB-buffer
uint8_t g_data_buf_A[1] = {0x00};
uint8_t g_data_buf_B[1] = {0x00};

uint8_t g_Buffer_AB = BUFFER_A ;

#endif      // AB-buffer

#if 1       // Circular buffer

uint8_t g_data_buffer = 0 ;     // 临时变量，用于存储数据
#endif      // Circular buffer


// 该文件全局 环形缓冲区 变量
static CircularBuffer_t * g_circular_buf_irq_thread = NULL ;
static QueueHandle_t g_queue_uart_irq_thread = NULL ;



/* Private function ----------------------------------------------------------*/


/**
 * @brief  
 * 
 * @param [in]  argument 
 * 
 * @details
 *      buffer 是否已满
 *      将当前数据就绪的事件发送给后端
 * @note
 * @warning
 */
void UartDriverFunc(void *argument)
{
    // g_Buffer_AB = BUFFER_A;      
    // Variables
    uint32_t receive_data = 0 ;

#if 0   // AB-buffer
    // create a circular buffer 
    CircularBuffer_t * p_cirbuffer = Cirbuf_CreateEmptyCircularBuffer();
    if( NULL == p_cirbuffer)
    {
        log_e(" ERROR: p_cirbuffer ! \n ");
    }
    log_i(" p_cirbuffer create success ! \n ");

    // judge Circular buffer is empty
    if( 0x00 == Cirbuf_IsEmpty(p_cirbuffer) )
    {
        log_i(" Circular Buffer is empty  \n");
    }

    // judge Circular buffer is full
    if( 0x00 == Cirbuf_IsFull(p_cirbuffer) )
    {
        log_i(" Circular Buffer is full  \n");
    }

    // insert data  and read data
    uint8_t insert_data = 0x34 ;
    if( 0x00 == Cirbuf_InsertData(p_cirbuffer, insert_data) )
    {
        log_i(" InsertData is success  \n");
    }

    uint8_t read_data = 0 ;
    if( 0x00 == Cirbuf_GetData(p_cirbuffer, &read_data) )
    {
        log_i(" GetData is success read_data : [%x] \n", read_data);
    }

    if( 0x00 == Cirbuf_IsEmpty(p_cirbuffer) )
    {
        log_i(" Circular Buffer is empty  \n");
    }

#endif    // AB-buffer

    // 1.创建 队列
    g_queue_uart_irq_thread = xQueueCreate(1, 4);
    if( NULL == g_queue_uart_irq_thread )
    {
        log_e(" ERROR: g_queue_uart_irq_thread create FAIl  ! \n ");
    }


    /* 创建空的环形缓冲区对象*/
    g_circular_buf_irq_thread = Cirbuf_CreateEmptyCircularBuffer();
    if( NULL == g_circular_buf_irq_thread)
    {
        log_e(" ERROR: g_circular_buf_irq_thread ! \n ");
    }
    log_i(" g_circular_buf_irq_thread create success ! \n ");

    HAL_StatusTypeDef ret = HAL_OK ;
    /* 2.1启动接收 此处为单字节*/
    // ret = HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);        // g_data_buf_A

    /* 2.2 串口空闲中断 + DMA 半满全满 */
    ret = HAL_UARTEx_ReceiveToIdle_DMA(&huart1, g_circular_buf_irq_thread->cirbuf_data, 10);      // CIRCULAR_BUFFER_SIZE
    if(HAL_OK == ret)
    {
        log_i(" HAL Uart Init Success \r\n");
    }else
    {
        log_i(" HAL Uart Init Fail \r\n");
    }

    // elog_i(LOG_TAG," HAL Init result : ret = [%d] \r\n", ret);

    for(;;)
    {
        // 判断是否有中断发送来队列通知环形缓冲区已有数据
        xQueueReceive(g_queue_uart_irq_thread, &receive_data, 0xFFFF);
            log_i(" front receive data from irq _data:[%x] \n ", receive_data);

        //  将当前数据就绪的事件发送给后端
        if( IRQ_SEND_TO_THREAD == receive_data )
        {
            uint32_t send_to_end = FRONT_SEND_TO_END ;
            // BaseType_t ret = xQueueSendToBack(g_Uart_irq_rec_A, &send_to_end, NULL);
            BaseType_t ret = xQueueGenericSend(g_Uart_irq_rec_A, &send_to_end, 0,  queueOVERWRITE);

            if( pdTRUE != ret )
            {
                log_e(" ERROR: xQueueSendToBack is Send to end FAIL ! \n");
            }
            else
            {
                // log_i("  front is Send to end Success ! \n");
            }
        }

    }

}



/**
 * @brief  
 * 
 * @param [in]  huart 
 * 
 * @details
 * @note
 * @warning
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // elog_i(LOG_TAG, " HAL_UART_RxCpltCallback \r\n");


    // long ret = 0 ;
    // uint32_t send_data_to_rec_A = 0x12 ;

    // BaseType_t	*pxHigherPriorityTaskWoken = pdFALSE ;
    // ret = xQueueSendFromISR(g_Uart_irq_rec_A, &send_data_to_rec_A , NULL );     // 为什么要设置为 NULL
    // portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);


    // HAL_StatusTypeDef ret = HAL_ERROR ;
    // ret = HAL_UART_Receive_IT(&huart1, g_data_buf_A, 1);
    // if(HAL_OK == ret)
    // {
    //     log_i(" HAL Uart Init Success \r\n");
    // }else
    // {
    //     log_i(" HAL Uart Init Fail \r\n");
    // }
#if 0       // AB buffer test
    // 切换AB buffer
    if( BUFFER_A == g_Buffer_AB )
    {
        log_i(" retval g_data_buf_A = [%c] \r\n", g_data_buf_A[0]);
        ret = HAL_UART_Receive_IT(&huart1, g_data_buf_B, 1);
        if( HAL_OK != ret)
        {
            log_e(" HAL_UART_Receive_IT BUFFER_B Failed ! \r\n");
        }
        g_Buffer_AB = BUFFER_B;
    }
    else
    {
        log_i(" retval g_data_buf_B = [%c] \r\n", g_data_buf_B[0]);
        ret = HAL_UART_Receive_IT(&huart1, g_data_buf_A, 1);
        if( HAL_OK != ret)
        {
            log_e(" HAL_UART_Receive_IT BUFFER_A Failed ! \r\n");
        }
        g_Buffer_AB = BUFFER_A;

    }
#endif      // AB buffer test

#if 0       // circular buffer test
    if( NULL == g_circular_buf_irq_thread )
    {
        log_e(" ERROR: g_circular_buf_irq_thread is NULL pointer ! \n");
        return ;
    }

// 将数据的写入指向环形缓冲区
    uint8_t retval = 0 ;
    retval = Cirbuf_InsertData( g_circular_buf_irq_thread, g_data_buffer);
    if( 0x00 == retval )
    {
        uint8_t read_data = 0 ;
        if( 0x00 == Cirbuf_GetData(g_circular_buf_irq_thread, &read_data) )
        {
            log_i(" g_circular_buf_irq_thread GetData is success read_data : [%d] \n", read_data);
        }
    }

// 测试：写入、读出、验证
    HAL_StatusTypeDef ret = HAL_ERROR ;
    ret = HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);        // g_data_buf_A
    if(HAL_OK == ret)
    {
        log_i(" HAL Uart Init Success \r\n");
    }else
    {
        log_i(" HAL Uart Init Fail \r\n");
    }


#endif      // circular buffer test

// 将数据存入环形缓冲区
// 通知前端，数据已经就绪
// 开启下一次串口接收
#if 1       // Circular buffer receive data and  next receive
    if( NULL == g_circular_buf_irq_thread )
    {
        log_e(" ERROR: g_circular_buf_irq_thread is NULL pointer ! \n");
        return ;
    }
    // 将数据存入环形缓冲区
    uint8_t retval = 0 ;
    retval = Cirbuf_InsertData( g_circular_buf_irq_thread, g_data_buffer);
    if( 0x00 != retval )
    {
        log_e(" ERROR: g_circular_buf_irq_thread Insert data FAIL  \n" );
    }
    // 通知前端，数据已经就绪

    uint32_t send_to_thread = IRQ_SEND_TO_THREAD ;
        // BaseType_t ret = xQueueSendToBackFromISR(g_queue_uart_irq_thread, &send_to_thread, NULL);
    BaseType_t ret = xQueueGenericSendFromISR(g_queue_uart_irq_thread, &send_to_thread, NULL, queueOVERWRITE);
    if( pdTRUE != ret )
    {
        log_e(" ERROR: xQueueSendToBackFromISR is Send to front FAIL ! \n");
    }
    else
    {
        // log_i("  xQueueSendToBackFromISR is Send to front Success ! \n");
    }
    //  开启下一次串口接收
    HAL_StatusTypeDef rec_ret = HAL_ERROR ;
    // rec_ret = HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);        // g_data_buf_A
    rec_ret = HAL_UARTEx_ReceiveToIdle_DMA(&huart1, &g_data_buffer, 10);      // CIRCULAR_BUFFER_SIZE
    if(HAL_OK == rec_ret)
    {
        // log_i(" HAL Uart Init Success \r\n");
    }else
    {
        // log_i(" HAL Uart Init Fail \r\n");
    }

#endif      // Circular buffer receive data and  next receive

}


/**
 * @brief  串口中断 函数
 * 
 * @param [in]  huart 
 * 
 * @details
 * @note
 *      DMA的半满中断、DMA的全满中断、串口的空闲中断，都要经过该回调函数
 * @warning
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,uint16_t size)
{
    log_d(" HAL_UARTEx_RxEventCallback ");
}


/**
 * @brief  获取返回 static 环形缓冲区地址 给外部调用 
 * 
 * @return      static 环形缓冲区 g_circular_buf_irq_thread 地址
 * 
 * @details
 *          这种函数一般要留给别人返回程序的状态，参数使用二级指针传递数据出去
 * @note
 * @warning
 */
CircularBuffer_t * BSP_GetCircularBuffer(void)
{
    if( NULL == g_circular_buf_irq_thread )
    {
        return  NULL ;
    }
    return g_circular_buf_irq_thread ; 
}

