/**
 *
 * @file   BSP_Led_Driver.h
 * @brief
 *
 * @author AtlasFg
 * @date   2026-09-23
 * @version 1.0
 *
 * @par 说明
 * *
 * @note 注意事项
 *              该代码方案起始还有很大的参考空间
 * *
 * @warning 参考
 * *
 * *
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

typedef struct BspLedDriver BspLedDriver_t;


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

typedef enum
{
    LED_PROPORTION_1_3 = 0,                        /* LED 亮灭时间比为 1:3 */
    LED_PROPORTION_1_2 = 1,                        /* LED 亮灭时间比为 1:2 */
    LED_PROPORTION_1_1 = 2,                        /* LED 亮灭时间比为 1:1 */
    LED_PROPORTION_INVALID = 0xFF                  /* 未设置或无效的亮灭时间比 */
} BspLedProportionOnOff_t;

typedef struct
{
    BspLedStatus_t (*BspLed_On)(void);             /* LED 点亮操作接口 */
    BspLedStatus_t (*BspLed_Off)(void);            /* LED 熄灭操作接口 */
} BspLedOperations_t;

typedef struct
{
    BspLedStatus_t (*BspLed_GetTimeMs)(uint32_t *const time_stamp); /* 获取当前毫秒时基接口 */

} BspLedTimeBaseMs_t;

#ifdef OS_SUPPORTING
typedef struct
{
    BspLedStatus_t (*BspLed_DelayMs)(uint32_t delay_time); /* 操作系统毫秒延时接口 */

} BspLedOsDelay_t;
#endif /* OS_SUPPORTING */

typedef BspLedStatus_t (*BspLedDriverControl_t)(BspLedDriver_t *const self,
                                                 uint32_t cycle_time_ms,       /* 闪烁周期，单位：ms */
                                                 uint32_t blink_times,         /* 闪烁次数 */
                                                 BspLedProportionOnOff_t proportion_on_off /* LED 亮灭时间占比 */
);

typedef struct BspLedDriver
{
    /**************** Target of internal status **** */
    uint8_t initialized; /* 驱动初始化状态 */

    /**************** Target of Features **** */
    /* blink cycle time*/
    uint32_t cycle_time_ms;                     /* 闪烁周期 */
    /* LED  blink times */
    uint32_t blink_times;                       /* 闪烁次数 */
    /* light On or Off */
    BspLedProportionOnOff_t proportion_on_off;  /* 亮灭时间占比 */

    /**************** Target of IOs needs **** */
    /* ******* the interface of  Core Layer    */
    BspLedOperations_t *operations;             /* LED 操作实例化 */
    BspLedTimeBaseMs_t *time_base_ms;           /* 时基，单位：ms */

    /* ******* the interface of  RTOS  Layer    */
#ifdef OS_SUPPORTING
    BspLedOsDelay_t *os_delay;                  /* 操作系统延时函数 */
#endif /* OS_SUPPORTING */
    /**************** Target of APIs ********* */
    BspLedDriverControl_t BspLedDriver_Control; /* LED 控制器 */

} BspLedDriver_t;

/*--------- Private declaring  ------------------------------------------------*/

/**
 * @brief  Do the action of LED blink
 *
 * @param [in]  self
 * @return
 *
 * @details
 * @note
 * @warning
 */
// static BspLedStatus_t BspLedDriver_Blink(BspLedDriver_t *const self);



/**
 * @brief   Control the LED , and do the action
 *
 * @param [in]  self
 * @param [in]  cycle_time_ms
 * @param [in]  blink_times
 * @param [in]  proportion_on_off
 * @return
 *
 * @details
 * @note
 * @warning
 */
// BspLedStatus_t BspLedDriver_Control(BspLedDriver_t *const self,
//                                      uint32_t cycle_time_ms,
//                                      uint32_t blink_times,
//                                      BspLedProportionOnOff_t proportion_on_off);


/**
 * @brief  Init the target BspLedDriver_t
 *
 * @param [in]  self pointer of input data
 * @return    BspLedStatus_t the status of running
 *
 * @details
 * @note        这个函数有待优化！
 * @warning
 */
// static BspLedStatus_t BspLedDriver_Initialize(BspLedDriver_t *const self);







/**
 * @brief   Instiantiate the target of BSP_Led_Driver  实例化
 *
 * Steps:
    1, Adding the Core interfaces into target of bsp_led_driver instance.
    2, Adding the OS interfaces into target of bsp_ledd_driver instance.
    3, Adding the timebase interfaces into targetof bsp_led_driver instance.
 * @param [in]  self
 * @param [in]  operations
 * @param [in]  time_base_ms
 * @param [in]  os_delay
 * @return      BspLedStatus_t  the status of running
 *
 * @details
 * @note
 * @warning
 */
// BspLedStatus_t BspLedDriver_Instantiate(BspLedDriver_t *const self,
//                                          BspLedOperations_t *const operations,
//                                          BspLedTimeBaseMs_t *const time_base_ms,
// #ifdef OS_SUPPORTING
//                                          BspLedOsDelay_t *const os_delay
// #endif /* OS_SUPPORTING */
// );




/**
 * @brief   Control the LED , and do the action
 *
 * @param [in]  self
 * @param [in]  cycle_time_ms
 * @param [in]  blink_times
 * @param [in]  proportion_on_off
 * @return
 *
 * @details
 * @note
 * @warning
 */
// BspLedStatus_t BspLedDriver_Control(BspLedDriver_t *const self,
//                                      uint32_t cycle_time_ms,
//                                      uint32_t blink_times,
//                                      BspLedProportionOnOff_t proportion_on_off);

/*--------- Private variables ------------------------------------------------*/

/*--------- Private function prototypes --------------------------------------*/

#endif /* BSP_LED_DRIVER_H */
