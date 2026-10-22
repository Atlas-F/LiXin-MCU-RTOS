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
 * 
 *      1、当前初版，还缺哪些内容？
 *      2、这些定义应该放在 头文件 中还是源文件中？
 * 
 *      很多时候语义存在分歧，大家理解的语义不同
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



#define OS_SUPPORTING

#define DEBUG

/* @note 同样的指令码在不同的操作中有不同的含义，为什么不能定义多个不同的宏，
    但是实际为同样的数据？例如：0x71 */

// #define AHT21B_INIT_CMD_CODE    0x71
#define AHT21_REG_READ_ADDR       0x71
#define AHT21B_STATUS_CMD_CODE    0x71
#define AHT21B_REG_WRITE_ADDR     0X70

#define STATUS_CHECK_AND_CODE     0X18

#define TRIGGER_MEASURE_CODE    0XAC
#define TRIGGER_MEASURE_PAR_1   0x33
#define TRIGGER_MEASURE_PAR_2   0x00

/* TODO : 这里的异常初始化寄存器的具体定义不正确 */
#define INIT_ABNORMAL_REG_1     0x1B
#define INIT_ABNORMAL_REG_2     0x1C
#define INIT_ABNORMAL_REG_3     0x1E

#define ORIGIN_DATA_LENGTH      6


#define CRC_ENABLE
/* TODO: CRC 多项式详细表达式*/
#define CRC_POLYNOMIAL 
#ifdef CRC_ENABLE
    #define TRANSFORMED_DATA_LENGTH 6
#else
    #define TRANSFORMED_DATA_LENGTH 5
#endif

 /* -------------------------------------------------------------------------*/
/**
 * @note 上述定义是自己写的，不完整，
 *          缺少初始化标志位、以及一些 AHT21B 使用测量过程的参数
 */

#define SYSTEM_START_WAITING_TIME 100
 #define AHT21B_MEASURE_WAITING_TIME    80      

 /* @note 为什么要使用这种宏定义来作为是否初始化的标志位，而不是定义在结构体中？ */
 #define AHT21B_INITED                  1
 #define AHT21B_NOT_INITED              0

 #define AHT21B_ID                      

 #define CRC8_POLYNOMIAL                0x31    /* [P(x)=1+x^4+x^5+x^8] */
 #define CRC8_INITAL                    0xFF






 /* Private variables -------------------------------------------------------*/

// static int8_t  g_aht21b_inited    = AHT21B_NOT_INITED;
// static uint8_t g_device_id = 0 ;
 













 /* Private typedef ---------------------------------------------------------*/

 typedef enum
{
    AHT21B_STATUS_OK                 = 0,    /* 操作成功 */
    AHT21B_STATUS_ERROR              = 1,    /* 通用错误 */
    AHT21B_STATUS_ERROR_TIMEOUT      = 2,    /* 操作超时 */
    AHT21B_STATUS_ERROR_RESOURCE     = 3,    /* 资源已被占用或已初始化 */
    AHT21B_STATUS_ERROR_PARAMETER    = 4,    /* 输入参数无效 */
    AHT21B_STATUS_ERROR_NO_MEMORY    = 5,    /* 内存不足 */
    AHT21B_STATUS_ERROR_ISR          = 6,    /* 在中断上下文中发生错误 */
    AHT21B_STATUS_ERROR_NULL_POINTER = 7,    /* NULL 指针 */
    AHT21B_STATUS_RESERVED           = 0xFF, /* 保留状态值 */

} BspAht21bStatus_t;



#ifndef HARDWARE_IIC    /*  software IIC  */
typedef struct 
{
    /* data */
    BspAht21bStatus_t (*pf_IIC_Init)        (void *);   /* TODO : 注释 */
    BspAht21bStatus_t (*pf_IIC_DeInit)      (void *);
    BspAht21bStatus_t (*pf_IIC_Start)       (void *);
    BspAht21bStatus_t (*pf_IIC_Stop)        (void *);
    BspAht21bStatus_t (*pf_IIC_WaitACK)     (void *);
    BspAht21bStatus_t (*pf_IIC_SendACK)     (void *);
    BspAht21bStatus_t (*pf_IIC_SendNACK)    (void *);
    BspAht21bStatus_t (*pf_IIC_SendByte)    (void *,   /*   IIC s-byte  interf.*/
                                            const    uint8_t);
    BspAht21bStatus_t (*pf_IIC_ReceiveByte) (void *,   /*   IIC r-byte  interf.*/
                                            uint8_t * const );  
    BspAht21bStatus_t (*pf_CriticalEnter)   (void *);
    BspAht21bStatus_t (*pf_CriticalExit)    (void *);

}IIC_DriverInterface_t;
#endif 

/* hardware IIC  */

#ifdef HARDWARE_IIC     /* HARDWARE_IIC */
typedef struct 
{
    /* data */
    uint8_t (*pf_IIC_Init)               (void *);   /* TODO : 注释 */
    uint8_t (*pf_IIC_DeInit)             (void *);
    void (*pf_IIC_SendACK)               (void *);
    void (*pf_IIC_SendNACK)              (void *);
    void (*pf_IIC_SendByte)     (void *, uint8_t);  /* 这里为什么只是uint8_t? */
    void (*pf_IIC_ReceiveByte)           (void *);
    
}IIC_DriverInterface_t;


#endif /* HARDWARE_IIC */



/* 时基 */

typedef struct Bsp_AHT21B_Driver
{
    /* data */
    uint32_t (*pf_GetTickCount)(void);
} BspAht21b_TimeBaseInterface_t;


/* 操作系统 */



/* 延时 */

#ifdef OS_SUPPORTING
typedef struct 
{
    /* data */
    void (*pf_RTOS_Yield)(const uint32_t);  /*  RTOS 的非阻塞延时 */
}BspAht21b_RTOS_YieldInterface_t;
#endif /* OS_SUPPORTING */


/**
 * @brief  Bsp_Aht21b_Driver 核心结构体
 * 
 * @details
 * @note
 * @warning
 */
typedef struct 
{
    /* data */


    /* interface */
    IIC_DriverInterface_t           *pIIC_DriverInstance;
    BspAht21b_TimeBaseInterface_t    *pTimeBaseInterface;
    BspAht21b_RTOS_YieldInterface_t *pRTOS_YieldInstance;


    BspAht21bStatus_t (*pAht21b_DriverInst)(             void * const pAht21bInstance ,
                    IIC_DriverInterface_t           * const pIIC_DriverInstance,
                    BspAht21b_TimeBaseInterface_t   * const pTimeBaseInterface,
#ifdef OS_SUPPORTING
                    BspAht21b_RTOS_YieldInterface_t * const pRTOS_YieldInstance);
#endif /* OS_SUPPORTING */                                                     


    /* 这里为什么要用uint8_t , 而不是定义的 BspAht21bStatus_t 类型，是因为为了避免类型错误吗？*/
    BspAht21bStatus_t (*pfAht21b_Init)(void * const);
    BspAht21bStatus_t (*pfAht21b_DeInit)(void * const);
    uint8_t          (*pfAht21b_ReadID)(void * const);
    BspAht21bStatus_t (*pfAht21b_ReadTempHumi)(void * const,
                                float * const temp,
                                float * const humi);
    BspAht21bStatus_t (*pfAht21b_ReadHumi)(void * const,
                                float * const humi);
    BspAht21bStatus_t (*pfAht21b_Sleep)(void * const);
    BspAht21bStatus_t (*pfAht21b_WakeUp)(void * const);

} Bsp_Aht21b_Driver_t;




/**
 * @brief  Aht21b 驱动实例化
 * 
 * @param [in]  pAht21bInstance 
 * @param [in]  pIIC_DriverInstance 
 * @param [in]  pTimeBaseInterface 
 * @param [in]  pRTOS_YieldInstance 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
BspAht21bStatus_t pAht21b_DriverInst(              Bsp_Aht21b_Driver_t * const pAht21bInstance ,
                            IIC_DriverInterface_t            * const pIIC_DriverInstance,
                            BspAht21b_TimeBaseInterface_t    * const pTimeBaseInterface,
#ifdef OS_SUPPORTING
                            BspAht21b_RTOS_YieldInterface_t  * const pRTOS_YieldInstance);
#endif /* OS_SUPPORTING */                                                              












// /* TODO：函数具体结构待优化 */
// typedef BspAht21bStatus_t (*BspAht21bDriver_SendCmdData_t)(
//                                             Bsp_Aht21b_Driver_t* driver_self, \
//                                             uint8_t              cmd_data    );

// typedef BspAht21bStatus_t (*BspAht21bDriver_ReceiveCmdData_t)(
//                                             Bsp_Aht21b_Driver_t* driver_self, \
//                                             uint8_t              cmd_data    );

// /* 该方法似乎应该放在 handler 层 */
// typedef BspAht21bStatus_t (*BspAht21bDriver_WaitingTime_t)(
//                                             Bsp_Aht21b_Driver_t* driver_self, \
//                                             uint8_t              wait_time    );

// typedef BspAht21bStatus_t (*BspAht21bDriver_CalTransformData_t)(
//                                             Bsp_Aht21b_Driver_t* driver_self, \
//                                             uint8_t*             origin_data, \
//                                             uint8_t*         transformed_data);                                          

// typedef struct 
// {
//     /* data */
//     uint8_t                            is_initizlized ;
//     uint8_t                     init_check_status_word;
//         // uint8_t transformed_data[TRANSFORMED_DATA_LENGTH];
//     uint8_t            origin_data[ORIGIN_DATA_LENGTH];
//     uint8_t     sampling_data[TRANSFORMED_DATA_LENGTH];
//         /* CRC 校验数据的长度似乎不是八位，需要注意订正 */
//     uint8_t                                   CRC_data;

//     /* operates */
//     BspAht21bDriver_SendCmdData_t           BspAht21bDriver_SendCmdData;
//     BspAht21bDriver_ReceiveCmdData_t     BspAht21bDriver_ReceiveCmdData;
//     BspAht21bDriver_WaitingTime_t           BspAht21bDriver_WaitingTime;
//     BspAht21bDriver_CalTransformData_t BspAht21bDriver_CalTransformData;


// } Bsp_Aht21b_Driver_t;




 /* Private declaring -------------------------------------------------------*/
























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

