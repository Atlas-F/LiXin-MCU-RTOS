/**
 * 
 * @file   Bsp_Aht21b_Driver.h
 * 
 * @brief  BSP 层 Aht21b 传感器驱动程序
 * 
 * @details 
 * 
 * @include
 *          IIC library
 * 
 * @author AtlasFg
 * @date   2026-10-11
 * @version 1.0
 * 
 * @par 说明
 * * 
 * @note 注意事项
 * * 
 * @warning 
 *          当前使用的传感器型号实际为 Aht21bB，所以时机的数据手册参数与案例不同，需要
 *              注意
 * 
 * ****************************************************************************
 */

 #ifndef __BSP_Aht21b_DRIVER_H__
 #define __BSP_Aht21b_DRIVER_H__


 /* Private includes --------------------------------------------------------*/

#include <stdint.h>


 /* Private define ----------------------------------------------------------*/

#define CRC_ENABLE

#define OS_SUPPORTING

#define DEBUG


#define INIT_CMD 0x71

#define INIT_CHECK_AND 0X18

#define TRIGGER_MEASURE 0XAC
#define TRIGGER_MEASURE_PAR_1 0x33
#define TRIGGER_MEASURE_PAR_2 0x00

/* TODO : 这里的异常初始化寄存器的具体定义不正确 */
#define INIT_ABNORMAL_REG_1 0x1B
#define INIT_ABNORMAL_REG_2 0x1C
#define INIT_ABNORMAL_REG_3 0x1E

#define ORIGIN_DATA_LENGTH 6

#ifdef CRC_ENABLE
    #define TRANSFORMED_DATA_LENGTH 6
#else
    #define TRANSFORMED_DATA_LENGTH 5
#endif

/* TODO: CRC 多项式详细表达式*/
#define CRC_POLYNOMIAL 


 /* Private variables -------------------------------------------------------*/





 /* Private typedef ---------------------------------------------------------*/

 typedef enum
{
    Aht21b_STATUS_OK                 = 0,    /* 操作成功 */
    Aht21b_STATUS_ERROR              = 1,    /* 通用错误 */
    Aht21b_STATUS_ERROR_TIMEOUT      = 2,    /* 操作超时 */
    Aht21b_STATUS_ERROR_RESOURCE     = 3,    /* 资源已被占用或已初始化 */
    Aht21b_STATUS_ERROR_PARAMETER    = 4,    /* 输入参数无效 */
    Aht21b_STATUS_ERROR_NO_MEMORY    = 5,    /* 内存不足 */
    Aht21b_STATUS_ERROR_ISR          = 6,    /* 在中断上下文中发生错误 */
    Aht21b_STATUS_ERROR_NULL_POINTER = 7,    /* NULL 指针 */
    Aht21b_STATUS_RESERVED           = 0xFF, /* 保留状态值 */

} BspAht21bStatus_t;

/* TODO：函数具体结构待优化 */
typedef BspAht21bStatus_t (*BspAht21bDriver_SendCmdData_t)(
                                            Bsp_Aht21b_Driver_t* driver_self, \
                                            uint8_t              cmd_data    );

typedef BspAht21bStatus_t (*BspAht21bDriver_ReceiveCmdData_t)(
                                            Bsp_Aht21b_Driver_t* driver_self, \
                                            uint8_t              cmd_data    );

/* 该方法似乎应该放在 handler 层 */
typedef BspAht21bStatus_t (*BspAht21bDriver_WaitingTime_t)(
                                            Bsp_Aht21b_Driver_t* driver_self, \
                                            uint8_t              wait_time    );

typedef BspAht21bStatus_t (*BspAht21bDriver_CalTransformData_t)(
                                            Bsp_Aht21b_Driver_t* driver_self, \
                                            uint8_t*             origin_data, \
                                            uint8_t*         transformed_data);                                          

typedef struct 
{
    /* data */
    uint8_t                            is_initizlized ;
    uint8_t                     init_check_status_word;
        // uint8_t transformed_data[TRANSFORMED_DATA_LENGTH];
    uint8_t            origin_data[ORIGIN_DATA_LENGTH];
    uint8_t     sampling_data[TRANSFORMED_DATA_LENGTH];
        /* CRC 校验数据的长度似乎不是八位，需要注意订正 */
    uint8_t                                   CRC_data;

    /* operates */
    BspAht21bDriver_SendCmdData_t           BspAht21bDriver_SendCmdData;
    BspAht21bDriver_ReceiveCmdData_t     BspAht21bDriver_ReceiveCmdData;
    BspAht21bDriver_WaitingTime_t           BspAht21bDriver_WaitingTime;
    BspAht21bDriver_CalTransformData_t BspAht21bDriver_CalTransformData;


} Bsp_Aht21b_Driver_t;


/**
 * @brief  
 * 
 * @param [in]  driver_self 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
BspAht21bStatus_t BspAht21bDriver_Instantiate(Bsp_Aht21b_Driver_t* driver_self);











 #endif /* __BSP_Aht21b_DRIVER_H__ */

