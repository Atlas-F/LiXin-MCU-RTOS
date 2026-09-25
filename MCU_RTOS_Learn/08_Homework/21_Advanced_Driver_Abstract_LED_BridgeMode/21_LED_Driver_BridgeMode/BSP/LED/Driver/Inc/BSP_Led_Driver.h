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

#ifndef __BSP_LED_DRIVER__
#define __BSP_LED_DRIVER__

/*---------- Private Includes -------------------------------------------------- */
#include <stdint.h>
#include <stdio.h>

/*----------- Private define -------------------------------------------------*/

#define INITED 1     /* 已完成初始化 */
#define NOT_INITED 0 /* 尚未初始化 */

#define OS_SUPPORTING /* OS_SUPPORTING depending on OS avaliable */
#define DEBUG

#define DEBUG_OUT(X) printf(X) /* 自定义 debug 输出方式 */

#define DEBUG_UNITY_TEST

/*----------- Private typedef ------------------------------------------------*/

typedef struct BSP_LedDriver BSP_LedDriver_t ;


typedef enum
{
    LED_OK                 = 0,    /* 操作成功 */
    LED_ERROR              = 1,    /* 通用错误 */
    LED_ERROR_TIMEOUT      = 2,    /* 操作超时 */
    LED_ERROR_RESOURCE     = 3,    /* 资源已被占用或已初始化 */
    LED_ERROR_PARAMETER    = 4,    /* 输入参数无效 */
    LED_ERFOR_NOMEMORY     = 5,    /* 内存不足 */
    LED_ERROR_ISR          = 6,    /* 在中断上下文中发生错误 */
    LED_ERROR_NULL_POINTER = 7,    /* NULL 指针 */
    LED_RESERVED           = 0xFF, /* 保留状态值 */

} Led_Status_t;

typedef enum
{
    ePROPORTION_1_3 = 0,                        /* LED 亮灭时间比为 1:3 */
    ePROPORTION_1_2 = 1,                        /* LED 亮灭时间比为 1:2 */
    ePROPORTION_1_1 = 2,                        /* LED 亮灭时间比为 1:1 */
    ePROPORTION_X_X = 0xFF                      /* 未设置或无效的亮灭时间比 */
} Proportion_OnOff_t;

typedef struct
{
    Led_Status_t (*pf_Led_On)(void);            /* LED 点亮操作接口 */
    Led_Status_t (*pf_Led_Off)(void);           /* LED 熄灭操作接口 */
} Led_Operations_t;

typedef struct
{
    Led_Status_t (*pf_Get_Time_ms)(uint32_t *const); /* 获取当前毫秒时基接口 */

} Time_Base_ms_t;

#ifdef OS_SUPPORTING
typedef struct
{
    Led_Status_t (*pf_OS_Delay_ms)(const uint32_t); /* 操作系统毫秒延时接口 */

} OS_Delay_t;
#endif /* OS_SUPPORTING */

typedef Led_Status_t (*pf_Led_Control_t)(BSP_LedDriver_t *const             self,
                                         uint32_t,                                  /* 闪烁周期，单位：ms */
                                         uint32_t,                                  /* 闪烁次数 */
                                         Proportion_OnOff_t                         /* LED 亮灭时间占比 */
);

typedef struct BSP_LedDriver
{
    /**************** Target of internal status **** */
    uint8_t is_inited; /* 驱动初始化状态 */

    /**************** Target of Features **** */
    /* blink cycle time*/
    uint32_t                        cycle_time_ms;      /* 闪烁周期 */
    /* LED  blink times */
    uint32_t                          blink_times;      /* 闪烁次数 */
    /* light On or Off */
    Proportion_OnOff_t          proportion_on_off;      /* 亮灭时间占比 */

    /**************** Target of IOs needs **** */
    /* ******* the interface of  Core Layer    */
    Led_Operations_t            *p_Led_OperaInst;       /* LED 操作实例化 */
    Time_Base_ms_t                *p_TimeBase_ms;       /* 时基，单位：ms */

    /* ******* the interface of  RTOS  Layer    */
#ifdef OS_SUPPORTING
    OS_Delay_t                     *p_OS_TimeDelay;     /* 操作系统延时函数 */
#endif /* OS_SUPPORTING */
    /**************** Target of APIs ********* */
    pf_Led_Control_t              pf_LedController;     /* LED 控制器 */

} BSP_LedDriver_t;

/*--------- Private declaring  ------------------------------------------------*/

/**
 * @brief   Instiantiate the target of BSP_Led_Driver  实例化
 *
 * Steps:
    1, Adding the Core interfaces into target of bsp_led_driver instance.
    2, Adding the OS interfaces into target of bsp_ledd_driver instance.
    3, Adding the timebase interfaces into targetof bsp_led_driver instance.
 * @param [in]  self
 * @param [in]  led_Ops
 * @param [in]  time_base
 * @param [in]  OS_delay
 * @return      Led_Status_t  the status of running
 *
 * @details
 * @note
 * @warning
 */
Led_Status_t LedDriver_inst(BSP_LedDriver_t *const  self,
                            Led_Operations_t *const led_Ops,
                            Time_Base_ms_t *const   time_base,
#ifdef OS_SUPPORTING
                            OS_Delay_t *const OS_delay
#endif /* OS_SUPPORTING */
);




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
Led_Status_t LED_Control(   BSP_LedDriver_t *const                   self,
                            uint32_t                        cycle_time_ms,    
                            uint32_t                          blink_times,  
                            Proportion_OnOff_t          proportion_on_off );

/*--------- Private variables ------------------------------------------------*/

/*--------- Private function prototypes --------------------------------------*/

#endif /* __BSP_LED_DRIVER__ */
