/**
 * ********************************************************************************
 * @file Bsp_Led.h
 * @brief 
 * 
 * @author AtlasFg (phoenixfg@foxmail.com)
 * @date 2026-07-16
 * @copyright Copyright (c) 2026
 * @version 0.1
 * 
 * @note 
 * @attention 
 * ********************************************************************************
 *  项目命名规范(v1.0) 
 *  局部变量                       : snake_case 
 *  全局变量                       : g_ + snake_case 
 *  静态变量                       : s_ + snake_case 
 *  指针                           : p_ 开头  
 *  函数                           : 模块前缀_PascalCase  (模块前缀大写_动词 + 名词)
 *  类型                           : PascalCase + _t 
 *  宏                             : UPPER_CASE 
 *  结构体/枚举/typedef 类型名      :PascalCase + _t 
 *  枚举值                         : eTypeName_VALUE 或 ePascalCase 
 * ********************************************************************************
 */

 #ifndef __BSP_LED_H__
 #define __BSP_LED_H__

/************************** private include  ************************************* */


// #include "main.h"

/* 为什么会加这么多文件？ */
#include <stdint.h>               // the compiling lib.
#include <stdio.h>
#include "main.h"
#include "cmsis_os.h"
#include "queue.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"



/************************** private typedef  ************************************* */


/**
 * @brief  LED 返回状态 值 枚举
 * 通过一个确定的枚举状态值来达到通用的函数返回，或者说，进行操作结果检查的目的，将
 * 
 */
typedef enum
{
  eLED_OK                = 0,           /* Operation completed successfully.  */
  eLED_ERROR             = 1,           /* Run-time error without case matched*/
  eLED_ERRORTIMEOUT      = 2,           /* Operation failed with timeout      */
  eLED_ERRORRESOURCE     = 3,           /* Resource not available.            */
  eLED_ERRORPARAMETER    = 4,           /* Parameter error.                   */
  eLED_ERRORNOMEMORY     = 5,           /* Out of memory.                     */
  eLED_ERRORISR          = 6,           /* Not allowed in ISR context         */
  eLED_RESERVED          = 0x7FFFFFFF   /* Reserved                           */    // LED 保留值或者初始值 设置

} LED_status_t;


/**
 * @brief LED 的执行事件枚举
 * 
 */
typedef enum
{
  eLED_ON                = 0,           /* Operation completed successfully.  */
  eLED_OFF               = 1,           /* Operation completed successfully.  */
  eLED_TOGGLE            = 2,           /* Operation completed successfully.  */
  eLED_BLINK_1_TIMES     = 3,           /* Operation completed successfully.  */
  eLED_BLINK_3_TIMES     = 4,           /* Operation completed successfully.  */
  eLED_BLINK_10_TIMES    = 5,           /* Operation completed successfully.  */
  eLED_INITED_VALUE      = 0xFF         /* Inited value                    .  */    // LED 执行事件 初始值 设置
} LED_operation_t;


/**
 * @brief 这里目前仅对当前开发板上仅有的 可控 LED PC13 进行控制，实际项目中不一定只有一个LED  \
 *        
 * 所以实际上可以设置 BSP_LED_ID_t id 给爱参数类型，控制指定的 LED 
 * 
 *          BSP_LED_ID_t id
 */
typedef enum 
{
    eLED_DEBUG = 0 ,                    // 调试 led
    eLED_PWR = 1                        // 电源指示灯
}   BSP_LED_ID_t ;



/************************** private variables  ************************************* */







/************************** private function declare  ******************************* */






/**
 * @brief  控制LED打开还是关闭
 * 
 * @param id 
 * @return LED_status_t 
 */
LED_status_t BSP_LED_On_Off(LED_operation_t operate);


/**
 * @brief 控制LED 翻转
 * 
 * @param id 
 * @return LED_status_t 
 */
LED_status_t BSP_LED_Toggle( void );






// bool BSP_LED_Init(BSP_LED_ID_t id);                 // 单个初始化
// LED_status_t BSP_LED_DeInit(BSP_LED_ID_t id);               // 去初始化（释放GPIO，用于低功耗）
// LED_status_t BSP_LED_InitAll(void);                         // 一次性初始化所有LED
// LED_status_t BSP_LED_Set(BSP_LED_ID_t id, bool state);      // 统一设置






 



#endif /* __BSP_LED_H__ */

