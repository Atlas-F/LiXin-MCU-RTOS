/**
 * ********************************************************************************
 * @file KeyLedControl.h
 * @brief 按键控制LED的方法
 * 
 *      应用层、策略层的按键、LED 的方法实现
 * 
 * @author AtlasFg (phoenixfg@foxmail.com)
 * @date 2026-07-18
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

#ifndef __KEYLEDCONTROL_H__
#define __KEYLEDCONTROL_H__


/************************** private include  ************************************* */

#include <stdint.h>               // the compiling lib.
#include <stdio.h>

#include "main.h"
#include "cmsis_os.h"

#include "FreeRTOS.h"
#include "queue.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#include "../../Bsp/Bsp_Key.h"
#include "../../Bsp/Bsp_Led.h"


/************************** private typedef  ************************************* */





/**
 * @brief 按键中断触发信息 结构体
 * 
 * 
 * 
 */
typedef struct 
{
    GPIO_PinState KeyLevel ;
    TickType_t keyTick ;
} KeyIrqStatus_t ;







/************************** private variables  ************************************* */








/************************** private function declare  ******************************* */

void KeyTask( void *pvParameters  );
void LedTask( void *pvParameters  );


/*  */

/**
 * @brief 判断长按还是短按
 * 
 * @param press_time  按下时间
 * @return KEY_PressStatus_t 
 */
KEY_PressStatus_t KeyJudgeShortLongPress( uint32_t press_time );









#endif /* __KEYLEDCONTROL_H__ */

