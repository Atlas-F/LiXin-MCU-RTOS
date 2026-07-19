/**
 * ********************************************************************************
 * @file Bsp_Led.c
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

#include "Bsp_Led.h"






/************************** private function definition  ******************************* */


/**
 * @brief 控制 LED开关或者是不同的操作方式
 * 
 * @param id 
 * @param operate 
 * @return LED_status_t 
 * 
 * @note    这里目前仅对当前开发板上仅有的 可控 LED PC13 进行控制，实际项目中不一定只有一个LED  \
 *          所以实际上可以设置 BSP_LED_ID_t id 给爱参数类型，控制指定的 LED 
 * 
 *          BSP_LED_ID_t id
 * 
 */
LED_status_t BSP_LED_On_Off( LED_operation_t operate)
{
    switch ( operate )
    {
        case eLED_ON:
            HAL_GPIO_WritePin( LED_GPIO_Port, LED_Pin , GPIO_PIN_RESET);
            break;

        case eLED_OFF:
            HAL_GPIO_WritePin( LED_GPIO_Port, LED_Pin , GPIO_PIN_SET);
            break;
        
        default:
            break;
    }
    return eLED_OK ;
}





/**
 * @brief LED  翻转
 * 
 *  以项目中已经写好的 LED 为例
 * 
 * @return LED_status_t 
 */
LED_status_t BSP_LED_Toggle( void )
{
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    return eLED_OK ;
}
