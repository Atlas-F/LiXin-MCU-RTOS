/**
 * 
 * @file KeyLedStateMachine.c
 * @brief  按键控制LED 状态机实现
 * 
 * @author AtlasFg (phoenixfg@foxmail.com)
 * @date 2026-07-05
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

#include "KeyLedStateMachine.h"



/**
 * @brief LED  翻转多次
 * 
 * @param toggle_times  翻转次数
 */
void LedToggleSeveralTimes( uint8_t toggle_times ) 
{
    for( uint8_t times = 0 ; times < toggle_times ; times++)
    {
        HAL_GPIO_TogglePin( LED_GPIO_Port ,  LED_Pin);
        vTaskDelay(pdMS_TO_TICKS(500)); 
        HAL_GPIO_TogglePin( LED_GPIO_Port ,  LED_Pin);
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}