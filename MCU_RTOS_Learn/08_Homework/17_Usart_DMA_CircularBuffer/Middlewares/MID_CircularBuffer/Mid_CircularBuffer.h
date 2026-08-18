/**
 * ********************************************************************************
 * @file   Mid_CircularBuffer.h
 * @brief  
 * 
 * @author AtlasFg
 * @date   2026-08-12
 * @version 1.0
 * 
 * @par 说明
 *      环形缓冲区相关操作函数使用前缀 Cirbuf
 * * 
 * @note 注意事项
 * * 
 * @warning 参考
 * * 
 * * 
 * ********************************************************************************
 */
#ifndef __MID_CIRCULAR_BUFFER_H__
#define __MID_CIRCULAR_BUFFER_H__
 /* Private includes ----------------------------------------------------------*/
#include <stdint.h>


 /* Private define ------------------------------------------------------------*/

typedef uint8_t data_type_t ;
#define CIRCULAR_BUFFER_SIZE 100 

 /* Private typedef -----------------------------------------------------------*/
/**
 * @brief 环形缓冲区 结构体  
 * 
 */
 typedef struct 
{
    uint8_t cirbuf_data[CIRCULAR_BUFFER_SIZE];
    uint32_t                      cirbuf_head;
    uint32_t                      cirbuf_tail;
} CircularBuffer_t;



 /* Private variables ---------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/



/* Private function declarations -----------------------------------------------*/

/**
 * @brief  环形缓冲区 创建函数
 * 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
CircularBuffer_t * Cirbuf_CreateEmptyCircularBuffer(void);

/**
 * @brief  判断环形缓冲区是否为空
 * 
 * @param [in]  p_cirbuf 
 * @return      
 * 
 * @details
 * @note 判断是否为空和满可以使用同一个函数来操作
 * @warning
 */
uint8_t Cirbuf_IsEmpty( CircularBuffer_t * p_cirbuf);

/**
 * @brief  判断环形缓冲区是否为满
 * 
 * @param [in]  p_cirbuf 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
uint8_t Cirbuf_IsFull( CircularBuffer_t * p_cirbuf);

/**
 * @brief  环形缓冲区 插入数据函数
 * 
 * @param [in]  p_cirbuf 环形缓冲区 对象
 * @param [in]  data  插入的数据
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
uint8_t Cirbuf_InsertData( CircularBuffer_t * p_cirbuf, data_type_t data );

/**
 * @brief  环形缓冲区 读取数据函数
 * 
 * @param [in]  p_cirbuf 环形缓冲区 对象
 * @param [in]  data  读取的数据
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
uint8_t Cirbuf_GetData( CircularBuffer_t * p_cirbuf, data_type_t * data );



#endif // __MID_CIRCULAR_BUFFER_H__

