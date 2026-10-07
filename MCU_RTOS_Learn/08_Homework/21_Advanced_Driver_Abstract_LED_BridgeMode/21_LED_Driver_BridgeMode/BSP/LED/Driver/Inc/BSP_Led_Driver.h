/**
 *
 * @file   BSP_Led_Driver.h
 * @brief LED 底层驱动接口与设备实例定义。
 *
 * @author AtlasFg
 * @date   2026-09-23
 * @version 1.0
 *
 * @par 说明
 * 本文件定义 LED Driver 的设备抽象。Driver 不直接访问 GPIO 或 FreeRTOS，
 * 而是保存由系统适配层注入的亮灯、灭灯、时基和延时回调。
 * 调用关系为：业务或 Handler -> Driver 控制接口 -> 适配层回调 -> 硬件或 RTOS。
 *
 * @note 注意事项
 * 传入实例化函数的接口对象只保存地址、不进行拷贝；接口对象和其中的函数
 * 指针必须在 Driver 实例的整个使用周期内保持有效。
 *
 * @warning 参考
 * 当前闪烁参数保存在 Driver 中。同一实例被多个任务同时控制时，需要由 Handler
 * 或上层实现串行化或互斥保护。
 * ********************************************************************************
 */

#ifndef BSP_LED_DRIVER_H
#define BSP_LED_DRIVER_H

/*---------- Private Includes -------------------------------------------------- */
#include <stdint.h>
#include <stdio.h>

#include "System_configuration.h"

// #include "System_adaption.h"


/*----------- Private define -------------------------------------------------*/

#if 0
#define BSP_LED_INITIALIZED            1U     /* 已完成初始化 */
#define BSP_LED_NOT_INITIALIZED        0U     /* 尚未初始化 */

#define OS_SUPPORTING                  /* OS_SUPPORTING depending on OS avaliable */
#define DEBUG_ENABLED

#define DEBUG_OUT(message)     printf(message) /* 自定义 debug 输出方式 */

#define BSP_LED_DEBUG_UNITY_TEST

#endif

/*----------- Private typedef ------------------------------------------------*/

/** @brief LED 驱动实例的前置声明。 */
typedef struct BspLedDriver BspLedDriver_t;


/** @brief LED 驱动调用结果。 */
typedef enum
{
    LED_STATUS_OK                 = 0,    /* 操作成功 */
    LED_STATUS_ERROR              = 1,    /* 通用错误 */
    LED_STATUS_ERROR_TIMEOUT      = 2,    /* 操作超时 */
    LED_STATUS_ERROR_RESOURCE     = 3,    /* 资源已被占用或已初始化 */
    LED_STATUS_ERROR_PARAMETER    = 4,    /* 输入参数无效 */
    LED_STATUS_ERROR_NO_MEMORY    = 5,    /* 内存不足 */
    LED_STATUS_ERROR_ISR          = 6,    /* 在中断上下文中发生错误 */
    LED_STATUS_ERROR_NULL_POINTER = 7,    /* NULL 指针 */
    LED_STATUS_RESERVED           = 0xFF, /* 保留状态值 */

} BspLedStatus_t;

/** @brief LED 亮灭时间比例。 */
typedef enum
{
    LED_PROPORTION_1_3 = 0,                        /* LED 亮灭时间比为 1:3 */
    LED_PROPORTION_1_2 = 1,                        /* LED 亮灭时间比为 1:2 */
    LED_PROPORTION_1_1 = 2,                        /* LED 亮灭时间比为 1:1 */
    LED_PROPORTION_INVALID = 0xFF                  /* 未设置或无效的亮灭时间比 */
} BspLedProportionOnOff_t;

/** @brief LED 物理开关操作回调集合。 */
typedef struct
{
    BspLedStatus_t (*BspLed_On)(void);             /* LED 点亮操作接口 */
    BspLedStatus_t (*BspLed_Off)(void);            /* LED 熄灭操作接口 */
} BspLedOperations_t;

/** @brief LED 毫秒时基回调集合。 */
typedef struct
{
    BspLedStatus_t (*BspLed_GetTimeMs)(uint32_t *const time_stamp); /* 获取当前毫秒时基接口 */

} BspLedTimeBaseMs_t;

#ifdef OS_SUPPORTING
/** @brief 操作系统延时回调集合。 */
typedef struct
{
    BspLedStatus_t (*BspLed_DelayMs)(uint32_t delay_time); /* 操作系统毫秒延时接口 */

} BspLedOsDelay_t;
#endif /* OS_SUPPORTING */

/** @brief LED 驱动控制函数指针类型。 */
typedef BspLedStatus_t (*BspLedDriverControl_t)(BspLedDriver_t *const self,
                                                 uint32_t    cycle_time_ms, /* 闪烁周期，单位：ms */
                                                 uint32_t      blink_times, /* 闪烁次数 */
                                 BspLedProportionOnOff_t proportion_on_off  /* LED 亮灭时间占比 */
);

/** @brief LED 驱动实例及其依赖接口。 */
typedef struct BspLedDriver
{
    /**************** Target of internal status **** */
    uint8_t                                initialized;     /* 驱动初始化状态 */

    /**************** Target of Features **** */
    /* blink cycle time*/
    uint32_t                             cycle_time_ms;     /* 闪烁周期 */
    /* LED  blink times */
    uint32_t                               blink_times;     /* 闪烁次数 */
    /* light On or Off */
    BspLedProportionOnOff_t          proportion_on_off;     /* 亮灭时间占比 */

    /**************** Target of IOs needs **** */
    /* ******* the interface of  Core Layer    */
    BspLedOperations_t                     *operations;     /* LED 操作实例化 */
    BspLedTimeBaseMs_t                   *time_base_ms;     /* 时基，单位：ms */

    /* ******* the interface of  RTOS  Layer    */
#ifdef OS_SUPPORTING
    BspLedOsDelay_t                          *os_delay;     /* 操作系统延时函数 */
#endif /* OS_SUPPORTING */
    /**************** Target of APIs ********* */
    BspLedDriverControl_t         BspLedDriver_Control;     /* LED 控制器 */

} BspLedDriver_t;

/*--------- Private declaring  ------------------------------------------------*/


/**
 * @brief   Instiantiate the target of BSP_Led_Driver  实例化
 *
 * Steps:
    1, Adding the Core interfaces into target of bsp_led_driver instance.
    2, Adding the OS interfaces into target of bsp_ledd_driver instance.
    3, Adding the timebase interfaces into targetof bsp_led_driver instance.
 * @param[in,out] self         驱动对象实例；输入时不能为 NULL，函数会写入其接口指针、
 *                              初始闪烁参数和初始化状态。
 * @param[in]     operations   LED 核心操作接口，必须指向有效的 BspLedOperations_t 对象，
 *                              并提供 BspLed_On 与 BspLed_Off 回调函数。
 * @param[in]     time_base_ms 毫秒时基接口，必须指向有效的 BspLedTimeBaseMs_t 对象，
 *                              并提供用于读取当前时间的 BspLed_GetTimeMs 回调函数。
 * @param[in]     os_delay     操作系统    延时接口，必须指向有效的 BspLedOsDelay_t 对象，
 *                              并提供毫秒延时的 BspLed_DelayMs 回调函数；仅在定义
 *                              OS_SUPPORTING 时生效。
 * @return      LED_STATUS_OK              实例化成功。
 * @return      LED_STATUS_ERROR_PARAMETER self、operations、time_base_ms 或 os_delay 为 NULL。
 * @return      LED_STATUS_ERROR_RESOURCE  self 对象已经完成实例化。
 * @return      其他 BspLedStatus_t        驱动初始化流程返回的错误状态。
 *
 * @details
 *  函数成功后，@p self 会持有上述接口对象的地址，因此这些对象在驱动使用期间
 *  必须保持有效；函数不会复制接口对象本身。
 * @note      调用前应将 self 指向的 BspLedDriver_t 对象清零或设置为 BSP_LED_NOT_INITIALIZED。
 * @warning   传入的接口对象及其函数指针不得为 NULL，且生命周期必须覆盖驱动实例的使用期。
 */
BspLedStatus_t BspLedDriver_Instantiate(BspLedDriver_t             *const self,
                                         BspLedOperations_t  *const operations, 
                                         BspLedTimeBaseMs_t *const time_base_ms, 
                                         BspLedOsDelay_t        *const os_delay 
);


/** @brief 点亮 LED。 @param[in] self 已初始化的驱动实例。 @return 操作状态。 */
BspLedStatus_t BspLedDriver_On(BspLedDriver_t *self);


/** @brief 熄灭 LED。 @param[in] self 已初始化的驱动实例。 @return 操作状态。 */
BspLedStatus_t BspLedDriver_Off(BspLedDriver_t *self);


/*--------- Private variables ------------------------------------------------*/

/*--------- Private function prototypes --------------------------------------*/

#endif /* BSP_LED_DRIVER_H */
