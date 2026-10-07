/**
 *
 * @file   BSP_LED_Handler.h
 * @brief LED 设备管理、事件调度及操作系统适配接口。
 *
 * @author AtlasFg
 * @date   2026-09-26
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
#ifndef BSP_LED_HANDLER_H
#define BSP_LED_HANDLER_H

/*---------- Private Includes -------------------------------------------------- */
#include <stdint.h>
#include <stdio.h>

#include "BSP_Led_Driver.h"

#include "System_configuration.h"
// #include "System_adaption.h"


/*----------- Private define -------------------------------------------------*/
#if 0
#define BSP_LED_HANDLER_MAX_INSTANCE_NUM  (10U)

#endif
/*----------- Private typedef ------------------------------------------------*/

/** @brief LED Handler 实例的前置声明。 */
typedef struct BspLedHandler Bsp_Led_Handler_t;

/* LED device driver list */
/** @brief Handler 管理的 LED 索引。 */
typedef enum
{
    LED_INDEX_1 = 0,
    LED_INDEX_2,
    LED_INDEX_3,
    LED_INDEX_4,
    LED_INDEX_5,
    LED_INDEX_6,
    LED_INDEX_7,
    LED_INDEX_8,
    LED_INDEX_9,
    MAX_INDEX_NUMBER,
    LED_NOT_INITIALIZED = UINT32_MAX // UINT32_MAX        // 0XFFFFFFFF
                                        /* 这里 MAX_INDEX_NUMBER 和 LED_NOT_INITIALIZED 的作用可以看做是一样的吧 ？*/
} BspLedHandlerIndex_t;


/** @brief Handler 初始化状态。 */
typedef enum
{
    LED_HANDLER_INITIALIZED     = 0,
    LED_HANDLER_NOT_INITIALIZED = 1
} BspLedHandlerInitialized_t;

/** @brief Handler 调用结果。 */
typedef enum
{
    LED_HANDLER_STATUS_OK                 = 0,    /* 操作成功 */
    LED_HANDLER_STATUS_ERROR              = 1,    /* 通用错误 */
    LED_HANDLER_STATUS_ERROR_TIMEOUT      = 2,    /* 操作超时 */
    LED_HANDLER_STATUS_ERROR_RESOURCE     = 3,    /* 资源已被占用或已初始化 */
    LED_HANDLER_STATUS_ERROR_PARAMETER    = 4,    /* 输入参数无效 */
    LED_HANDLER_STATUS_ERROR_NO_MEMORY    = 5,    /* 内存不足 */
    LED_HANDLER_STATUS_ERROR_ISR          = 6,    /* 在中断上下文中发生错误 */
    LED_HANDLER_STATUS_ERROR_NULL_POINTER = 7,    /* NULL 指针 */
    LED_HANDLER_STATUS_RESERVED           = 0xFF, /* 保留状态值 */

} BspLedHandlerStatus_t;


/**
 * @brief   LED 操作命令结构体
 *          在 LED 控制函数中用于发送操作命令
 * @details
 * @note
 *          命令参数需要后续修正
 * @warning
 */
/** @brief LED 控制事件及其目标设备。 */
typedef struct
{
    uint32_t                        cycle_time_ms; /**< 闪烁周期，单位 ms。 */
    uint32_t                          blink_times; /**< 闪烁次数。 */
    BspLedProportionOnOff_t     proportion_on_off; /**< 亮灭时间比例。 */
    BspLedHandlerIndex_t                led_index; /**< 目标 LED 索引。 */
} BspLedHandlerEvent_t;



/** @brief Handler 时基适配接口。 */
typedef struct
{
    BspLedStatus_t (*Bsp_Led_Handler_GetTimeMs)(uint32_t *const time_stamp); /* 获取当前毫秒时基接口 */

} BspLedHandlerTimeBaseMs_t;

#ifdef OS_SUPPORTING

/** @brief Handler 延时适配接口。 */
typedef struct
{
    BspLedStatus_t (*Bsp_Led_Handler_DelayMs)(uint32_t delay_time); /* 操作系统毫秒延时接口 */

} BspLedHandlerOsDelay_t;
#endif /* OS_SUPPORTING */

/* 不同的 LED 控制策略，控制之前的通过改变 LED 的闪烁和周期时间的策略，总体来说，应该算是一种控制策略，所谓的不同控制方式仅仅是对同一策略的微调 */
/* 这个优先级靠后，优先完成总体架构设计 */
// TBD：？
// typedef struct
// {

// } BspLedControlStrategy_t;


    /* 这里的 led control 的作用是什么？如果和 driver 层相同，为什么要放到 handler 层？
    如果不同，作用是什么？ */


/** 
 * @note
 * 这里的第一个参数类型应该是 BSP_Handler 层的，而不是 Driver 层的，因为要解耦，职责边界要明确
 */
/** @brief Handler 控制函数指针类型。 */
typedef BspLedHandlerStatus_t (*Bsp_Led_Handler_Control_t)(
                                                            Bsp_Led_Handler_t       *const self,
                                                            uint32_t                 cycle_time_ms,     /* 闪烁周期，单位：ms */
                                                            uint32_t                 blink_times,       /* 闪烁次数 */
                                                            BspLedProportionOnOff_t  proportion_on_off, /* LED 亮灭时间占比 */
                                                            BspLedHandlerIndex_t     led_index);

/** @brief LED 驱动注册函数指针类型。 */
typedef BspLedHandlerStatus_t (*Bsp_Led_Handler_Register_t)(
    Bsp_Led_Handler_t *const self,
    BspLedDriver_t *const    led_driver,
    BspLedHandlerIndex_t *const
        led_index /* 这里不是通过 led_index 返回数据，而是写入固定在该 index 位置中了  */
);
/* TODO：该部分接口还没有测试  */
/** 
 * @note:
 *      该部分接口用于创建 Queue 相关接口，由上层 RTOS 填充
 *      逻辑有点混淆了，原本设计是用于操作 LED Driver 设备列表的接口
 */
/** @brief 队列操作系统适配接口。 */
typedef struct
{
    /* TODO : Queue create、destroy、push、pop definition 或者是入栈、出栈
                待优化 */
    BspLedHandlerStatus_t (*Bsp_Led_Handler_CreateQueue)(uint32_t item_num,
                                                         uint32_t item_size,
                                                         void   **queue_handle); 
                                                         /* 这里返回句柄，后面要使用，应该要强转 */

    BspLedHandlerStatus_t (*Bsp_Led_Handler_DestroyQueue)(void **     queue_handle);

    /* BspLedHandlerStatus_t (*Bsp_Led_Handler_PutQueue)(BspLedDriver_t **instance_group)
    这里为什么使用 void * 类型，而不是已经定义好的 typedef 结构体？还是说对于函数指针来说，这样能得到解耦？ */
    BspLedHandlerStatus_t (*Bsp_Led_Handler_PutQueue)(void *const queue_handle,
                                                     void *const  queue_item,
                                                     uint32_t     timeout);

    BspLedHandlerStatus_t (*Bsp_Led_Handler_PopQueue)(void *const queue_handle,
                                                      void *const queue_item,
                                                      uint32_t    timeout);
} BspLedHandlerOsQueue_t;


/** @brief 线程操作系统适配接口。 */
typedef struct
{
    BspLedHandlerStatus_t (*Bsp_Led_Handler_CreateThread)(void             *create_task,
                                                          const char *const thread_name,
                                                          uint16_t          stack_depth,
                                                          void *const       parameters,
                                                          uint16_t          thread_priority,
                                                          void            **thread_handle);
    /*  void *const */

    BspLedHandlerStatus_t (*Bsp_Led_Handler_DestroyThread)(void * thread_handle);

} BspLedHandlerOsThread_t;



/** @brief 临界区操作适配接口。 */
typedef struct
{
    /* 这个临界区操作需要分 OS 和裸机吗？ */
    BspLedHandlerStatus_t (*Bsp_Led_Handler_CriticalEnter)(void); 
    BspLedHandlerStatus_t (*Bsp_Led_Handler_CriticalExit)(void);
} BspLedHandlerCriticalRegion_t;

/** @brief 已注册 LED 驱动实例数组。 */
typedef struct
{
    uint32_t instance_num; /**< 当前已注册实例数。 */
    BspLedDriver_t *instance_group[BSP_LED_HANDLER_MAX_INSTANCE_NUM]; /**< 驱动实例表。 */
} BspLedHandlerInstanceGroup_t;


/** @brief LED Handler 状态、资源和操作入口。 */
typedef struct BspLedHandler
{
    /**************** Target of internal status **** */
    uint8_t                                  initialized;   /* 驱动初始化状态 */
    BspLedHandlerIndex_t                       led_index;
    /**************** Target of Features **** */
    BspLedHandlerInstanceGroup_t * instance_group;
    BspLedHandlerCriticalRegion_t        critical_region;

    //TBD:
        void *                              queue_handle;   
        void *                             thread_handle;
    /**************** Target of IOs needs **** */
    /* ******* the interface of  Core Layer ** */
    BspLedHandlerTimeBaseMs_t *             time_base_ms;   /* 时基，单位：ms */

    /* ******* the interface of  RTOS  Layer ** */
#ifdef OS_SUPPORTING
    BspLedHandlerOsDelay_t *                    os_delay;   /* 操作系统延时函数 */
    //TODO：Queue
    BspLedHandlerOsQueue_t *                    os_queue;    /* 这里为什么定义为指针而不是结构体对象 ？ */
    BspLedHandlerOsThread_t *                   os_thread;
#endif /* OS_SUPPORTING */
    //TODO：Thread
    /**************** Target of APIs ********* */
    Bsp_Led_Handler_Control_t         Bsp_Led_Handler_Control;  /* LED 控制器 */
    Bsp_Led_Handler_Register_t Bsp_Led_Handler_RegisterDriver;
        /* TODO:更加合理的设置还应该包含一个 注销 方法，用于将指定设备从设备列表中注销
                防止对象销毁后注册表留下悬空指针 */

} Bsp_Led_Handler_t;

/*--------- Private declaring  ------------------------------------------------*/


 /**
  * @brief  BSP LED Handler 对象实例化
  * 
  * @param [in]  handler_self Handler 对象
  * @param [in]  time_base_ms 时基函数接口
  * @param [in]  os_delay     延时函数接口
  * @param [in]  os_queue     队列函数接口
  * @param [in]  os_thread    线程函数接口
  * @return      模块调用结果或错误状态。
  * 
  * @details
  * @note
  * @warning
  */
BspLedHandlerStatus_t Bsp_Led_Handler_Instantiate(Bsp_Led_Handler_t         *const handler_self,
                                                  BspLedHandlerTimeBaseMs_t *const time_base_ms,
#ifdef OS_SUPPORTING
                                                  BspLedHandlerOsDelay_t        *const os_delay,
                                                  BspLedHandlerOsQueue_t        *const os_queue,
                                                  BspLedHandlerOsThread_t             *os_thread
#endif /* OS_SUPPORTING */
);

/*--------- Private variables ------------------------------------------------*/



/*--------- Private function prototypes --------------------------------------*/

#endif /* BSP_LED_HANDLER_H */
