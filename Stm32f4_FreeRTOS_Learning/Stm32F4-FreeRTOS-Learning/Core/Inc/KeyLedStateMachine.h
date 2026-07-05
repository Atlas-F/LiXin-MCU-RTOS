/**
 * 
 * @file KeyLedStateMachine.h
 * @brief   按键控制LED 状态机实现方案
 * 
 * @author AtlasFg (phoenixfg@foxmail.com)
 * @date 2026-07-05
 * @copyright Copyright (c) 2026
 * @version 0.1
 * 
 * @note 
 * 
 *  单次按下按键,则实现LED的翻转。长按按键,则 LED 闪烁三次;
 * 
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


#ifndef __KEYLEDSTATEMACHINE_H__
#define __KEYLEDSTATEMACHINE_H__

#include "FreeRTOS.h"
#include "main.h"

/**
 * @brief LED 状态枚举
 * 
 */
typedef enum 
{
    eLED_IDLE = 0 ,             // 空闲状态，不做语义和状态列举
    eLED_ON ,                   // LED 亮起
    eLED_OFF                    // LED 熄灭
} LedState_t ;


/**
 * @brief  按键状态枚举
 * 
 */
typedef enum 
{
    eKEY_IDLE = 0 ,             // 空闲状态，不做语义和状态枚举
    eKEY_DOWN ,                 // 按键按下
    eKEY_UP ,                   // 按键未按下
    eKEY_FINISH ,               // 完成一次按键动作
    eKEY_LONGPRESS              // 长按，超过 1 秒
} keyState_t ;




/**
 * @brief  LED  翻转多次
 * 
 */
void LedToggleSeveralTimes( uint8_t toggle_times ) ; 





#endif /* __KEYLEDSTATEMACHINE_H__ */