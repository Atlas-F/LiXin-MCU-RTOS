/**
 * 
 * @file   System_configuration.h
 * @brief  
 * 
 * @author AtlasFg
 * @date   2026-10-04
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

 #ifndef __SYSTEM_CONFIGURATION_H__
 #define __SYSTEM_CONFIGURATION_H__

    #define BSP_LED_INITIALIZED               1U     /* 已完成初始化 */
    #define BSP_LED_NOT_INITIALIZED           0U     /* 尚未初始化 */

    #define OS_SUPPORTING                     /* OS_SUPPORTING depending on OS avaliable */

    #define DEBUG_ENABLED

    #define DEBUG_OUT(message)                printf(message) /* 自定义 debug 输出方式 */

    #define BSP_LED_DEBUG_UNITY_TEST

    #define BSP_LED_HANDLER_MAX_INSTANCE_NUM  (10U)







 #endif /* __SYSTEM_CONFIGURATION_H__ */

