/**
 * 
 * @file   System_adaption.h
 * @brief  将 LED BSP、Handler 与 FreeRTOS 适配接口连接起来。
 * 
 * @author AtlasFg
 * @date   2026-10-03
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
#ifndef __SYSTEM_ADAPTION__
#define __SYSTEM_ADAPTION__


/* *********************************** Includes ***************************** */



#include "BSP_Led_Driver.h"
#include "BSP_LED_Handler.h"

/* *********************************** Typedef ***************************** */

typedef enum
{
    SYS_ADAPT_STATUS_OK                 = 0,    /* 操作成功 */
    SYS_ADAPT_STATUS_ERROR              = 1,    /* 通用错误 */
    SYS_ADAPT_STATUS_ERROR_TIMEOUT      = 2,    /* 操作超时 */
    SYS_ADAPT_STATUS_ERROR_RESOURCE     = 3,    /* 资源已被占用或已初始化 */
    SYS_ADAPT_STATUS_ERROR_PARAMETER    = 4,    /* 输入参数无效 */
    SYS_ADAPT_STATUS_ERROR_NO_MEMORY    = 5,    /* 内存不足 */
    SYS_ADAPT_STATUS_ERROR_ISR          = 6,    /* 在中断上下文中发生错误 */
    SYS_ADAPT_STATUS_ERROR_NULL_POINTER = 7,    /* NULL 指针 */
    SYS_ADAPT_STATUS_RESERVED           = 0xFF, /* 保留状态值 */

} SystemAdaptStatus_t;


/* *********************************** Defines ****************************** */









/* ******************************* Declaration ****************************** */



// static BspLedStatus_t BspLedDemo_On(void);

// static BspLedStatus_t BspLedDemo_Off(void);

// static BspLedStatus_t BspLedDemo_GetTimeMs(uint32_t *const time_stamp);

// static BspLedStatus_t BspLedDemo_DelayMs(uint32_t delay_time);

/** @brief 初始化 LED 驱动及 Handler 适配层。 @return 系统适配状态。 */
SystemAdaptStatus_t SystemAdaption_LedInit(void);

















#endif /* __SYSTEM_ADAPTION__ */

