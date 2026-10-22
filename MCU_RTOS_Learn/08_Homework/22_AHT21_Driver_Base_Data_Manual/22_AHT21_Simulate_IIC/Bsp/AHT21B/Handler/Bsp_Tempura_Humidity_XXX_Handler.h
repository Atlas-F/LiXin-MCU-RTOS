/**
 * 
 * @file   Bsp_Tempura_Humidity_XXX_Handler.h
 * @brief   Bsp层 温湿度测量 Handler 层 
 * 
 * @author AtlasFg
 * @date   2026-10-16
 * @version 1.0
 * 
 * @details 解耦 driver 和上层应用
 * 
 * 包含文件：
 *      - _AHT21B_Driver.h
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
#ifndef ___BSP_TEMP_HUMI_SENSOR_HANDLER_H__
#define ___BSP_TEMP_HUMI_SENSOR_HANDLER_H__

/* Private includes --------------------------------------------------------*/
#include "Bsp_Aht21b_Driver.h"


#include <stdint.h>
#include <stdbool.h>

/* Private define ----------------------------------------------------------*/

#define HANDLER_DEBUG

#define OS_SUPPORTING

// #define FREERTOS_SUPPORTING
/* 问题也太多了，tmd，这里暂时不打开  FREERTOS_SUPPORTING 支持，但是临界区的接口
        必须重新设计*/

#define UNITY_TEST_TEMP_HUMI_HANDLER

#define DATA_TEMP_HUMI_LIFETIME     (5000) /* 温湿度数据新鲜度：数据的有效期：单位：ms */


#define DIFFERENT_TYPE_TEMP_HUMI_SENSOR_SUPPORT 



#ifdef DIFFERENT_TYPE_TEMP_HUMI_SENSOR_SUPPORT

    // #ifndef TEMP_HUMI_SENSOR_SUPPORT_AHT21B
        #define SENSOR_SUPPORT_AHT21B
        // #define SENSOR_SUPPORT_DHT11
        // #define SENSOR_SUPPORT_AHT21
    // #endif

#endif /* DIFFERENT_TYPE_TEMP_HUMI_SENSOR_SUPPORT */

// #if SENSOR_SUPPORT_AHT21B == DIFFERENT_TYPE_TEMP_HUMI_SENSOR_SUPPORT


/* Private variables -------------------------------------------------------*/

// extern void * g_temp_humi_handler_event_queue ;


/* Private typedef ---------------------------------------------------------*/


typedef struct TempHumiHandlerInitedFlag TempHumiHandlerInitedFlag_t;

typedef struct TempHumiHandlerSensorType TempHumiHandlerSensorType_t;


typedef IIC_DriverInterface_t TempHumiHandler_ReplaceIICInterface_t;

/* @brief 模块内部维护的一份数据 */
typedef struct
{
    float temperature;
    float humidity;
    uint32_t timestamp;
    bool valid;
} TempHumiData_t;

typedef enum
{
    HANDLER_OK                 = 0,    /* 操作成功 */
    HANDLER_ERROR              = 1,    /* 通用错误 */
    HANDLER_ERROR_TIMEOUT      = 2,    /* 操作超时 */
    HANDLER_ERROR_RESOURCE     = 3,    /* 资源已被占用或已初始化 */
    HANDLER_ERROR_PARAMETER    = 4,    /* 输入参数无效 */
    HANDLER_ERROR_NO_MEMORY    = 5,    /* 内存不足 */
    HANDLER_ERROR_ISR          = 6,    /* 在中断上下文中发生错误 */
    HANDLER_ERROR_NULL_POINTER = 7,    /* NULL 指针 */
    HANDLER_RESERVED           = 0xFF, /* 保留状态值 */

} TempHumiHandlerStatus_t;


/* IIC */
typedef struct 
{
    /* data */
    TempHumiHandlerStatus_t (*pf_IIC_Init)        (void *);   /* TODO : 注释 */
    TempHumiHandlerStatus_t (*pf_IIC_DeInit)      (void *);
    TempHumiHandlerStatus_t (*pf_IIC_Start)       (void *);
    TempHumiHandlerStatus_t (*pf_IIC_Stop)        (void *);
    TempHumiHandlerStatus_t (*pf_IIC_WaitACK)     (void *);
    TempHumiHandlerStatus_t (*pf_IIC_SendACK)     (void *);
    TempHumiHandlerStatus_t (*pf_IIC_SendNACK)    (void *);

    TempHumiHandlerStatus_t (*pf_IIC_SendByte)( void *,const uint8_t data);
    TempHumiHandlerStatus_t (*pf_IIC_ReceiveByte)(void *,const  uint8_t *data);

    TempHumiHandlerStatus_t (*pf_CriticalEnter)   (void *);
    TempHumiHandlerStatus_t (*pf_CriticalExit)    (void *);

}TempHumiHandler_IICInterface_t;


// typedef struct 
// {


// }TempHumiHandler_CriticalInterface_t;



/* 时基 */
typedef struct 
{
    /* data */
    uint32_t (*pf_GetTickCount)(void);
} TempHumiHandler_TimeBaseInterface_t;




/* 延时 */

#ifdef OS_SUPPORTING
typedef struct 
{
    /* data */
    void (*pf_RTOS_Yield)(const uint32_t);  /*  RTOS 的非阻塞延时 */
}TempHumiHandler_RTOS_YieldInterface_t;
#endif /* OS_SUPPORTING */

/* 温度传感器事件类型枚举 */
/* @note 这里的事件枚举可以设计为温度、湿度、二者，但是觉得缺少一些扩展性 */
typedef enum
{
    TempHumi_EVENT_1        = 0,      /* 传感器事件 1 ，待定保留，TODO： */
    TempHumi_EVENT_2        = 1,      /* 传感器事件 2 ，待定保留，TODO： */
    TempHumi_EVENT_3        = 2,      /* 传感器事件 3 ，待定保留，TODO： */
    TempHumi_EVENT_4        = 3,      /* 传感器事件 4 ，待定保留，TODO： */
    TempHumi_EVENT_Temp     = 4,      /* 传感器事件 4 ，待定保留，TODO： */
    TempHumi_EVENT_Humi     = 5,      /* 传感器事件 4 ，待定保留，TODO： */
    TempHumi_EVENT_RESERVED = 0xFF,   /* 传感器事件 FF ，待定保留，TODO： */
}TempHumiHandler_EventType_t;

/* 事件通知 */
typedef struct 
{
    /* data */
    // Bsp_Aht21b_Driver_t             *TempHumi;
    void *                          temp_humi_sensor;
    uint8_t                         is_inited;     /* 这里的初始化标志位是否有必要？ */
    uint32_t                        tempureture;
    uint32_t                        humidity;
    // uint32_t                        data_life_time;
    uint32_t                        time_stamp;     /* 该时间戳为数据获取到时的时间戳 */
    TempHumiHandler_EventType_t     TempHumi_event_type;

    /* @note 这个回调函数的参数是有必要的吗？传入一个sensor指针即可 */
    uint8_t (*pf_EventCallBack)(void * TempHumi);
}TempHumiHandler_Event_t;


/* 队列接口 */
typedef struct
{
    TempHumiHandlerStatus_t (*TempHumiHandler_CreateQueue)( uint32_t item_num,
                                                            uint32_t item_size,
                                                            void   **queue_handle); 
                                                         /* 这里返回句柄，后面要使用，应该要强转 */

    TempHumiHandlerStatus_t (*TempHumiHandler_DestroyQueue)(void ** queue_handle);

    TempHumiHandlerStatus_t (*TempHumiHandler_PutQueue)(void *const queue_handle,
                                                        void *const  queue_item,
                                                        uint32_t     timeout);

    TempHumiHandlerStatus_t (*TempHumiHandler_PopQueue)(void *const queue_handle,
                                                        void *const queue_item,
                                                        uint32_t    timeout);
} TempHumiHandler_OsQueue_t;



#ifdef OS_SUPPORTING

/** @brief Handler 延时适配接口。 */
typedef struct
{   
    /* 操作系统 毫秒 延时接口 */
    TempHumiHandlerStatus_t (*TempHumiHandler_DelayMs)(uint32_t delay_time); 
    /* 操作系统 微秒 延时接口 */
    TempHumiHandlerStatus_t (*TempHumiHandler_DelayUs)(uint32_t delay_time); 
} TempHumiHandler_OsDelay_t;
#endif /* OS_SUPPORTING */

/* 线程接口 */
typedef struct
{
    TempHumiHandlerStatus_t (*TempHumiHandler_CreateThread)(void             *create_task,
                                                          const char *const thread_name,
                                                          uint16_t          stack_depth,
                                                          void *const       parameters,
                                                          uint16_t          thread_priority,
                                                          void            **thread_handle);
    /*  void *const */

    TempHumiHandlerStatus_t (*TempHumiHandler_DestroyThread)(void * thread_handle);

} TempHumiHandler_OsThread_t;





/**
 * @brief  外部向 Handler 内部传入的参数
 * @details
 * @note
 * @warning
 */
typedef struct 
{
    /* data */
    // TempHumiHandler_IICInterface_t         * iic_handler_interface;
    TempHumiHandler_ReplaceIICInterface_t   * iic_handler_interface;
    TempHumiHandler_TimeBaseInterface_t    * time_base_interface;
#ifdef OS_SUPPORTING
    TempHumiHandler_RTOS_YieldInterface_t  * yield_interface;
    TempHumiHandler_OsQueue_t              * os_queue;
    TempHumiHandler_OsDelay_t              * os_delay;
    TempHumiHandler_OsThread_t             * os_thread;
#endif /* OS_SUPPORTING */
}TempHumiHandler_InputArgument_t;



/* Handler 核心结构体 */
typedef struct 
{
    /* data */
    /* @note 温湿度数据是否要设计成 私有的 ？ */
    float                       tempurature;
    float                        humiditity;
    uint32_t                 last_temp_tick;

    bool               temp_humi_data_valid;

    TempHumiHandler_Event_t * TempHumi_event;  /* 事件通知需要考虑到并发和竞态 */
#ifdef SENSOR_SUPPORT_AHT21B
    // static BspAht21b_TimeBaseInterface_t g_aht21b_time_base;
    // static BspAht21b_RTOS_YieldInterface_t g_aht21b_yield;

#endif /* SENSOR_SUPPORT_AHT21B */


    /* operate */
    // TempHumiHandler_IICInterface_t         * iic_handler_interface;
    TempHumiHandler_ReplaceIICInterface_t  * iic_handler_interface;
    TempHumiHandler_TimeBaseInterface_t    * time_base_interface;
    TempHumiHandler_RTOS_YieldInterface_t  * yield_interface;
    TempHumiHandlerInitedFlag_t            * is_inited_flag;

    TempHumiHandlerSensorType_t            * sensor_device;
    /* 维护一个事件通信的队列句柄 */
    // void * event_queue_handle;

#ifdef OS_SUPPORTING
    TempHumiHandler_OsQueue_t  * os_queue;
    TempHumiHandler_OsDelay_t  * os_delay;
    TempHumiHandler_OsThread_t * os_thread;
#endif /* OS_SUPPORTING */

    uint8_t (*pf_HandlerInst)(  
                                                      void * temp_humi_handler,
                            // TempHumiHandler_IICInterface_t * iic_handler_interface,
                            TempHumiHandler_ReplaceIICInterface_t * iic_handler_interface,
                    // TempHumiHandler_CriticalInterface_t    * critical_interface,
                    TempHumiHandler_TimeBaseInterface_t    * time_base_interface,
#ifdef OS_SUPPORTING
                    TempHumiHandler_RTOS_YieldInterface_t  * yield_interface,
                                TempHumiHandler_OsQueue_t  * os_queue,
                                TempHumiHandler_OsDelay_t  * os_delay,
                                TempHumiHandler_OsThread_t * os_thread);
#endif /* OS_SUPPORTING */

/* Deinst 函数的具体内容 */
    uint8_t (*pf_HandlerDeInst)(  void * temp_humi_handler );

}TempHumi_Handler_t;





/* Private declaring -------------------------------------------------------*/


/**
 * @brief  
 * 
 * @param [in]  temp_humi_handler 
 * @param [in]  iic_handler_interface 
 * @param [in]  critical_interface 
 * @param [in]  time_base_interface 
 * @param [in]  yield_interface 
 * @param [in]  os_queue 
 * @param [in]  os_delay 
 * @param [in]  os_thread 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
TempHumiHandlerStatus_t TempHumiHandler_Inst(  
                                        TempHumi_Handler_t * temp_humi_handler,
                            // TempHumiHandler_IICInterface_t * iic_handler_interface,
                            TempHumiHandler_ReplaceIICInterface_t * iic_handler_interface,
                    TempHumiHandler_TimeBaseInterface_t    * time_base_interface,
#ifdef OS_SUPPORTING
                    TempHumiHandler_RTOS_YieldInterface_t  * yield_interface,
                                TempHumiHandler_OsQueue_t  * os_queue,
                                TempHumiHandler_OsDelay_t  * os_delay,
                                TempHumiHandler_OsThread_t * os_thread);
#endif /* OS_SUPPORTING */


// /**
//  * @brief  温湿度处理 Handler 的事件发送函数
//  * 
//  * @param [in]  temp_humi_handler 
//  * @param [in]  temp_humi_event 
//  * @return      
//  * 
//  * @details
//  * @note
//  *      实际上由于发送事件由外部执行，所以该函数其实也没用了，由于操作系统一版不会换，所以上层直接使用
//  *          FreeRTOS 进行操作也无所谓了
//  * @warning
//  */
// static TempHumiHandlerStatus_t TempHumiHandler_SendTempHumiEvent(
//                                     TempHumi_Handler_t * temp_humi_handler,
//                                     TempHumiHandler_Event_t* temp_humi_event);


/**
 * @brief  温湿度处理 Handler 的事件接收函数
 * 
 * @param [in]  temp_humi_handler 
 * @param [in]  temp_humi_event 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
// TempHumiHandlerStatus_t TempHumiHandler_ReceiveTempHumiEvent(
//                                     TempHumi_Handler_t * temp_humi_handler,
//                                     TempHumiHandler_Event_t* temp_humi_event);


/**
 * @brief  温湿度 Handler 运行线程
 * 
 * 
 * @details
 * @note
 * @warning
 */
void TempHumiHandler_RunnableThread(void * argument);



// /**
//  * @brief  
//  * 
//  * @param [in]  temp_humi_handler 
//  * 
//  * @param [out]  temp 
//  * @param [out]  humi 
//  * @return      
//  * 
//  * @details
//  * @note
//  * @warning
//  */
// TempHumiHandlerStatus_t TempHumiHandler_UpdateTempHumiData(
//                                     TempHumi_Handler_t * temp_humi_handler,
//                                     float * temp,
//                                     float * humi );


// TempHumiHandlerStatus_t TempHumiHandler_GetTempHumiData(
//                                     TempHumi_Handler_t * temp_humi_handler,
//                                     TempHumiHandler_Event_t * event,
//                                     float * temp,
//                                     float * humi );



/*
 * 队列项目类型：
 *     TempHumiHandler_Event_t *
 *
 * 队列项目大小：
 *     sizeof(TempHumiHandler_Event_t *)
 *
 * 发送时：
 *     TempHumiHandler_Event_t *event_ptr = &event;
 *     PutQueue(queue_handle, &event_ptr, timeout);
 *
 * 注意：
 *     event 对象必须在 Handler 消费完成前保持有效，
 *     不能传递已经离开作用域的局部变量地址。
 */
void *TempHumiHandler_GetEventQueueHandle(void);

/**
 * @brief  外部获取最新 温湿度 数据
 * 
 * @param [in]  data 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
TempHumiHandlerStatus_t TempHumiHandler_GetLatestData( TempHumiData_t *data);




#endif /* ___BSP_TEMP_HUMI_SENSOR_HANDLER_H__ */



