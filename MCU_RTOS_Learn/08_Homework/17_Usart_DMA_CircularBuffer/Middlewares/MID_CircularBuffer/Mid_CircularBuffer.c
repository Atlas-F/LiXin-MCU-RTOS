/**
 * ********************************************************************************
 * @file   Mid_CircularBuffer.c
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

/* Private includes ----------------------------------------------------------*/
#include "Mid_CircularBuffer.h"

#include "stdio.h"
#include "elog.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "usart.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/* Private variables ---------------------------------------------------------*/



/* Private function definition ------------------------------------------------*/

/**
 * @brief  创建一个新的空的环形缓冲区
 * 
 * @return  返回创建的环形缓冲区指针    
 * 
 * @details
 * @note
 * @warning
 */
CircularBuffer_t * Cirbuf_CreateEmptyCircularBuffer(void)
{
    // 1. 定义局部变量
    CircularBuffer_t * p_circular_buf_tem = NULL ;
    // 2. 分配内存
    p_circular_buf_tem = (CircularBuffer_t *)malloc(sizeof(CircularBuffer_t));                // pvPortMalloc 分配的内存在 FreeRTOS 的堆栈中， malloc 直接分配在系统堆栈当中
    if( NULL == p_circular_buf_tem)
    {
        log_e(" ERROR: p_circular_buf_tem Malloc Failed ! \n ");
        return NULL ;
    }
    // 初始化
    memset(p_circular_buf_tem, 0, sizeof(CircularBuffer_t));

    // 3. 返回指针
    return p_circular_buf_tem ;
}


/**
 * @brief  判断环形缓冲区是否为空
 * 
 * @param [in]  p_cirbuf  circular buffer pointer
 * @return  uint8_t: 
 *                  0xFF: Circular buffer is NULL
 *                  0x00: Circular buffer is empty
 *                  0x01: Circular buffer is not empty
 * @details
 * @note 判断是否为空和满可以使用同一个函数来操作
 * @warning
 */
uint8_t Cirbuf_IsEmpty( CircularBuffer_t * p_cirbuf)
{
    if( NULL == p_cirbuf )
    {
        // log_e("ERROR: Cirbuf_IsEmpty p_cirbuf is NULL !\n");
        log_d("ERROR: Cirbuf_IsEmpty p_cirbuf is NULL !\n");
        return 0xFF;
    }

    if( p_cirbuf->cirbuf_head == p_cirbuf->cirbuf_tail)
    {
        // log_i(" p_cirbuf is Empty !\n ");
        return 0x00;
    }
    else
    {
        return 0x01;
    }
}


/**
 * @brief  判断环形缓冲区是否为空
 * 
 * @param [in]  p_cirbuf  circular buffer pointer
 * @return  uint8_t: 
 *                  0xFF: Circular buffer is NULL
 *                  0x00: Circular buffer is Full
 *                  0x01: Circular buffer is not Full
 * @details
 * @note 判断是否为空和满可以使用同一个函数来操作
 * @warning
 */
uint8_t Cirbuf_IsFull( CircularBuffer_t * p_cirbuf)
{
    if( NULL == p_cirbuf )
    {
        log_e("ERROR: Cirbuf_IsFull p_cirbuf is NULL !\n");
        return 0xFF;
    }

    if( ((p_cirbuf->cirbuf_head + 1)%CIRCULAR_BUFFER_SIZE ) == ((p_cirbuf->cirbuf_tail)%CIRCULAR_BUFFER_SIZE)  )
    {
        return 0x00;
    }
    else
    {
        return 0x01;
    }
}


/**
 * @brief  环形缓冲区 插入数据函数
 * 
 * @param [in]  p_cirbuf 环形缓冲区 对象
 * @param [in]  data  插入的数据
 * @return  uint8_t: 
 *                  0xFF: Circular buffer is NULL
 *                  0xFE: Circular buffer is Full
 *                  0x01: insert data success
 *                  0x02: insert data fail 
 * @details
 * @note
 * @warning
 */
uint8_t Cirbuf_InsertData( CircularBuffer_t * p_cirbuf, data_type_t data )
{
    if( NULL == p_cirbuf )
    {
        log_e("ERROR: Cirbuf_InsertData p_cirbuf is NULL !\n");
        return 0xFF;
    }
    if( 0x00 == Cirbuf_IsFull(p_cirbuf) )
    {
        log_i(" p_cirbuf is full  \n");
        return 0xFE ;
    }

    p_cirbuf->cirbuf_data[(p_cirbuf->cirbuf_head)%CIRCULAR_BUFFER_SIZE] = data ;
    p_cirbuf->cirbuf_head++ ;

    return 0x00;
}


/**
 * @brief  环形缓冲区 读取数据函数
 * 
 * @param [in]  p_cirbuf 环形缓冲区 对象
 * @param [in]  data  读取的数据
 * @return  uint8_t: 
 *                  0xFF: Circular buffer is NULL
 *                  0xFE: Circular buffer is empty
 *                  0x01: Get data success
 *                  0x02: Get data fail 
 * @details
 * @note
 * @warning
 */
uint8_t Cirbuf_GetData( CircularBuffer_t * p_cirbuf, data_type_t * data )
{
    if( NULL == p_cirbuf )
    {
        // log_e("ERROR: Cirbuf_GetData p_cirbuf is NULL !\n");
        log_d("ERROR: Cirbuf_GetData p_cirbuf is NULL !\n");
        return 0xFF;
    }
    if( 0x00 == Cirbuf_IsEmpty(p_cirbuf) )
    {
        log_i(" p_cirbuf is empty  \n");
        return 0xFE ;
    }

    *data = p_cirbuf->cirbuf_data[(p_cirbuf->cirbuf_tail)%CIRCULAR_BUFFER_SIZE] ;
    p_cirbuf->cirbuf_tail++ ;

    return 0x00;
}
