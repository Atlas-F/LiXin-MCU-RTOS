/**
 * ********************************************************************************
 * @file Bsp_Key.h
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

#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__


/************************** private include  ************************************* */



#include <stdint.h>               // the compiling lib.
#include <stdio.h>

#include "main.h"
#include "cmsis_os.h"
#include "queue.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"





/************************** private typedef  ************************************* */


/**
 * @brief  KEY 返回状态 值 枚举
 * 通过一个确定的枚举状态值来达到通用的函数返回，或者说，进行操作结果检查的目的，将
 * 
 */
typedef enum
{
  eKEY_OK                = 0,           /* Operation completed successfully.  */
  eKEY_ERROR             = 1,           /* Run-time error without case matched*/
  eKEY_ERRORTIMEOUT      = 2,           /* Operation faiKEY with timeout      */
  eKEY_ERRORRESOURCE     = 3,           /* Resource not available.            */
  eKEY_ERRORPARAMETER    = 4,           /* Parameter error.                   */
  eKEY_ERRORNOMEMORY     = 5,           /* Out of memory.                     */
  eKEY_ERRORISR          = 6,           /* Not allowed in ISR context         */
  eKEY_RESERVED          = 0x7FFFFFFF   /* Reserved                           */    // KEY 保留值或者初始值 设置

} KEY_Status_t;


/**
 * @brief KEY 的执行事件枚举
 * 
 */
typedef enum
{
  eKEY_PRESSED           = 0,           /* Operation completed successfully.  */
  eKEY_NOT_PRESSED       = 1,           /* Run-time error without case matched*/
  eKEY_SHORT_PRESSED     = 2,           /* Operation                          */
  eKEY_LONG_PRESSED      = 3,           /* Operation                          */
} KEY_PressStatus_t;


/**
 * @brief 这里目前仅对当前开发板上仅有的 可控 KEY PA0 进行控制，实际项目中不一定只有一个KEY  \
 *        
 * 所以实际上可以设置 BSP_KEY_ID_t id 给参数类型，控制指定的 KEY 
 * 
 *          BSP_KEY_ID_t id
 */
typedef enum 
{
    eKEY_DEBUG = 0 ,                    // 调试 KEY
    eKEY_PWR = 1                        // 电源指示灯
}   BSP_KEY_ID_t ;






/************************** private variables  ************************************* */








/************************** private function declare  ******************************* */



/**
 * @brief 按键扫描，判断是否按下，读取按键值，并通过形参将数据 传入和传出
 * 
 * @param key_value 
 * @return KEY_Status_t 返回操作状态 结果，成功或者超时
 */
KEY_Status_t keyScan( KEY_PressStatus_t * key_value);




/**
 * - 初始化函数
 * - 反初始化函数
 * 
 */













#endif /* __BSP_KEY_H__ */


